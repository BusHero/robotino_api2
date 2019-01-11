Windows:

configure_myconfig.cache

-debug-and-release
-static 
-opensource 
-no-qt3support 
-qtnamespace 
robotino_api2_qt 
-no-gif 
-no-libmng 
-no-libtiff 
-no-libjpeg 
-no-dsp 
-no-vcproj 
-no-phonon 
-no-webkit 
-no-scripttools
-no-dbus
-no-script
-no-declarative
-no-multimedia

configure -loadconfig myconfig
nmake
nmake install

Linux:

./configure -release -opensource -static -no-qt3support -no-multimedia -no-audio-backend -no-phonon -no-phonon-backend -no-svg -no-webkit -no-javascript-jit -no-script -no-scripttools -no-declarative -no-declarative-debug -qt-zlib -no-gif -no-libpng -no-libmng -no-libtiff -no-libjpeg -no-openssl -no-gui -no-nis -no-cups -no-iconv -no-pch -no-dbus -no-gtkstyle -no-opengl -no-openvg -no-sm -no-xshape -no-xvideo -no-xsync -no-xinerama -no-xcursor -no-xfixes -no-xrandr -no-xrender -no-mitshm -no-fontconfig -no-xinput -no-xkb -no-glib -qtnamespace robotino_api2_qt