
#include "UniformStructsProcessor.h"

#include "spirv_reflect.hpp"

namespace shadertool {

namespace {
std::string Preamble = R"(
#pragma once

#include <cstdint>
#include <array>

)";

std::string makeCppIdentifier(const std::string& name)
{
	auto replace = [](char c) {
		if (isalnum(c)) {
			return c;
		}
		return '_';
	};

	std::string ident = name;
	std::transform(ident.begin(), ident.end(), ident.begin(), replace);
	return ident;
}
std::string getBaseCppType(const spirv_cross::SPIRType& type)
{
	switch (type.basetype) {
	case spirv_cross::SPIRType::Boolean:
		// Output booleans as uints to ensure the size is consistent across systems/compilers
		return "std::uint32_t";
	case spirv_cross::SPIRType::SByte:
		return "std::int8_t";
	case spirv_cross::SPIRType::UByte:
		return "std::uint8_t";
	case spirv_cross::SPIRType::Short:
		return "std::int16_t";
	case spirv_cross::SPIRType::UShort:
		return "std::uint16_t";
	case spirv_cross::SPIRType::Int:
		return "std::int32_t";
	case spirv_cross::SPIRType::UInt:
		return "std::uint32_t";
	case spirv_cross::SPIRType::Int64:
		return "std::int64_t";
	case spirv_cross::SPIRType::UInt64:
		return "std::uint64_t";
	case spirv_cross::SPIRType::Float:
		return "float";
	case spirv_cross::SPIRType::Double:
		return "double";
	default:
		throw std::runtime_error("Unsupported type in uniform buffer.");
	}
}
std::string getCppType(const spirv_cross::SPIRType& type)
{
	auto basicType = getBaseCppType(type);

	if (type.vecsize == 1 && type.columns == 1) {
		// Basic type, nothing special here
		return basicType;
	}

	switch (type.basetype) {
	case spirv_cross::SPIRType::Float:
		if (type.vecsize > 1 && type.columns == 1) {
			// Simple array type
			return "SPIRV_FLOAT_VEC" + std::to_string(type.vecsize);
		}

		return "SPIRV_FLOAT_MAT_" + std::to_string(type.vecsize) + "x" + std::to_string(type.columns);
	default:
		throw std::runtime_error("Unsupported vector or matrix type.");
	}
}
std::string getStructTypeName(const std::string& uboName, const std::filesystem::path& filePath)
{
	auto filename = filePath.filename();
	// Remove one level to remove the .spv extension
	filename.replace_extension();

	return uboName + "_" + makeCppIdentifier(filename.u8string());
}
uint64_t getMemberCppSize(const spirv_cross::SPIRType& type)
{
	uint64_t baseSize;

	switch (type.basetype) {
	case spirv_cross::SPIRType::SByte:
	case spirv_cross::SPIRType::UByte:
		baseSize = 1;
		break;
	case spirv_cross::SPIRType::Short:
	case spirv_cross::SPIRType::UShort:
		baseSize = 2;
		break;
	case spirv_cross::SPIRType::Boolean:
	case spirv_cross::SPIRType::Int:
	case spirv_cross::SPIRType::UInt:
	case spirv_cross::SPIRType::Float:
		baseSize = 4;
		break;
	case spirv_cross::SPIRType::Int64:
	case spirv_cross::SPIRType::UInt64:
	case spirv_cross::SPIRType::Double:
		baseSize = 8;
		break;
	default:
		throw std::runtime_error("Unsupported type in uniform buffer.");
	}

	return baseSize * type.vecsize * type.columns;
}
} // namespace

UniformStructsProcessor::UniformStructsProcessor()
	: BaseShaderProcessor("structs", "Output C++ structs for uniform buffer types")
{
}
UniformStructsProcessor::~UniformStructsProcessor() = default;

void UniformStructsProcessor::addOptions(CLI::App& app)
{
	BaseShaderProcessor::addOptions(app);

	app.add_option_function<std::string>(
		   "--structs-output",
		   [this](const std::string& path) { m_outputPath = std::filesystem::u8path(path); },
		   "Path of the file to write the GLSL code to.")
		->check(CLI::ExistingFile | CLI::NonexistentPath);
}
bool UniformStructsProcessor::processShader(const std::filesystem::path& shaderPath,
	const std::vector<uint32_t>& spirvCode)
{
	spirv_cross::Compiler compiler(spirvCode);

	std::stringstream buffer;

	auto resources = compiler.get_shader_resources();
	auto ubos = resources.uniform_buffers;

	buffer << Preamble;

	for (const auto& ubo : ubos) {
		auto cppTypeName = getStructTypeName(ubo.name, shaderPath);
		buffer << "struct " << cppTypeName << " {\n";

		auto type = compiler.get_type(ubo.base_type_id);

		uint64_t currentCppOffset = 0;
		uint64_t numPads = 0;

		std::vector<std::tuple<std::string, uint64_t>> memberOffsets;

		for (uint32_t i = 0; i < type.member_types.size(); ++i) {
			auto& membertype = compiler.get_type(type.member_types[i]);
			const auto& name = compiler.get_member_name(ubo.base_type_id, i);

			const auto memberOffset = compiler.type_struct_member_offset(type, i);

			if (memberOffset > currentCppOffset) {
				buffer << "\tuint8_t _padding" << numPads << "[" << memberOffset - currentCppOffset << "];\n";

				++numPads;
				currentCppOffset = memberOffset;
			}

			buffer << "\t" << getCppType(membertype) << " " << name << ";\n";

			currentCppOffset += getMemberCppSize(membertype);
			memberOffsets.emplace_back(name, memberOffset);
		}

		auto totalSize = compiler.get_declared_struct_size(type);
		if (currentCppOffset < totalSize) {
			buffer << "\tuint8_t _padding" << numPads << "[" << totalSize - currentCppOffset << "];\n";
		}

		buffer << "};\n";

		// Now output some code to check the size
		buffer << "static_assert(sizeof(" << cppTypeName << ") == " << totalSize << ", \"Size of struct " << cppTypeName
			   << " does not match what is expected for the uniform block!\");\n";
		for (const auto& [name, offset] : memberOffsets) {
			buffer << "static_assert(offsetof(" << cppTypeName << ", " << name << ") == " << offset << ", \"Offset of member "
				   << name << " does not match the uniform buffer offset!\");\n";
		}
	}

	std::ofstream out(m_outputPath, std::ios::binary | std::ios::trunc);
	if (!out.good()) {
		throw std::runtime_error("Failed to open " + m_outputPath.u8string());
	}

	out << buffer.str();

	return true;
}

} // namespace shadertool
