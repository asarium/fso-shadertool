
set(SOURCES
	main.cpp

	processor/BaseShaderProcessor.cpp
	processor/BaseShaderProcessor.h
	processor/GlslOutputProcessor.cpp
	processor/GlslOutputProcessor.h
	processor/UniformStructsProcessor.cpp
	processor/UniformStructsProcessor.h
)

add_executable(shadertool ${SOURCES})

target_link_libraries(shadertool
	PRIVATE
		spirv-cross-core
		spirv-cross-glsl
		CLI11
		termcolor
)

target_precompile_headers(shadertool PRIVATE "prefix_header.h")

# Enable extra warnings
target_compile_options(shadertool PRIVATE
	$<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
		-Wall>
	$<$<CXX_COMPILER_ID:MSVC>:
		/W4>
)

target_compile_definitions(shadertool PRIVATE NOMINMAX)

install(TARGETS shadertool DESTINATION bin)
