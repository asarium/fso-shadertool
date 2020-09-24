
add_subdirectory(SPIRV-Cross)

install(FILES "SPIRV-Cross/LICENSE"
	DESTINATION "share/fso-shadertool"
	RENAME "LICENSE-SPIRV-Cross")

add_subdirectory(CLI11)

add_subdirectory(termcolor)
