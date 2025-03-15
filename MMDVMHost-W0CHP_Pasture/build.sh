#!/bin/bash

make -j$(nproc)
mv MMDVMHost ../../W0CHP-PiStar-bin
mv RemoteCommand ../../W0CHP-PiStar-bin

make clean

