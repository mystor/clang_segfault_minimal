set -e
set -x

export CLANG="$HOME/Code/llvm_bin/bin/clang++"
$CLANG -cc1 -load libplugin.dylib -add-plugin moz-check file.cpp
