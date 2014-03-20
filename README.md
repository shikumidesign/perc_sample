perc_sample
===========

Samples for Intel Perceptual Computing SDK

Requirements
------------
* Windows
* Visual Studio
* CMake (http://www.cmake.org/)
* OpenCV (http://opencv.org/)
* Intel Perceptual Computing SDK 2013 (http://software.intel.com/en-us/vcsource/tools/perceptual-computing-sdk)
* Creative Senze3D

Build Instructions
------------------
1. Download and install the Intel Perceptual Computing SDK.

2. Download the OpenCV for Windows(opencv-2.4.8.exe). Unpack the self-extract archive, and put them into perc_cample/opencv directory.

3. Configure by CMake:  
Start the CMake. Specify the source code directory and the build directory. Press the Configure button, and choose compiler you use. Press the Generate button. Then, Visual Studio solution will be generated.

4. Open the solution in the Visual Studio IDE.

About Sample
------------
### Wave
This sample shows how to apply a wave effect using depth image given by the depth sensor. A wave propagation is simulated by a wave equation.
This effect is used in *KAGURA for PerC*. 

See the URL below for more information about *KAGURA for PerC*. 
http://software.intel.com/sites/campaigns/perceptualshowcase/kagura.htm
