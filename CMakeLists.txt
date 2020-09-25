CMAKE_MINIMUM_REQUIRED(VERSION 3.17)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 17)

project(shadertool)

if (MSVC)
	# A bunch of options for Windows builds. Mostly copied from FSO
	set(CMAKE_C_FLAGS "/MP /GS- /analyze- /Zc:wchar_t /errorReport:prompt /Zc:forScope /Gd /EHsc /nologo /Zm200")
	set(CMAKE_CXX_FLAGS "/MP /GS- /analyze- /Zc:wchar_t /errorReport:prompt /Zc:forScope /Gd /EHsc /nologo /Zm200")

	set(CMAKE_EXE_LINKER_FLAGS "/MANIFEST /DYNAMICBASE:NO /SAFESEH:NO /ERRORREPORT:PROMPT /NOLOGO")
	set(CMAKE_STATIC_LINKER_FLAGS "")

	set(CMAKE_C_FLAGS_RELEASE "/GL /Gy- /Ox /Ot /Ob2 /fp:precise /GF /Oy /Oi /Zi /W3 /MT")
	set(CMAKE_CXX_FLAGS_RELEASE "/GL /Gy- /Ox /Ot /Ob2 /fp:precise /GF /Oy /Oi /Zi /W3 /MT")
	# /DEBUG:FULL so that we get PDBs which is needed for debugging crashdumps
	set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/OPT:REF /LTCG /INCREMENTAL:NO")
	set(CMAKE_STATIC_LINKER_FLAGS_RELEASE "/LTCG")

	set(CMAKE_C_FLAGS_DEBUG "/W4 /Gy /Zi /Od /RTC1 /Gd /Oy- /MTd")
	set(CMAKE_CXX_FLAGS_DEBUG "/W4 /Gy /Zi /Od /RTC1 /Gd /Oy- /MTd")
	set(CMAKE_EXE_LINKER_FLAGS_DEBUG "/DEBUG /INCREMENTAL:YES /MAPINFO:EXPORTS")
endif()

add_subdirectory(3rd-party)

add_subdirectory(shadertool)
