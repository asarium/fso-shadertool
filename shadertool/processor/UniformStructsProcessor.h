#pragma once

#include "BaseShaderProcessor.h"

namespace shadertool {

class UniformStructsProcessor : public BaseShaderProcessor {
  public:
	UniformStructsProcessor();
	~UniformStructsProcessor() override;

	void addOptions(CLI::App& app) override;

	bool processShader(const std::filesystem::path& shaderPath, const std::vector<uint32_t>& spirvCode) override;

  private:
	std::filesystem::path m_outputPath;
};

} // namespace shadertool
