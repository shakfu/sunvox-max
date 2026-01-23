include(${CMAKE_CURRENT_SOURCE_DIR}/../../max-sdk-base/script/max-pretarget.cmake)

#############################################################
# MAX EXTERNAL
#############################################################

set(SUNVOX_LIB ${CMAKE_CURRENT_SOURCE_DIR}/../sunvox_lib)

# Build SunVox static library
add_subdirectory(${SUNVOX_LIB} ${CMAKE_BINARY_DIR}/sunvox_lib)

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

target_link_libraries(${PROJECT_NAME}
	PUBLIC
	sunvox_static
)

if(APPLE)
	target_link_libraries(${PROJECT_NAME}
		PUBLIC
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
	target_link_libraries(${PROJECT_NAME}
		PUBLIC
		winmm
		dsound
		ole32
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
