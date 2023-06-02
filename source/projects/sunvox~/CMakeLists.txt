include(${CMAKE_CURRENT_SOURCE_DIR}/../../max-sdk-base/script/max-pretarget.cmake)

#############################################################
# MAX EXTERNAL
#############################################################


include_directories( 
	"${MAX_SDK_INCLUDES}"
	"${MAX_SDK_MSP_INCLUDES}"
	"${MAX_SDK_JIT_INCLUDES}"
)

execute_process(
	COMMAND install_name_tool -id "@loader_path/../Frameworks/libsunvox.dylib"
		   ${CMAKE_CURRENT_SOURCE_DIR}/libsunvox.dylib
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
	${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME}
	PUBLIC
	"-lsunvox"
	"-lstdc++"
)


target_link_directories(${PROJECT_NAME}
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
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
    COMMAND mkdir -p ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/sunvox~.mxo/Contents/Frameworks
    COMMAND cp -af ${CMAKE_CURRENT_SOURCE_DIR}/libsunvox.dylib
    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/sunvox~.mxo/Contents/Frameworks/libsunvox.dylib
)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../max-sdk-base/script/max-posttarget.cmake)
