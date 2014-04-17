#!/bin/bash

cd ../linux-2.6.33-lpc313x/


for i in ./../patches/0* ;do
echo $i
patch -Np2 < $i
done

