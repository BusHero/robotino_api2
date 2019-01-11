ROBOTINO_API2_DIR = $$PWD
message(Robotino API2 located in $$ROBOTINO_API2_DIR)

INCLUDEPATH += $$ROBOTINO_API2_DIR/include

win32:!android {
        message(WIN32)
	CONFIG( debug, debug|release ) {
            LIBS += "$$ROBOTINO_API2_DIR/lib/rec_robotino_api2d.lib"
            DLL = "$$ROBOTINO_API2_DIR/bin/rec_robotino_api2d.dll"
            QMAKE_PRE_LINK += $${QMAKE_COPY} "$$shell_path($${DLL})" debug
        } else {
            LIBS += "$$ROBOTINO_API2_DIR/lib/rec_robotino_api2.lib"
			DLL = "$$ROBOTINO_API2_DIR/bin/rec_robotino_api2.dll"
            QMAKE_PRE_LINK += $${QMAKE_COPY} "$$shell_path($${DLL})" release
        }
}

unix:!mac:!android {
            LIBS += "$$ROBOTINO_API2_DIR/lib/librec_robotino_api2.so"
}

android: {
    message(ANDROID)

	#build-rec_robotino_api2-Android_for_armeabi_v7a_GCC_4_9_Qt_5_9_0_android_armv7-Release
	
    LIBPATH_PREFIX = "C:/srcs/svn.openrobotino.org/build-"
    LIBPATH_SUFFIX = "-Android_for_armeabi_v7a_GCC_4_9_Qt_5_9_1_android_armv7-"
    LIBPATH_DEBUG_SUFFIX = "Debug"
    LIBPATH_RELEASE_SUFFIX = "Release"
	LIBPATH_BUILDTYPE_SUFFIX = $$LIBPATH_RELEASE_SUFFIX

    CONFIG( debug, debug|release ) {
		LIBPATH_BUILDTYPE_SUFFIX = $$LIBPATH_DEBUG_SUFFIX
    }
	
	LIBS += "$${LIBPATH_PREFIX}rec_robotino_api2$${LIBPATH_SUFFIX}$${LIBPATH_BUILDTYPE_SUFFIX}/librec_robotino_api2.a" \
            "$${LIBPATH_PREFIX}rec_cv_lt$${LIBPATH_SUFFIX}$${LIBPATH_BUILDTYPE_SUFFIX}/librec_cv_lt.a" \
            "$${LIBPATH_PREFIX}rec_robotino_rpc$${LIBPATH_SUFFIX}$${LIBPATH_BUILDTYPE_SUFFIX}/librec_robotino_rpc.a" \
            "$${LIBPATH_PREFIX}rec_rpc$${LIBPATH_SUFFIX}$${LIBPATH_BUILDTYPE_SUFFIX}/librec_rpc.a"
}

#MacOSX and iOS
mac: {
    message(MAC)
    #MacOSX
    macx: {
        message(MACX)
        CONFIG( debug, debug|release ) {
            LIBS += $$ROBOTINO_API2_DIR/lib/librec_robotino_api2_clang64_debug.a \
                    $$ROBOTINO_API2_DIR/lib/librec_cv_lt_clang64_debug.a \
                    $$ROBOTINO_API2_DIR/lib/librec_robotino3_fleetcom_clang64_debug.a \
                    $$ROBOTINO_API2_DIR/lib/librec_robotino_rpc_clang64_debug.a \
                    $$ROBOTINO_API2_DIR/lib/librec_rpc_clang64_debug.a
        } else {
            LIBS += $$ROBOTINO_API2_DIR/lib/librec_robotino_api2_clang64_release.a \
                    $$ROBOTINO_API2_DIR/lib/librec_cv_lt_clang64_release.a \
                    $$ROBOTINO_API2_DIR/lib/librec_robotino3_fleetcom_clang64_release.a \
                    $$ROBOTINO_API2_DIR/lib/librec_robotino_rpc_clang64_release.a \
                    $$ROBOTINO_API2_DIR/lib/librec_rpc_clang64_release.a
        }
    }
    #iOS
    !macx: {
        message(!MACX)
        CONFIG( debug, debug|release ) {
            CONFIG( iphoneos, iphoneos|iphonesimulator ) {
                LIBS += $$ROBOTINO_API2_DIR/lib/librec_robotino_api2_iphoneos_debug.a \
                        $$ROBOTINO_API2_DIR/lib/librec_cv_lt_iphoneos_debug.a \
                        $$ROBOTINO_API2_DIR/lib/librec_robotino3_fleetcom_iphoneos_debug.a \
                        $$ROBOTINO_API2_DIR/lib/librec_robotino_rpc_iphoneos_debug.a \
                        $$ROBOTINO_API2_DIR/lib/librec_rpc_iphoneos_debug.a
            } else {
                LIBS += $$ROBOTINO_API2_DIR/lib/librec_robotino_api2_iphonesimulator_debug.a \
                        $$ROBOTINO_API2_DIR/lib/librec_cv_lt_iphonesimulator_debug.a \
                        $$ROBOTINO_API2_DIR/lib/librec_robotino3_fleetcom_iphonesimulator_debug.a \
                        $$ROBOTINO_API2_DIR/lib/librec_robotino_rpc_iphonesimulator_debug.a \
                        $$ROBOTINO_API2_DIR/lib/librec_rpc_iphonesimulator_debug.a
            }
        } else {
            CONFIG( iphoneos, iphoneos|iphonesimulator ) {
                LIBS += $$ROBOTINO_API2_DIR/lib/librec_robotino_api2_iphoneos_release.a \
                        $$ROBOTINO_API2_DIR/lib/librec_cv_lt_iphoneos_release.a \
                        $$ROBOTINO_API2_DIR/lib/librec_robotino3_fleetcom_iphoneos_release.a \
                        $$ROBOTINO_API2_DIR/lib/librec_robotino_rpc_iphoneos_release.a \
                        $$ROBOTINO_API2_DIR/lib/librec_rpc_iphoneos_release.a
            } else {
                LIBS += $$ROBOTINO_API2_DIR/lib/librec_robotino_api2_iphonesimulator_release.a \
                        $$ROBOTINO_API2_DIR/lib/librec_cv_lt_iphonesimulator_release.a \
                        $$ROBOTINO_API2_DIR/lib/librec_robotino3_fleetcom_iphonesimulator_release.a \
                        $$ROBOTINO_API2_DIR/lib/librec_robotino_rpc_iphonesimulator_release.a \
                        $$ROBOTINO_API2_DIR/lib/librec_rpc_iphonesimulator_release.a
            }
        }
    }
}
