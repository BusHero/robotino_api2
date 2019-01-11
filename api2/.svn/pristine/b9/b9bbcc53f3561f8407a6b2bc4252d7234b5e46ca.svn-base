function help () {
    echo "Usage: $0 DIRECTORY" >&2
    echo "    First parameter is the Qt iOS libraries"
    echo "    Second parameter is the Qt framework output";
}		

function contains() {
    string="$1"
    substring="$2"
    if test "${string#*$substring}" != "$string"
    then
        return 0 
    else
        return 1 
    fi
}

function QtArm() {
	for lib_arm in `find $1/lib -name "lib*\.a" -maxdepth 1`; do
		if ! contains $lib_arm "debug" &&  ! contains $lib_arm "iphonesimulator" && ! contains $lib_arm "Declarative"; then

			if ! lipo -info $lib_arm | grep -q x86_64; then
				 lib_arm7sf+=`echo "$lib_arm " `
			fi
			
		fi

	done


	for lib_arm1 in `find $1/plugins/platforms -name "lib*\.a" -maxdepth 1`; do

		if ! contains $lib_arm1 "debug" && ! contains $lib_arm1 "iphonesimulator"; then
			lib_arm7sf+=`echo "$lib_arm1 " `
		fi

	done

	libtool -static $lib_arm7sf -o "$2/libQt_arm.a"
}

function Qti386() {
	for lib_i386 in `find $1/lib -name "lib*\.a" -maxdepth 1`; do

		if ! contains $lib_i386 "debug" &&   contains $lib_i386 "iphonesimulator" && ! contains $lib_i386 "Declarative"; then
			 lib_i386s+=`echo "$lib_i386 " `
		fi

	done

	for lib_i3861 in `find $1/plugins/platforms -name "lib*\.a" -maxdepth 1`; do

		if ! contains $lib_i3861 "debug" &&  contains $lib_i3861 "iphonesimulator"; then
			 lib_i386s+=`echo "$lib_i3861 " `
		fi

	done

	 libtool -static $lib_i386s -o "$2/libQt_i386.a"
}

function createFramework() {
	set -e

	mkdir -p "$2/Qt.framework/Versions/A/Headers"
	#"Link"the""Current""version"to""A"

	/bin/ln -sfh A "$2/Qt.framework/Versions/Current"
	/bin/ln -sfh  Versions/Current/Headers "$2/Qt.framework/Headers"
	/bin/ln -sfh "Versions/Current/Qt" "$2/Qt.framework/Qt"
	# The -a  ensures that the headers maintain the"source modification date so that we don't  constantly
	# cause propagating rebuilds of files that import these headers.
	/bin/cp -a "$1/include/" "$2/Qt.framework/Versions/A/Headers"

	cp -a "$2/Qt" "$2/Qt.framework/Versions/A/"
}

if [ "$#" -ne 2 ] || ! [ -d "$1" ] || ! [ -d "$2" ]; then
    help
else
    QtArm $1 $2
    Qti386 $1 $2
    lipo -arch armv7s "$2/libQt_arm.a" -arch i386 "$2/libQt_i386.a" -create -output "$2/Qt" 2>/dev/zero
    if [ $? != 0 ] ; then
      lipo "$2/libQt_arm.a" "$2/libQt_i386.a" -create -output "$2/Qt"
    fi
    createFramework $1 $2
fi



if false
then

	set -e

	mkdir -p "./Qt/output/Qt.framework/Versions/A/Headers"
	#"Link"the""Current""version"to""A"

	/bin/ln -sfh A "./Qt/output/Qt.framework/Versions/Current"
	/bin/ln -sfh  Versions/Current/Headers "./Qt/output/Qt.framework/Headers"
	/bin/ln -sfh "Versions/Current/Qt" "./Qt/output/Qt.framework/Qt"
	# The -a  ensures that the headers maintain the"source modification date so that we don't  constantly
	# cause propagating rebuilds of files that import these headers.
	/bin/cp -a "./Qt/output/Headers/" "./Qt/output/Qt.framework/Versions/A/Headers"

	cp -a "./Qt/output/Qt" "./Qt/output/Qt.framework/Versions/A/"

fi
