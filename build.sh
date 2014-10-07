export PATH="$PATH":`pwd`/../../depot_tools
gclient sync
export GYP_GENERATORS='ninja'
./build/gyp_chromium content/content.gyp --no-circular-check
ninja -C out/Release nw -j4