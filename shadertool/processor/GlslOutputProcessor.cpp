
#include "GlslOutputProcessor.h"

#include "spirv_glsl.hpp"

namespace shadertool {

GlslOutputProcessor::GlslOutputProcessor() : BaseShaderProcessor("glsl", "Output GLSL shader compatible with OpenGL") {}
GlslOutputProcessor::~GlslOutputProcessor() = default;

void GlslOutputProcessor::addOptions(CLI::App& app)
{
	BaseShaderProcessor::addOptions(app);

	app.add_option_function<std::string>(
		   "--glsl-output",
		   [this](const std::string& path) { m_glslOutputPath = std::filesystem::u8path(path); },
		   "Path of the file to write the GLSL code to.")
		->check(CLI::ExistingFile | CLI::NonexistentPath);
}
bool GlslOutputProcessor::processShader(const std::filesystem::path& /*shaderPath*/, const std::vector<uint32_t>& spirvCode)
{
	spirv_cross::CompilerGLSL glsl(spirvCode);

	spirv_cross::CompilerGLSL::Options options;
	options.version = 150;
	options.es = false;
	options.enable_420pack_extension = false;
	glsl.set_common_options(options);

	auto source = glsl.compile();

	std::ofstream out(m_glslOutputPath, std::ios::binary | std::ios::trunc);
	if (!out.good()) {
		throw std::runtime_error("Failed to open " + m_glslOutputPath.u8string());
	}

	out << source;

	return true;
}

} // namespace shadertool
