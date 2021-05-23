# Glyptics Portrait Generator

Software for producing a 3D rendering of the user's face in glyptics art style.

Download the release version [here](https://github.com/lakies/glypticsgenerator/releases/tag/v1.0).

## Overview

The application uses the Ogre rendering engine to render an [engraved gem](https://en.wikipedia.org/wiki/Engraved_gem).


The gem is rendered with two different materials: marble and multi-layered agate. \
Both materials are created procedurally based on each pixel's coordinates in the 3D scene.

### Marble

![Marble](https://i.imgur.com/GX3sTB5.png "Marble")

Marble texture is created with a method created by [Lagae, A. et al.](https://hal.inria.fr/hal-00920177/document) in which vertical dark lines across the surface are distorted using layered [Simplex noise](https://en.wikipedia.org/wiki/Simplex_noise). \
The lines are achieved by passing the sine of the x coordinate to a colormap function, which produced the following result on the left:

![Lines](https://i.imgur.com/Q3HZ1u2.png "Lines")
![Noise](https://i.imgur.com/rmDnUT8.png "Noise")

Then a turbulence factor is added to the x coordinate, which is created using Simplex noise. The noise can be seen above on the right.\
Furthermore, the square of the y coordinate is multiplied with the sine function input which broke up the lines a little bit. 

All put together, the color calculation for the marble texture looked like this:\
`vec3 color = colormap(sin(uv.y * uv.y * uv.x * frequency + turbulence(uv)));`

### Agate

Many materials used in real life glyptic art composed of multiple layers, each of which is colored differently. In GPG this multi-layer effect can be seen in the agate material.

![Agate](https://i.imgur.com/hSxMB2r.png "Agate")

Three different layers are used to create this texture.

First, the background is a dark red color, sampled from a picture of a real-life agate engraved gem. The color is then modulated using Simplex noise to give it a more natural look.

The head is colored white. To sell the multi-layered look even further, a fade is given to the edges where the white layer is thinner.

The hair is colored darker than the rest of the head by calculating each surface pixel's distance to three pre-defined points under the surface. Then if the distance to the closest point is in a specific range then the pixel is colored darker.

### Subsurface Scattering

In the case of marble and agate (and other translucent materials), when light hits a point on the surface, then a small amount of it penetrates the surface and exits from some other point (see illustration below). This gives a distinct glowy effect to many real materials and without it, they would look less realistic. To account for this, GPG has a method for approximating this effect, called *subsurface scattering*. The method is originally created by [Jimenez, J. et al.](http://www.iryoku.com/separable-sss/).

![Subsurface scattering](https://imgur.com/Vq6bJ1p.png "Subsurface scattering")

The subsurface scattering effect is implemented as a post-processing filter. After the image of the engraved gem is rendered, the filter first blurs the image vertically and then horizontally. Using two passes instead of one greatly speeds up the image processing.\
The result can be seen below, where on the left are the two materials without the effect and on the right is the effect applied.

![SS](https://i.imgur.com/Fat3u9o.png "SS")



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