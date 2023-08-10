#!/bin/bash

cwd=$(pwd)

# Check for the correct arguments
if [ "$#" -ne 1 ] || ( [ "$1" != "gpsd" ] && [ "$1" != "nogpsd" ] ); then
  echo "Usage: $0 {gpsd|nogpsd}"
  exit 1
fi

## Set the branch name based on the argument passed to the script
#if [ "$1" == "gpsd" ]; then
#  BRANCH="gpsd"
#elif [ "$1" == "nogpsd" ]; then
#  BRANCH="master"
#fi

SRC_DIR=$HOME/dev/WPSD_CustomBinaries-Source
DEST_DIR=$HOME/dev/W0CHP-PiStar-bin

## Switch to the respective branch in the DEST_DIR repository
#if ! git -C "$DEST_DIR" checkout "$BRANCH"; then
#  echo "Error: Failed to switch to branch '$BRANCH' in '$DEST_DIR' repository."
#  exit 1
#fi

# now, select the proper Makefiles for the OSs and gatweays that use 'libgps'
if [ "$1" == "gpsd" ]; then
  MAKEFILE="Makefile"
elif [ "$1" == "nogpsd" ]; then
  MAKEFILE="Makefile.buster" # no libgps, sunsetted now that libgps28 is in buster backports
fi

cd $SRC_DIR/APRSGateway && make clean && make -j$(nproc) && make install && make clean
cd $SRC_DIR/MMDVMHost && make clean && make -j$(nproc) && make install && make clean && \
     make -j$(nproc) -f Makefile.Pi.Adafruit && make -f Makefile.Pi.Adafruit install && make -f Makefile.Pi.Adafruit clean
cd $SRC_DIR/DAPNETGateway && make clean && make -j$(nproc) && make install && make clean
cd $SRC_DIR/DMRGateway && make -f $MAKEFILE clean && make -f $MAKEFILE -j$(nproc) && make -f $MAKEFILE install && make -f $MAKEFILE clean
cd $SRC_DIR/AMBEServer && make clean && make -j$(nproc) && make install && make clean
cd $SRC_DIR/ircDDBGateway && make -f $MAKEFILE clean && make -f $MAKEFILE -j2 && make -f $MAKEFILE install && make -f $MAKEFILE clean
cd $SRC_DIR/M17Gateway && make -f $MAKEFILE clean && make -f $MAKEFILE -j$(nproc) && make -f $MAKEFILE install && make -f $MAKEFILE clean
cd $SRC_DIR/MMDVMCal && make clean && make -j$(nproc) && make install && make clean
cd $SRC_DIR/MMDVM_CM/DMR2YSF && make clean && make -j$(nproc) && make install && make clean
cd $SRC_DIR/MMDVM_CM/DMR2NXDN && make clean && make -j$(nproc) && make install && make clean
cd $SRC_DIR/MMDVM_CM/YSF2DMR && make clean && make -j$(nproc) && make install && make clean
cd $SRC_DIR/MMDVM_CM/YSF2P25 && make clean && make -j$(nproc) && make install && make clean
cd $SRC_DIR/MMDVM_CM/YSF2NXDN && make clean && make -j$(nproc) && make install && make clean
cd $SRC_DIR/MMDVM_CM/NXDN2DMR && make clean && make -j$(nproc) && make install && make clean
cd $SRC_DIR/NXDNClients/NXDNGateway && make -f $MAKEFILE clean && make -f $MAKEFILE -j$(nproc) && make -f $MAKEFILE install && make -f $MAKEFILE clean
cd $SRC_DIR/NXDNClients/NXDNParrot && make clean && make -j$(nproc) && make clean
cd $SRC_DIR/NXDNClients/NXDNGateway && make -f $MAKEFILE clean && make -f $MAKEFILE -j$(nproc) && make -f $MAKEFILE install && make -f $MAKEFILE clean
cd $SRC_DIR/P25Clients && make clean && make -j$(nproc) && make install && make clean
cd $SRC_DIR/YSFClients/YSFGateway && make -f $MAKEFILE clean && make -f $MAKEFILE -j$(nproc) && make -f $MAKEFILE install && make -f $MAKEFILE clean
cd $SRC_DIR/YSFClients/DGIdGateway && make -f $MAKEFILE clean && make -f $MAKEFILE -j$(nproc) && make -f $MAKEFILE install && make -f $MAKEFILE clean
cd $SRC_DIR/YSFClients/YSFParrot && make clean && make -j$(nproc) && make install && make clean
cd $SRC_DIR/NextionDriver && make clean && make -j$(nproc) && make install && make clean
cd $SRC_DIR/teensy_loader_cli && make clean && make -j$(nproc) && make install && make clean

strip `find $DEST_DIR -type f -executable -exec file -i '{}' \; | grep 'x-executable; charset=binary' | sed 's/:.*//g'`

cd $DEST_DIR
git commit -a -m '* Minor: update from upstream & rebuilds'
cd $cwd

exit 0
