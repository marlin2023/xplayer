#! /usr/bin/env bash

#--------------------
# Standard options:
export COMMON_FF_CFG_FLAGS=
# export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --prefix=PREFIX"

# Licensing options:
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --enable-gpl"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --enable-nonfree"

# Configuration options:
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --enable-runtime-cpudetect"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --disable-gray"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --disable-swscale-alpha"

# Program options:
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --disable-programs"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --disable-ffmpeg"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --disable-ffplay"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --disable-ffprobe"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --disable-ffserver"

# Documentation options:
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --disable-doc"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --disable-htmlpages"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --disable-manpages"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --disable-podpages"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --disable-txtpages"

# Component options:
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --disable-avdevice"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --enable-avcodec"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --enable-avformat"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --enable-avutil"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --enable-swresample"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --enable-swscale"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --disable-postproc"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --disable-avfilter"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --disable-avresample"

export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --enable-network"

# Hardware accelerators:
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --disable-dxva2"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --disable-vaapi"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --disable-vda"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --disable-vdpau"

# Individual component options:
# export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --disable-everything"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --disable-encoders"
#export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --enable-libfdk-aac "
#export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --enable-libx264 "
#export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --enable-encoder=libx264 "
#export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --enable-encoder=libfdk_aac"
#export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --enable-encoder=mpeg4"

# ./configure --list-decoders
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --disable-decoders"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --enable-decoder=aac"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --enable-decoder=aac_latm"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --enable-decoder=flv"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --enable-decoder=h264"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --enable-decoder=mpeg4"

export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --disable-hwaccels"

# ./configure --list-muxers
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --disable-muxers"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --enable-muxer=mpegts"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --enable-muxer=mp4"

# ./configure --list-demuxers
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --disable-demuxers"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --enable-demuxer=aac"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --enable-demuxer=flv"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --enable-demuxer=hls"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --enable-demuxer=mov"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --enable-demuxer=mpegts"

# ./configure --list-parsers
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --disable-parsers"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --enable-parser=aac"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --enable-parser=aac_latm"
# export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --enable-parser=ac3"
#export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --enable-parser=h263"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --enable-parser=h264"

# ./configure --list-bsf
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --disable-bsfs"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --disable-bsf=mjpeg2jpeg"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --disable-bsf=mjpeg2jpeg"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --disable-bsf=mjpega_dump_header"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --disable-bsf=mov2textsub"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --disable-bsf=text2movsub"

# ./configure --list-protocols
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --disable-protocols"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --enable-protocol=http"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --enable-protocol=hls"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --enable-protocol=file"

#
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --disable-devices"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --disable-filters"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --enable-avfilter"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --enable-filter=movie"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --enable-filter=overlay"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --enable-filter=aresample"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --enable-bsf=h264_mp4toannexb"
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --enable-bsf=aac_adtstoasc"

# External library support:
export COMMON_FF_CFG_FLAGS="$COMMON_FF_CFG_FLAGS --disable-iconv"
