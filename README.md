# robotino_api2

This is an unofficial git version of svn repository for Robotino API2.

I had a lot of problems with compiling the API2 for Windows. I had to make some small changes in source code, to be able to succesfully compile it. This repository contains these changes. I don't offer any guaranties it will work for other peoples as well.

How to compile Robotino API2?
1. Install QT5 with libraries for Visual Studio 2017(even if you do not plan to make graphical programing)
2. Install Doxygen
3. Install Visaul Studio 2017(Be sure you installed is with cmake development tools option enabled)
3. Download this repository
4. Set environment variables
5. Compile REC_RPC
6. Compile Robotino Common
7. Compile Robotino API2

Don't try to compile the Robotino examples directly. The cmake scripts provided with the examples are incomplete and do not include all the required modules.

Do be able to compile any Robotino project example use this CMake script instead:

I could miss something. If you have problems with compiling or if you thinck I should describe some steps more precisely I am open to suggestions. 
