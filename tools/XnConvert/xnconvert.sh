#!/bin/sh
dirname="$(dirname "$(readlink -e "$0")")"
export LD_LIBRARY_PATH="$dirname"/lib
export QT_PLUGIN_PATH="$dirname"/lib
"$dirname"/XnConvert "$@"

