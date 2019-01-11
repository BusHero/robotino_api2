set -e


mkdir -p "${BUILT_PRODUCTS_DIR}/${PRODUCT_NAME}.framework/Versions/A/Headers"

# Link the "Current" version to "A"
/bin/ln -sfh A "${BUILT_PRODUCTS_DIR}/${PRODUCT_NAME}.framework/Versions/Current"
/bin/ln -sfh Versions/Current/Headers "${BUILT_PRODUCTS_DIR}/${PRODUCT_NAME}.framework/Headers"
/bin/ln -sfh "Versions/Current/${PRODUCT_NAME}" "${BUILT_PRODUCTS_DIR}/${PRODUCT_NAME}.framework/${PRODUCT_NAME}"

# The -a ensures that the headers maintain the source modification date so that we don't constantly
# cause propagating rebuilds of files that import these headers.
/bin/cp -a "${BUILT_PRODUCTS_DIR}/../install/usr/local/robotino/api2/include/" "${BUILT_PRODUCTS_DIR}/${PRODUCT_NAME}.framework/Versions/A/Headers"


# make the headers suitable for the framework
for file_h in `find ${BUILT_PRODUCTS_DIR}/${PRODUCT_NAME}.framework/Versions/A/Headers -name "*\.h"` ; do

LC_CTYPE=C && LANG=C && sed -e 's/#include "\(.*\)"/#include <'${PRODUCT_NAME}'\/\1>/g' $file_h > ${file_h}.tmp && mv ${file_h}.tmp ${file_h}

done





set +u
# Avoid recursively calling this script.
if [[ $SF_MASTER_SCRIPT_RUNNING ]]
then
exit 0
fi
set -u
export SF_MASTER_SCRIPT_RUNNING=1


SF_TARGET_NAME=${PRODUCT_NAME} #${PROJECT_NAME}
SF_WRAPPER_NAME="${SF_TARGET_NAME}.framework"

# The following conditionals come from
# https://github.com/kstenerud/iOS-Universal-Framework

if [[ "$SDK_NAME" =~ ([A-Za-z]+) ]]
then
SF_SDK_PLATFORM=${BASH_REMATCH[1]}
else
echo "Could not find platform name from SDK_NAME: $SDK_NAME"
exit 1
fi

if [[ "$SDK_NAME" =~ ([0-9]+.*$) ]]
then
SF_SDK_VERSION=${BASH_REMATCH[1]}
else
echo "Could not find sdk version from SDK_NAME: $SDK_NAME"
exit 1
fi

if [[ "$SF_SDK_PLATFORM" = "iphoneos" ]]
then
SF_OTHER_PLATFORM=iphonesimulator
SF_ARCHS="i386 x86_64"
else
SF_OTHER_PLATFORM=iphoneos
SF_ARCHS="armv7 arm64"
fi

if [[ "$BUILT_PRODUCTS_DIR" =~ (.*)$SF_SDK_PLATFORM$ ]]
then
SF_OTHER_BUILT_PRODUCTS_DIR="${BASH_REMATCH[1]}${SF_OTHER_PLATFORM}"
else
echo "Could not find platform name from build products directory: $BUILT_PRODUCTS_DIR"
exit 1
fi


# Build the other platform.
xcodebuild -project "${PROJECT_FILE_PATH}" -target "${TARGET_NAME}" -configuration "${CONFIGURATION}" -sdk ${SF_OTHER_PLATFORM}${SF_SDK_VERSION} BUILD_DIR="${BUILD_DIR}" OBJROOT="${OBJROOT}" BUILD_ROOT="${BUILD_ROOT}" SYMROOT="${SYMROOT}" ARCHS="${SF_ARCHS}" $ACTION

# Smash the two static libraries into one fat binary and store it in the .framework

for lib_arm in `find ${SF_OTHER_BUILT_PRODUCTS_DIR} -name "lib*\.a" -maxdepth 1`; do

lib_arm7sf+=`echo "$lib_arm " `

done

libtool -static $lib_arm7sf -o "${SF_OTHER_BUILT_PRODUCTS_DIR}/libRobotinoAPI2_arm.a"

for lib_i386 in `find ${BUILT_PRODUCTS_DIR} -name "lib*\.a" -maxdepth 1`; do

lib_i386s+=`echo "$lib_i386 " `

done

libtool -static $lib_i386s -o "${BUILT_PRODUCTS_DIR}/libRobotinoAPI2_i386.a"


lipo "${SF_OTHER_BUILT_PRODUCTS_DIR}/libRobotinoAPI2_arm.a" "${BUILT_PRODUCTS_DIR}/libRobotinoAPI2_i386.a" -create -output "${BUILT_PRODUCTS_DIR}//${SF_WRAPPER_NAME}/Versions/A/${SF_TARGET_NAME}"

rm "${SF_OTHER_BUILT_PRODUCTS_DIR}/libRobotinoAPI2_arm.a"
rm "${BUILT_PRODUCTS_DIR}/libRobotinoAPI2_i386.a"
# Copy the binary to the other architecture folder to have a complete framework in both.
cp -a "${BUILT_PRODUCTS_DIR}/${SF_WRAPPER_NAME}/Versions/A/${SF_TARGET_NAME}" "${SF_OTHER_BUILT_PRODUCTS_DIR}/${SF_WRAPPER_NAME}/Versions/A/${SF_TARGET_NAME}"