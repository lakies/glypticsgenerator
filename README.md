# Glyptics Portrait Generator

Master's Thesis for University of Tartu Software Engineering programme

Software for producing a 3D rendering of the user's face in glyptics art style.

## Development guide with Visual Studio and Windows:

* Install vcpkg for installing other dependencies
    * [https://github.com/microsoft/vcpkg#quick-start-windows](https://github.com/microsoft/vcpkg#quick-start-windows)
* Install Ogre
    * Install CG toolkit [https://developer.nvidia.com/cg-toolkit-download](https://developer.nvidia.com/cg-toolkit-download)
    * Clone ogre source v1.12.4
        * `git clone --recursive --branch v1.12.4 https://github.com/OGRECave/ogre.git`
        * Open CMake GUI and set CMAKE_BUILD_TYPE=Debug
        * Click Configure
        * Set CMAKE_BUILD_TYPE=Debug again and uncheck OGRE_BUILD_PLUGIN_DOT_SCENE
        * Click Generate
        * Open the project 
        * Select Plugin_CgProgramManager target and under Project > Properties > Linker > Input > Additional Dependencies change the /lib/ to /lib.x64/
        * Run ALL_BUILD and INSTALL targets
* Install PCL 1.9.1
    * [https://github.com/PointCloudLibrary/pcl/releases/tag/pcl-1.9.1](https://github.com/PointCloudLibrary/pcl/releases/tag/pcl-1.9.1)
* Install eos (NOTE: built this using CLion IDE, Visual Studio steps might be a bit different)
    * Install boost from vcpkg (`.\vcpkg.exe install boost --triplet x64-windows`)
    * Download opencv 3.4.2 from [https://opencv.org/releases/](https://opencv.org/releases/)
    * Download eos source [http://patrikhuber.github.io/eos/doc/index.html](http://patrikhuber.github.io/eos/doc/index.html)
        * add the following lines to CMakeLists.txt
        `add_compile_options(/bigobj)`
        `set(CMAKE_INSTALL_PREFIX C:/Users/adria/Desktop/OGRE/eos/out/build)`
        
            (replace install prefix path with your system's path)
        * add to cmake options in clion `-DCMAKE_TOOLCHAIN_FILE=C:/Users/adria/Desktop/vcpkg/scripts/buildsystems/vcpkg.cmake -DOpenCV_DIR=C:/Users/adria/Desktop/OGRE/opencv/build`
        * run install under Build > Install
        
* Install realsense2, dlib, glfw3, glm
    * `.\vcpkg.exe install dlib --triplet x64-windows`
    * `.\vcpkg.exe install glfw3 --triplet x64-windows`
    * `.\vcpkg.exe install glm --triplet x64-windows`

After installing all of the dependencies open the Visual Studio project.

Under Project > Properties change the following paths to the correct install locations
* C/C++ > General > Additional Include Directories
* Linker > General > Additional Library Directories
* Linker > Input > Additional Dependencies