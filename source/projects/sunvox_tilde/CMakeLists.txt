include(${CMAKE_CURRENT_SOURCE_DIR}/../../max-sdk-base/script/max-pretarget.cmake)

#############################################################
# MAX EXTERNAL
#############################################################

set(SUNVOX_LIB ${CMAKE_CURRENT_SOURCE_DIR}/../sunvox_lib)

include_directories(
	"${MAX_SDK_INCLUDES}"
	"${MAX_SDK_MSP_INCLUDES}"
	"${MAX_SDK_JIT_INCLUDES}"
)

file(GLOB PROJECT_SRC
     "*.h"
	 "*.c"
     "*.cpp"
)
add_library(
	${PROJECT_NAME}
	MODULE
	${PROJECT_SRC}
)

target_include_directories(${PROJECT_NAME}
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${SUNVOX_LIB}/sunvox_lib/headers
)

if(APPLE)
	# macOS: Build and link static library from source
	add_subdirectory(${SUNVOX_LIB} ${CMAKE_BINARY_DIR}/sunvox_lib)
	target_compile_definitions(${PROJECT_NAME} PRIVATE SUNVOX_STATIC_LIB)
	target_link_libraries(${PROJECT_NAME}
		PUBLIC
		sunvox_static
		"-framework Foundation"
		"-framework Cocoa"
		"-framework AudioUnit"
		"-framework AudioToolbox"
		"-framework CoreAudio"
		"-framework CoreFoundation"
		"-framework OpenGL"
		"-lstdc++"
		"-lobjc"
	)
	target_compile_options(${PROJECT_NAME}
		PUBLIC
		"-m64"
	)
	target_link_options(${PROJECT_NAME}
		PUBLIC
		"-m64"
	)
elseif(WIN32)
	# Windows: Use prebuilt DLL with runtime loading
	target_compile_definitions(${PROJECT_NAME} PRIVATE SUNVOX_MAIN)
	target_link_libraries(${PROJECT_NAME}
		PUBLIC
		winmm
	)
	# Copy sunvox.dll to support folder (Max package convention for shared libraries)
	set(SUPPORT_DIR "${CMAKE_SOURCE_DIR}/support")
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E make_directory "${SUPPORT_DIR}"
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"${CMAKE_SOURCE_DIR}/source/projects/sunvox_lib/sunvox_lib/windows/lib_x86_64/sunvox.dll"
			"${SUPPORT_DIR}/sunvox.dll"
	)
endif()

# Copy resources if the directory exists (macOS bundle structure only)
if(APPLE AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/resources)
    add_custom_command(TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/sunvox~.mxo/Contents/Resources
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/resources ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/sunvox~.mxo/Contents/Resources
    )
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/../../max-sdk-base/script/max-posttarget.cmake)
