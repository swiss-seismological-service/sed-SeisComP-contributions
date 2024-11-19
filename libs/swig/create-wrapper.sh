#!/bin/sh
SOURCE_DIR=../../../../..
BINARY_DIR=$1

COMMON_DIR=src/base/common/libs
EXT_DIR=src/base/contrib-sed/libs

swig -c++ -I$SOURCE_DIR/$COMMON_DIR -I$SOURCE_DIR/$EXT_DIR -I$SOURCE_DIR/src/system/libs -I$BINARY_DIR/$COMMON_DIR -I$BINARY_DIR/$EXT_DIR -interface _strongmotion -o strongmotionPYTHON_wrap.cxx -python strongmotion.i
