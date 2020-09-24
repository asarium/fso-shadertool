
#include "BaseShaderProcessor.h"

namespace shadertool {

BaseShaderProcessor::BaseShaderProcessor(std::string name, std::string description)
	: m_name(std::move(name)), m_description(std::move(description))
{
}

BaseShaderProcessor::~BaseShaderProcessor() = default;

void BaseShaderProcessor::addOptions(CLI::App& app)
{
	app.add_flag(
		"--" + m_name,
		[this](int64_t count) { m_enabled = count > 0; },
		m_description);
}
bool BaseShaderProcessor::isEnabled() const
{
	return m_enabled;
}
const std::string& BaseShaderProcessor::getName() const
{
	return m_name;
}
const std::string& BaseShaderProcessor::getDescription() const
{
	return m_description;
}

} // namespace shadertool
