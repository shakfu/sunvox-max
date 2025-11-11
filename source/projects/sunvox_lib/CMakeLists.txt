cmake_minimum_required(VERSION 3.19)

project(sunvox_static)

# Platform detection
if(APPLE)
    set(OS_MACOS ON)
    add_definitions(-DOS_MACOS -DOS_APPLE -DOS_UNIX)
elseif(UNIX)
    set(OS_LINUX ON)
    add_definitions(-DOS_LINUX -DOS_UNIX)
elseif(WIN32)
    set(OS_WIN ON)
    add_definitions(-DOS_WIN)
endif()

# SunVox library configuration - matching original Makefile
add_definitions(
    -DMIN_SAMPLE_RATE=44100
    -DSUNVOX_LIB
    -DNOVIDEO
    -DNOVCAP
    -DNOLIST
    -DNOFILEUTILS
    -DNOIMAGEFORMATS
    -DNOMIDI
    -DPS_STYPE_FLOAT32
    -DCOLOR32BITS
    -DMAKE_WITHOUT_GUI=1
    -DMAKE_WITHOUT_MAIN=1
    -DNOGUI
    -DNOMAIN
)

if(APPLE)
    add_definitions(-DOPENGL)
endif()

# Disable encoder/decoder features we don't need
add_definitions(
    -DMAKE_WITHOUT_FLAC_ENCODER=1
    -DMAKE_WITHOUT_LIBVORBIS_ENCODER=1
    -DMAKE_WITHOUT_LIBVORBIS_DECODER=1
    -DNOFLAC
    -DNOFLACENC
    -DNOOGG
    -DNOOGGENC
    -DNOVORBISENC
)

# Include directories
set(SUNVOX_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SUNDOG_DIR ${SUNVOX_LIB_DIR}/lib_sundog)
set(SUNVOX_DIR ${SUNVOX_LIB_DIR}/lib_sunvox)
set(DSP_DIR ${SUNVOX_LIB_DIR}/lib_dsp)
set(VORBIS_DIR ${SUNVOX_LIB_DIR}/lib_vorbis)
set(MP3_DIR ${SUNVOX_LIB_DIR}/lib_mp3)
set(FLAC_DIR ${SUNVOX_LIB_DIR}/lib_flac)

include_directories(
    ${SUNVOX_LIB_DIR}/sunvox_lib/headers
    ${SUNDOG_DIR}
    ${SUNVOX_DIR}
    ${DSP_DIR}
    ${VORBIS_DIR}
    ${VORBIS_DIR}/tremor
    ${MP3_DIR}
    ${FLAC_DIR}
)

# Collect all source files
file(GLOB_RECURSE SUNDOG_SOURCES
    "${SUNDOG_DIR}/*.cpp"
)

# Add Apple-specific Objective-C++ files on macOS
if(APPLE)
    file(GLOB_RECURSE APPLE_SOURCES
        "${SUNDOG_DIR}/*.mm"
    )
    list(APPEND SUNDOG_SOURCES ${APPLE_SOURCES})
    # Mark .mm files as Objective-C++
    set_source_files_properties(${APPLE_SOURCES} PROPERTIES LANGUAGE CXX)
endif()

file(GLOB_RECURSE SUNVOX_SOURCES
    "${SUNVOX_DIR}/*.cpp"
)

file(GLOB_RECURSE DSP_SOURCES
    "${DSP_DIR}/*.cpp"
)

file(GLOB_RECURSE VORBIS_SOURCES
    "${VORBIS_DIR}/tremor/*.c"
)

file(GLOB_RECURSE MP3_SOURCES
    "${MP3_DIR}/*.c"
    "${MP3_DIR}/*.cpp"
)

# FLAC not needed for basic SunVox usage (encoder disabled in original makefiles)
# file(GLOB_RECURSE FLAC_SOURCES
#     "${FLAC_DIR}/*.c"
#     "${FLAC_DIR}/*.cpp"
# )

# Main library file
set(MAIN_SOURCE ${SUNVOX_LIB_DIR}/sunvox_lib/main/sunvox_lib.cpp)

# Combine all sources
set(ALL_SOURCES
    ${MAIN_SOURCE}
    ${SUNDOG_SOURCES}
    ${SUNVOX_SOURCES}
    ${DSP_SOURCES}
    ${VORBIS_SOURCES}
    ${MP3_SOURCES}
    # ${FLAC_SOURCES}
)

# Filter out unwanted files (if any)
list(FILTER ALL_SOURCES EXCLUDE REGEX ".*test.*")
list(FILTER ALL_SOURCES EXCLUDE REGEX ".*example.*")
list(FILTER ALL_SOURCES EXCLUDE REGEX ".*android.*")
list(FILTER ALL_SOURCES EXCLUDE REGEX ".*ios.*")
list(FILTER ALL_SOURCES EXCLUDE REGEX ".*win.*")
list(FILTER ALL_SOURCES EXCLUDE REGEX ".*wince.*")
list(FILTER ALL_SOURCES EXCLUDE REGEX ".*linux.*")
list(FILTER ALL_SOURCES EXCLUDE REGEX ".*/main/macos/.*")
list(FILTER ALL_SOURCES EXCLUDE REGEX ".*/main/ios/.*")

# Create static library
add_library(sunvox_static STATIC ${ALL_SOURCES})

# Set target properties
set_target_properties(sunvox_static PROPERTIES
    OUTPUT_NAME "sunvox"
    POSITION_INDEPENDENT_CODE ON
)

# Compiler flags
if(APPLE)
    target_compile_options(sunvox_static PRIVATE
        -fno-exceptions
        -fno-rtti
        -Wno-deprecated-declarations
        -Wno-unused-variable
        -Wno-unused-function
    )

    target_compile_definitions(sunvox_static PRIVATE
        MAKE_WITHOUT_GUI=1
        MAKE_WITHOUT_MAIN=1
    )

    # Link required frameworks for Apple-specific code
    target_link_libraries(sunvox_static PUBLIC
        "-framework Foundation"
        "-framework Cocoa"
        "-framework AudioUnit"
        "-framework AudioToolbox"
        "-framework CoreAudio"
        "-framework CoreFoundation"
        "-framework OpenGL"
    )
endif()

# Installation
install(TARGETS sunvox_static
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)
