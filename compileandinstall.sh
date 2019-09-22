#!/bin/bash
git reset --hard HEAD~1
git pull
cd control/src
make clean
make
make install
cd ../control
make clean
make
cp control.elf /u/cs452/tftp/ARM/f5fei
