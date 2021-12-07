#!/usr/bin/env bash
cd "$(dirname "$0")"
if [ "$(uname)" == "Darwin" ]; then
  arduino_root=`pwd`/../../cache/Arduino/build/macosx/work/Arduino.app/Contents/Java
else
  arduino_root=`pwd`/../../cache/Arduino/build/linux/work
fi

if [ ! -d $arduino_root ]; then
  echo "Arduino not installed! Run install_arduino.sh" >&2
  exit 1
fi

if [ ! -d libraries ]; then
  mkdir -p libraries/Yabl && cd libraries/Yabl
  ln -s ../../../../src/ .
  ln -s ../../../../examples/ .
  cd ../..
fi

build_root=`pwd`/build
exit_code=0
for sketch in `find ../../examples -name '*.ino'`
do
  echo "Building "$sketch
  sketch_build_dir=$(basename $sketch)
  mkdir -p $build_root/$sketch_build_dir

  cd $arduino_root
  ./arduino-builder -hardware ./hardware -tools ./hardware/tools/avr -tools ./tools-builder -libraries ./libraries -libraries $build_root/../libraries -fqbn arduino:avr:uno -build-path $build_root/$sketch_build_dir $build_root/../$sketch

  [ $? == 0 ] && rm -rf $build_root/$sketch_build_dir || exit_code=1
done

[ $exit_code == 0 ] && rm -rf $build_root || exit 1
