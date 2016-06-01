export LLVM_CONFIG="$HOME/Code/llvm_bin/bin/llvm-config"
export CXX=g++

set -e
set -x

$CXX -c $($LLVM_CONFIG --cxxflags) plugin.cpp -o plugin.o
$CXX -g -bundle -Wl,-flat_namespace -Wl,-undefined -Wl,suppress -o libplugin.dylib plugin.o $($LLVM_CONFIG --ldflags) -lclangASTMatchers
