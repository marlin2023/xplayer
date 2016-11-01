#! /usr/bin/env bash

UNAME_SM=$(uname -sm)
echo "===================="
echo "build on $UNAME_SM"
echo "===================="


echo "ANDROID_SDK=$ANDROID_SDK"
echo "ANDROID_NDK=$ANDROID_NDK"

if [ -z "$ANDROID_NDK" -o -z "$ANDROID_SDK" ]; then
    echo "You must define ANDROID_NDK, ANDROID_SDK before starting."
    echo "They must point to your NDK and SDK directories."
    echo ""
    exit 1
fi

#--------------------
# Check the architecture info.
FF_ARCH=$1
if [ -z "$FF_ARCH" ]; then
    echo ""
    echo "You must specific an architecture 'arm, armv7a, x86, ...'."
    echo ""
    exit 1
fi


# first ,try to detect NDK version
FF_NDK_REL=$(grep -o '^r[0-9]*.*' $ANDROID_NDK/RELEASE.TXT 2>/dev/null|cut -b2-)
case "$FF_NDK_REL" in
    9*|10*)
        # we don't use 4.4.3 because it doesn't handle threads correctly.
        if test -d ${ANDROID_NDK}/toolchains/arm-linux-androideabi-4.8
        # if gcc 4.8 is present, it's there for all the archs (x86, mips, arm)
        then
            echo "NDKr$FF_NDK_REL detected"
        else
            echo "You need the NDKr9 or later"
            exit 1
        fi
    ;;
    *)
        echo "You need the NDKr9 or later"
        exit 1
    ;;
esac

#----------------------------
#------set variable values---
FF_BUILD_ROOT=`pwd`/..


#--------------------
echo ""
echo "--------------------"
echo "[*] make NDK standalone toolchain"
echo "--------------------"

FF_MAKE_TOOLCHAIN_FLAGS="$FF_MAKE_TOOLCHAIN_FLAGS --system=darwin-x86_64"

FF_TOOLCHAIN_PATH=$FF_BUILD_ROOT/build/toolchain
FF_MAKE_TOOLCHAIN_FLAGS="$FF_MAKE_TOOLCHAIN_FLAGS --install-dir=$FF_TOOLCHAIN_PATH"

$ANDROID_NDK/build/tools/make-standalone-toolchain.sh \
        $FF_MAKE_TOOLCHAIN_FLAGS \
        --platform=android-19 \
        --toolchain=arm-linux-androideabi-4.8

echo "[*] make NDK standalone toolchain completed"
echo "-------------------------------------------"


#--------------------
echo ""
echo "--------------------"
echo "[*] check ffmpeg env"
echo "--------------------"
export PATH=$FF_TOOLCHAIN_PATH/bin/:$PATH

export CC="arm-linux-androideabi-gcc"
export CXX="arm-linux-androideabi-g++"
export LD=arm-linux-androideabi-ld
export AR=arm-linux-androideabi-ar
export STRIP=arm-linux-androideabi-strip


#----- armv7a begin -----
if [ "$FF_ARCH" = "armv7a" ]; then
    FF_BUILD_NAME=ffmpeg-armv7a
    FF_SOURCE=$FF_BUILD_ROOT/$FF_BUILD_NAME

    FF_CFG_FLAGS="$FF_CFG_FLAGS --arch=arm --cpu=cortex-a8"
    FF_CFG_FLAGS="$FF_CFG_FLAGS --enable-neon"
    FF_CFG_FLAGS="$FF_CFG_FLAGS --enable-thumb"

    FF_EXTRA_CFLAGS="$FF_EXTRA_CFLAGS -march=armv7-a -mcpu=cortex-a8 -mfpu=vfpv3-d16 -mfloat-abi=softfp -mthumb"
    FF_EXTRA_LDFLAGS="$FF_EXTRA_LDFLAGS -Wl,--fix-cortex-a8"

    FF_ASM_OBJ_DIR="libavutil/arm/*.o libavcodec/arm/*.o libswscale/arm/*.o libswresample/arm/*.o"

else
    echo "unknown architecture $FF_ARCH";
    exit 1
fi


#--------------------
echo ""
echo "--------------------"
echo "[*] configure ffmpeg"
echo "--------------------"

FF_SYSROOT=$FF_TOOLCHAIN_PATH/sysroot
FF_PREFIX=$FF_BUILD_ROOT/build/$FF_BUILD_NAME/output
mkdir -p $FF_PREFIX


FF_CFLAGS="-O3 -Wall -pipe \
    -std=c99 \
    -ffast-math \
    -fstrict-aliasing -Werror=strict-aliasing \
    -Wno-psabi -Wa,--noexecstack \
    -DANDROID -DNDEBUG"

export FF_DEP_LIBS=
export COMMON_FF_CFG_FLAGS=

# "--------------------"
# execute sh script to generate COMMON_FF_CFG_FLAGS
#export COMMON_FF_CFG_FLAGS=
. $FF_BUILD_ROOT/ffmpeg/module-default.sh
#echo "====>$COMMON_FF_CFG_FLAGS"

#--------------------
# Standard options:
FF_CFG_FLAGS="$FF_CFG_FLAGS --prefix=$FF_PREFIX"

# Advanced options (experts only):
FF_CFG_FLAGS="$FF_CFG_FLAGS --cross-prefix=arm-linux-androideabi-"
FF_CFG_FLAGS="$FF_CFG_FLAGS --enable-cross-compile"
FF_CFG_FLAGS="$FF_CFG_FLAGS --target-os=linux"
FF_CFG_FLAGS="$FF_CFG_FLAGS --enable-pic"

# Optimization options (experts only):
FF_CFG_FLAGS="$FF_CFG_FLAGS --enable-asm"
FF_CFG_FLAGS="$FF_CFG_FLAGS --enable-inline-asm"


export FF_CFG_FLAGS="$FF_CFG_FLAGS $COMMON_FF_CFG_FLAGS"
echo "[*] cd ffmpeg source directory"
cd $FF_BUILD_ROOT/ffmpeg/ffmpeg-3.1.3
if [ -f "./config.h" ]; then
    echo '[*] make clean'
    make clean
fi

./configure $FF_CFG_FLAGS \
        --extra-cflags="$FF_CFLAGS $FF_EXTRA_CFLAGS" \
        --extra-ldflags="$FF_DEP_LIBS $FF_EXTRA_LDFLAGS"


#--------------------
echo ""
echo "--------------------"
echo "[*] compile ffmpeg"
echo "--------------------"
make -j4
make install




#--------------------
echo ""
echo "--------------------"
echo "[*] link ffmpeg"
echo "--------------------"

$CC -lm -lz -shared --sysroot=$FF_SYSROOT -Wl,--no-undefined -Wl,-z,noexecstack $FF_EXTRA_LDFLAGS \
    -Wl,-soname,libffmpeg.so \
    compat/*.o \
    libavutil/*.o \
    libavcodec/*.o \
    libavformat/*.o \
    libswresample/*.o \
    libswscale/*.o \
    libavfilter/*.o \
    $FF_ASM_OBJ_DIR \
    $FF_DEP_LIBS $FF_EXTRA_LDFLAGS\
    -o $FF_PREFIX/libffmpeg.so

echo ""
echo "----------------------------------"
echo "[*] link ffmpeg library completed."
echo "----------------------------------"