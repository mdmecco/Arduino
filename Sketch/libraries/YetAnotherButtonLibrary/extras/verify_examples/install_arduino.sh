#!/usr/bin/env bash
cd "$(dirname "$0")"
cd ../..
[[ ! -d cache ]] || exit 0
mkdir -p cache
cd cache
git clone https://github.com/arduino/Arduino.git
cd Arduino/build
echo 1.8.2 | ant clean dist
if [ "$(uname)" == "Darwin" ]; then
  arduino_root=`pwd`/macosx/work/Arduino.app/Contents/Java
  arduino_binary=`pwd`/macosx/work/Arduino.app/Contents/MacOS/Arduino
else
  arduino_root=`pwd`/linux/work
  arduino_binary=`pwd`/linux/work/arduino
fi
# this doesn't work for some reason
# $arduino_binary --install-library "Bounce2"
cd $arduino_root/libraries
git clone https://github.com/thomasfredericks/Bounce2.git
