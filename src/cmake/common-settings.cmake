#
# Common FLAGS for all compilers
#
set(CMAKE_CXX_STANDARD 20)

set(COMMON_GCC_FLAGS "-Wall -W -Wextra -Wfatal-errors")
if (NOT WIN32)
# These flags are incompatible with non-x86 architectures (e.g ARM)
# TODO remove
	set(COMMON_GCC_FLAGS "${COMMON_GCC_FLAGS} -pipe -msse -msse2 -Wunused-but-set-variable -Wunused-but-set-parameter")
	set(COMMON_GCC_FLAGS "${COMMON_GCC_FLAGS} -Werror=return-type")
endif()
set(COMMON_MSVC_FLAGS "/W3 /MP4")
set(COMMON_MSVC_FLAGS "${COMMON_MSVC_FLAGS} /we4715 /we4716") #adding no return or no return for all code paths as errors
set(ADDITIONAL_C_FLAGS " -Wconversion -Wmissing-prototypes -Wstrict-prototypes")
set(ADDITIONAL_C_FLAGS "${ADDITIONAL_C_FLAGS} -Wmissing-noreturn -Wpacked -Wredundant-decls -Wbad-function-cast -W -Wcast-align -Wcast-qual -Wsign-compare -fno-exceptions -Wdeclaration-after-statement")
set(CMAKE_COLOR_DIAGNOSTICS on)


Set(ANTARES_VERSION_TARGET "unknown")
Set(ANTARES_INSTALLER_EXTENSION "")


macro(EMBED_MANIFEST manifestfile target)
	if(MSVC AND NOT MSVC9)
		message(STATUS "{antares}    :: adding rule for manifest ${manifestfile}")
		set(ANTARES_UI_BIN_TARGET "Debug\\")
		if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
			set(ANTARES_UI_BIN_TARGET "Release\\")
		endif()

		add_custom_command(TARGET ${target} POST_BUILD COMMAND
			"${PROJECT_SOURCE_DIR}/../bin/mt.exe"
			"-nologo" "-manifest"
			"\"${CMAKE_CURRENT_SOURCE_DIR}/${manifestfile}\""
			"-outputresource:${CMAKE_CURRENT_SOURCE_DIR}\\${ANTARES_UI_BIN_TARGET}\\${target};1"
			COMMENT "Embedding the manifest into the executable '${CMAKE_CURRENT_SOURCE_DIR}\\${ANTARES_UI_BIN_TARGET}\\${target}'")
	endif()
endmacro()

#
# Command line options for G++ (Debug)
#
# Ex: cmake . -DCMAKE_BUILD_TYPE=release
#
if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")

	#
	# Build Configuration: Release
	#
	set(ANTARES_VERSION_TARGET "release")

	if(NOT WIN32)
		set(CMAKE_CXX_FLAGS_RELEASE "${COMMON_GCC_FLAGS} -O3 -funroll-loops -finline-functions")
		set(CMAKE_C_FLAGS_RELEASE   "${COMMON_GCC_FLAGS} -O3 -funroll-loops -finline-functions ${ADDITIONAL_C_FLAGS}")
	endif(NOT WIN32)
	add_definitions("-DNDEBUG") # Remove asserts

	if (NOT MINGW AND NOT MSVC)
		set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -ansi")
	endif()
	if (NOT MSVC)
		set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -std=c99")
	endif()
else()

	#
	# Build Configuration: Debug
	#
	set(ANTARES_VERSION_TARGET "debug")
	set(ANTARES_INSTALLER_EXTENSION "-debug")

	if(NOT WIN32)

		set(CMAKE_CXX_FLAGS_DEBUG "${COMMON_GCC_FLAGS} -g3 -ggdb3 -feliminate-unused-debug-symbols")

		set(CMAKE_C_FLAGS_DEBUG   "${COMMON_GCC_FLAGS} -g3 -ggdb3 -feliminate-unused-debug-symbols ${ADDITIONAL_C_FLAGS}")

	endif()
	add_definitions("-DANTDEBUG") # More debug !

	if (NOT MINGW AND NOT MSVC)
		set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -ansi")
	endif()
	if (NOT MSVC)
		set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -std=c99")
	endif()

endif()



#TODO : check these definitions
# UNICODE
if(WIN32 OR WIN64)
	add_definitions("-DUNICODE")
	add_definitions("-D_UNICODE")
	add_definitions("-D__UNICODE")
endif()


if(MSVC)
	set(CMAKE_C_FLAGS   "${COMMON_MSVC_FLAGS}")
	set(CMAKE_CXX_FLAGS "${COMMON_MSVC_FLAGS}")

	# If other libraries have been linked statically with C runtime, we need to keep consistent with it
	if("${VCPKG_TARGET_TRIPLET}" STREQUAL "x64-windows-static" OR "${VCPKG_TARGET_TRIPLET}" STREQUAL "x86-windows-static")
		set(CRT_LIBRARY_DEBUG "/MTd")
		set(CRT_LIBRARY_RELEASE "/MT")
    else()
		set(CRT_LIBRARY_DEBUG "/MDd")
		set(CRT_LIBRARY_RELEASE "/MD")
	endif()

	set(CMAKE_C_FLAGS_DEBUG   "${CRT_LIBRARY_DEBUG} /Zi /GR /Ot /Od /EHsc /RTC1")
	set(CMAKE_CXX_FLAGS_DEBUG "${CRT_LIBRARY_DEBUG} /Zi /GR /Ot /Od /EHsc /RTC1 /fp:except")

	# RELEASE
	set(CMAKE_EXE_LINKER_FLAGS_RELEASE)
	set(MSVC_RELEASE_FLAGS)
	# O2x: optimization
	set(MSVC_RELEASE_FLAGS "${MSVC_RELEASE_FLAGS} /O2")
	# Prefer speed instead of size
	set(MSVC_RELEASE_FLAGS "${MSVC_RELEASE_FLAGS} /Ot")
	# Omit frame pointer
	set(MSVC_RELEASE_FLAGS "${MSVC_RELEASE_FLAGS} /Oy")
	# Any suitable inlining
	set(MSVC_RELEASE_FLAGS "${MSVC_RELEASE_FLAGS} /Ob2")
	# Fiber-safe optimizations
	set(MSVC_RELEASE_FLAGS "${MSVC_RELEASE_FLAGS} /GT")
	# whole program / requires "Link time code generation"
	#set(MSVC_RELEASE_FLAGS "${MSVC_RELEASE_FLAGS} /GL")
	# No buffer security check
	set(MSVC_RELEASE_FLAGS "${MSVC_RELEASE_FLAGS} /GS-")
	# Intrinsic functions
	set(MSVC_RELEASE_FLAGS "${MSVC_RELEASE_FLAGS} /Oi")
	# C runtime library
	set(MSVC_RELEASE_FLAGS "${MSVC_RELEASE_FLAGS} ${CRT_LIBRARY_RELEASE}")


	# linker: Link time code generation
	#set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")
	if(NOT "${ANTARES_x86_64}" STREQUAL "")
		#message(STATUS "{antares} using 64bits architecture")
		set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /MACHINE:X64")
		set(CMAKE_EXE_LINKER_FLAGS_DEBUG   "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /MACHINE:X64")
	else()
		# SSE2
		set(MSVC_RELEASE_FLAGS "${MSVC_RELEASE_FLAGS} /arch:SSE2")
	endif()
	# Release
	set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /RELEASE")
	# Remove symbols
	set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /OPT:REF")

	set(CMAKE_CXX_FLAGS_RELEASE "${MSVC_RELEASE_FLAGS} /EHsc")
	set(CMAKE_C_FLAGS_RELEASE "${MSVC_RELEASE_FLAGS} /EHsc")


	#SET(CMAKE_EXE_LINKER_FLAGS_DEBUG   "/debug /VERSION:${ANTARES_VERSION_HI}.${ANTARES_VERSION_LO}")
	#SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "/VERSION:${ANTARES_VERSION_HI}.${ANTARES_VERSION_LO}")
endif()

if (NOT MSVC)
	# segv for gcc
	if (CMAKE_COMPILER_IS_GNUCC)
		execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)

		if (GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7)
			#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fvisibility-ms-compat -fvisibility-inlines-hidden")
			set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fvisibility-inlines-hidden")
			set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG}   -fvisibility-ms-compat -fvisibility-inlines-hidden")
		endif()
	endif()
endif()


#TODO : check why theses definition  are needed
if(WIN32)
	add_definitions("-DANT_WINDOWS")
	add_definitions("/D_CRT_SECURE_NO_WARNINGS")
endif()

# Thread safety
add_definitions("-D_REENTRANT -DXUSE_MTSAFE_API")

# ICC Optimizations
if (ICC)
	message(STATUS "{antares} Adding optimization flags for ICC")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Os /QaxSSE3 /Qunroll-aggressive /Qinline-calloc /Qms:0")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /Os /QaxSSE3 /Qunroll-aggressive /Qinline-calloc /Qms:0")
endif()


#TODO : check these macro
macro(import_std_libs  TARGET)
	if(MSVC10)
		# WinSock
		target_link_libraries(${TARGET} PRIVATE wsock32.lib)
	elseif(WIN32)
		target_link_libraries(${TARGET}  PRIVATE wsock32.lib legacy_stdio_definitions.lib)
	endif()
endmacro()


macro(executable_strip TARGET)
	if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
		if(NOT MSVC)
			if(WIN32)
				add_custom_command(TARGET ${TARGET} POST_BUILD COMMAND ${CMAKE_STRIP} $<TARGET_FILE:${TARGET}>)
			else()
				add_custom_command(TARGET ${TARGET} POST_BUILD COMMAND ${CMAKE_STRIP} --strip-all $<TARGET_FILE:${TARGET}>)
			endif()
		endif()
	endif()
endmacro()
