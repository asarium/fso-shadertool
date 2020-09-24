
#include "processor/BaseShaderProcessor.h"
#include "processor/GlslOutputProcessor.h"
#include "processor/UniformStructsProcessor.h"

namespace {

std::vector<uint32_t> load_spirv_file(const std::filesystem::path& path)
{
	std::ifstream in(path, std::ios::binary);

	if (!in.good()) {
		throw std::runtime_error("Failed to open file.");
	}

	auto fsize = in.tellg();
	in.seekg(0, std::ios::end);
	fsize = in.tellg() - fsize;
	in.seekg(0, std::ios::beg);

	std::vector<uint32_t> output;
	output.resize(fsize / 4);
	in.read(reinterpret_cast<char*>(output.data()), fsize);

	return output;
}

std::vector<std::unique_ptr<shadertool::BaseShaderProcessor>> buildProcessorList()
{
	std::vector<std::unique_ptr<shadertool::BaseShaderProcessor>> list;

	list.emplace_back(new shadertool::GlslOutputProcessor());
	list.emplace_back(new shadertool::UniformStructsProcessor());

	return list;
}

} // namespace

int main(int argc, char** argv)
{
	auto processorList = buildProcessorList();

	CLI::App app{"A tool for post processing FreeSpace Open SPIR-V shaders"};

	std::string shader_path;
	app.add_option("shader", shader_path, "The shader file to process")->required()->check(CLI::ExistingFile);

	for (const auto& processor : processorList) {
		processor->addOptions(app);
	}

	CLI11_PARSE(app, argc, argv)

	std::vector<uint32_t> shader_content;
	auto shaderFsPath = std::filesystem::u8path(shader_path);
	try {
		shader_content = load_spirv_file(shaderFsPath);
	} catch (const std::exception& e) {
		std::cout << "Failed to read " << termcolor::blue << "\"" << shader_path << "\"" << termcolor::reset << ":"
				  << termcolor::red << e.what() << termcolor::reset << "\n";
		return EXIT_FAILURE;
	}

	bool failure = false;
	for (const auto& processor : processorList) {
		if (processor->isEnabled()) {
			try {
				processor->processShader(shaderFsPath, shader_content);
			} catch (const std::exception& e) {
				std::cout << "Failed processing " << termcolor::blue << "\"" << shader_path << "\"" << termcolor::reset
						  << " with processor " << termcolor::bold << processor->getName() << termcolor::reset << ": "
						  << termcolor::red << e.what() << termcolor::reset << "\n";
				failure = true;
				continue;
			}
		}
	}

	if (failure) {
		return EXIT_FAILURE;
	} else {
		return EXIT_SUCCESS;
	}
}
