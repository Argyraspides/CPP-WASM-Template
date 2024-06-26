# Refer to https://emscripten.org/docs/getting_started/downloads.html 
cd ./emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh
emcc --version
cd ..
cd ..
mkdir debug
cd ./debug

# Enable compiling with emscripten
header_file="../include/project_template/BUILD_EMCC.h"
new_string=" "
sed -i '2s/.*/'"$new_string"'/' "$header_file"
echo "$header_file definition changed to: $new_string"

cmake -DMY_COMPILER=gcc -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .