#!/bin/bash
CLINES=`find . -name "*.c" -print0 | xargs -0 cat | wc -l`
HLINES=`find . -name "*.h" -print0 | xargs -0 cat | wc -l`
echo C Files: $CLINES
echo H FIles: $HLINES
