#pragma once

namespace shadertool {

class BaseShaderProcessor {
  public:
	BaseShaderProcessor(std::string name, std::string description);
	virtual ~BaseShaderProcessor();

	virtual void addOptions(CLI::App& app);

	virtual bool processShader(const std::filesystem::path& shaderPath, const std::vector<uint32_t>& spirvCode) = 0;

	[[nodiscard]] bool isEnabled() const;
	[[nodiscard]] const std::string& getName() const;
	[[nodiscard]] const std::string& getDescription() const;

  protected:
	std::string m_name;
	std::string m_description;
	bool m_enabled = false;
};

} // namespace shadertool
