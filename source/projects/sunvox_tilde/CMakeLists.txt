include(${CMAKE_CURRENT_SOURCE_DIR}/../../max-sdk-base/script/max-pretarget.cmake)

#############################################################
# MAX EXTERNAL
#############################################################

# Build SunVox static library
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../sunvox_lib ${CMAKE_BINARY_DIR}/sunvox_lib)

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
	${CMAKE_CURRENT_SOURCE_DIR}/../sunvox_lib/sunvox_lib/headers
)

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

add_custom_command(TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND mkdir -p ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/sunvox~.mxo/Contents/Resources
    COMMAND cp -f ${CMAKE_CURRENT_SOURCE_DIR}/resources/* ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/sunvox~.mxo/Contents/Resources
)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../max-sdk-base/script/max-posttarget.cmake)
