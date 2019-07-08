cmake_minimum_required(VERSION 2.8.12)
project(bop_renderer)

# Enable support for C++11.
set(CPP_STANDARD -std=c++11)
add_definitions(${CPP_STANDARD})
#add_definitions("-O3")

# Python.
set(PYTHON_LIBRARIES "/usr/lib/x86_64-linux-gnu/libpython2.7.so")
set(PYTHON_INCLUDE_DIRS "/usr/include/python2.7")

# Pybind11.
set(PYBIND11_CPP_STANDARD ${CPP_STANDARD})
set(PYBIND11_PYTHON_VERSION 2.7.12)
add_subdirectory(3rd/pybind11)

# LLVM.
link_directories("/opt/llvm/lib")

# PNG.
include(FindPNG)
include_directories(${PNG_INCLUDE_DIR})

# OSMesa.
set(OSMESA_LIBRARIES "/opt/osmesa/lib/libOSMesa32.so")
include_directories("/opt/osmesa/include")

# YAML-CPP.
find_package(yaml-cpp REQUIRED)
#find_package(yaml-cpp REQUIRED PATHS "/path/to/yaml-cpp/build")
include_directories(${YAML_CPP_INCLUDE_DIR})

# Others.
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(3rd)
include_directories(3rd/glm)

# Header files.
set(HEADERFILES
        src/Renderer.h
        src/PythonWrapper.h
        src/AbstractLoader.h
        src/PlyLoader.h
        src/GPUBuffer.h
        src/Pose.h
        src/UtilIO.h
        src/Geometry.h
        src/glutils/FrameBufferObject.h
        src/glutils/GLSLProgram.h
        src/glutils/gl_core_3_3.h
        src/context/Context.h
        src/Texture.h
        src/Shaders.h
        )

# Source files.
set(SOURCEFILES
        src/Renderer.cpp
        src/AbstractLoader.cpp
        src/PlyLoader.cpp
        src/GPUBuffer.cpp
        src/Pose.cpp
        src/UtilIO.cpp
        src/glutils/FrameBufferObject.cpp
        src/glutils/GLSLProgram.cpp
        src/glutils/gl_core_3_3.cpp
        src/context/OSMContext.cpp
        3rd/rply/rply.c
        3rd/lodepng/lodepng.cpp
        )

# Executable.
add_executable(renderer src/main.cpp ${HEADERFILES} ${SOURCEFILES})
target_link_libraries(renderer
        ${OSMESA_LIBRARIES} ${PNG_LIBRARY} ${YAML_CPP_LIBRARIES})

# Python module.
pybind11_add_module(bop_renderer MODULE
        src/PythonWrapper.cpp ${HEADERFILES} ${SOURCEFILES})
target_link_libraries(bop_renderer PRIVATE
        ${OSMESA_LIBRARIES} ${PNG_LIBRARY} ${YAML_CPP_LIBRARIES})
