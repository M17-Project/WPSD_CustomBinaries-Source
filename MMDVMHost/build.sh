#!/bin/bash

make -j$(nproc)
mv MMDVMHost ../../W0CHP-PiStar-bin
mv RemoteCommand ../../W0CHP-PiStar-bin

make clean

#make -j$(nproc) -f Makefile.Pi.Adafruit
#mv MMDVMHost_Adafruit ../../W0CHP-PiStar-bin
#make clean


