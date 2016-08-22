#!/bin/sh

# For reference:
# emcc --show-ports

mkdir -p build

emcc -Wall -std=c++11 src/main.cpp src/game/*.cpp src/graphics/*.cpp src/audio/*.cpp external/lodepng/lodepng.cpp -Iexternal -s USE_SDL=2 -s USE_OGG=1 -s USE_VORBIS=1 -O2 -o build/a.man.html --preload-file data
