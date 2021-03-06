cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(GPG-Cpp)

add_compile_options(/bigobj)

# Save the command line compile commands in the build output
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(OGRE_DIR C:/Users/adria/Desktop/OGRE/CMake)
set(OGRE_HOME C:/Users/adria/Desktop/OGRE)
#set(OGRE_DIR C:/Users/adria/Desktop/OGRE/CMake)
find_package(OGRE REQUIRED)
include_directories(${OGRE_INCLUDE_DIRS})
link_directories(${OGRE_INCLUDE_DIRS})
add_definitions(${OGRE_DEFINITIONS})

find_package(realsense2 REQUIRED)
include_directories(${realsense_INCLUDE_DIR})

find_package(dlib CONFIG REQUIRED)
include_directories(${dlib_INCLUDE_DIR})

# Replace this with your eos path
include_directories(C:/Users/adria/Desktop/OGRE/eos/out/build/include)
include_directories(C:/Users/adria/Desktop/OGRE/eos/out/build/3rdparty/cereal/include)
include_directories(C:/Users/adria/Desktop/OGRE/eos/out/build/3rdparty/eigen/Eigen)
include_directories(C:/Users/adria/Desktop/OGRE/eos/out/build/3rdparty/eigen3-nnls/src)
include_directories(C:/Users/adria/Desktop/OGRE/eos/out/build/3rdparty/glm)
include_directories(C:/Users/adria/Desktop/OGRE/eos/out/build/3rdparty/nanoflann/include)
include_directories(C:/Users/adria/Desktop/OGRE/eos/out/build/3rdparty/toml11)

set(OpenCV_DIR C:/Users/adria/Desktop/OGRE/opencv/build)
FIND_PACKAGE( OpenCV REQUIRED )
INCLUDE_DIRECTORIES( ${OpenCV_INCLUDE_DIRS} )


include_directories()

# Make project require C++11
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
endif()

# Simple non robust way to find the librealsense library
if(WIN32)
    if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
        set(LIBRARY_DIR "C:\\Program Files (x86)\\Intel RealSense SDK 2.0\\lib\\x64")          # TODO: Update this variable to correct path - folder where realsense2.lib is found
        set(DLL_DIR "C:\\Program Files (x86)\\Intel RealSense SDK 2.0\\bin\\x64")              # TODO: Update this variable to correct path - folder where realsense2.dll is found
    else()
        set(LIBRARY_DIR "C:\\Program Files (x86)\\Intel RealSense SDK 2.0\\lib\\x86")          # TODO: Update this variable to correct path - folder where realsense2.lib is found
        set(DLL_DIR "C:\\Program Files (x86)\\Intel RealSense SDK 2.0\\bin\\x86")              # TODO: Update this variable to correct path - folder where realsense2.dll is found
    endif()
    set(PROJECT_BINARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/\$\(Configuration\)")                # TODO: Update this variable to correct path - folder to which your project will compile
    set(ADDITIONAL_INCLUDE_DIRS "C:\\Program Files (x86)\\Intel RealSense SDK 2.0\\include")   # TODO: Update this variable to correct path - folder where librealsense2 folder is found
endif()


find_library(REALSENSE2_FOUND realsense2 HINTS ${LIBRARY_DIR} REQUIRED)
if(NOT REALSENSE2_FOUND)
    SET(REALSENSE2_FOUND "realsense2")
    message(WARN "Failed to find_library(realsense2)")
endif()


find_package(OpenGL REQUIRED)

# GLM
find_package(GLM REQUIRED)

# GLFW
find_package(GLFW3 REQUIRED)

set(LIBS glfw3 opengl32)

file(GLOB SOURCES
        src/*.h
        src/*.cpp
        )
add_executable (gpg_cpp ${SOURCES})

FIND_PACKAGE( Boost 1.40 COMPONENTS program_options thread REQUIRED )
message("boost ${Boost_LIBRARIES}")

target_link_libraries (gpg_cpp
        ${REALSENSE2_FOUND}
        ${OGRE_LIBRARIES}
        ${realsense2_LIBRARY}
        dlib::dlib
        glfw
        ${OpenCV_LIBS}
        ${Boost_LIBRARIES}
        )
set_target_properties (gpg_cpp PROPERTIES FOLDER Examples)

# Post Build script to copy realsense2.dll
if(WIN32)
message(STATUS "Adding Post build script to copy realsense2.dll to project's binary folder")
message(STATUS "Will try to copy from ${DLL_DIR} to ${PROJECT_BINARY_OUTPUT_PATH}")
add_custom_command(TARGET gpg_cpp POST_BUILD        # Adds a post-build event to rs-getting-started
    COMMAND ${CMAKE_COMMAND} -E copy_if_different              # which executes "cmake - E copy_if_different..."
        "${DLL_DIR}\\realsense2.dll"                            # <--this is in-file
        ${PROJECT_BINARY_OUTPUT_PATH})                 # <--this is out-file path
endif()

install(
    TARGETS

    gpg_cpp

    RUNTIME DESTINATION
    ${CMAKE_INSTALL_PREFIX}/bin
)