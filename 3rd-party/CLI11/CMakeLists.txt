
add_library(CLI11 INTERFACE)

target_include_directories(CLI11 INTERFACE "include")

install(FILES "LICENSE"
	DESTINATION "share/fso-shadertool"
	RENAME "LICENSE-CLI11")

