
add_library(termcolor INTERFACE)

target_include_directories(termcolor INTERFACE "include")

install(FILES "LICENSE"
	DESTINATION "share/fso-shadertool"
	RENAME "LICENSE-termcolor")

