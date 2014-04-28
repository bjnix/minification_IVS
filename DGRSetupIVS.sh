#!/usr/bin/env bash 
# exit script if any command returns a non-zero exit code.
#set -vx

# The hostname that we expect to be running on:
EXPECT_HOSTNAME="ccsr.ee.mtu.edu"


if [[ ${HOSTNAME} != ${EXPECT_HOSTNAME} ]]; then
    echo "This script ($0) is intended to work only on $EXPECT_HOSTNAME. You are running it from $HOSTNAME."
	echo "If you are in the IVS lab and running this script from your laptop, press Enter."
	echo "If you are NOT in the IVS lab, press Ctrl+C."
	read
fi


IVS_HOSTNAME=ivs.research.mtu.edu
echo
ls
echo
DEST_DIR=/research/bjnix/temp-assimp
DEST_ASSIMP_DIR=/research/bjnix/assimp-lib

echo "I will recursively copy the above files into:"
echo "bjnix@${IVS_HOSTNAME}:${DEST_DIR}"
echo "Press Ctrl+C to cancel or any other key to continue."
read

echo "You may have to enter your password multiple times..."
# make directories in case they doesn't exist
ssh bjnix@${IVS_HOSTNAME} mkdir -p $DEST_DIR
#ssh bjnix@${IVS_HOSTNAME} mkdir -p $DEST_ASSIMP_DIR
# recursively copy files over.
#scp -r * "bjnix@${IVS_HOSTNAME}:$DEST_DIR"

rsync -ah -e ssh --exclude={.svn,.git,*README*,CMakeCache.txt,DGRAssimpMaster,DGRAssimpSlave,*~} --checksum --partial --no-whole-file --inplace --progress . ${IVS_HOSTNAME}:$DEST_DIR
#cd ../assimp-lib
#rsync -ah -e ssh --exclude={.svn,.git,*README*,CMakeCache.txt} --checksum --partial --no-whole-file --inplace --progress . ${IVS_HOSTNAME}:$DEST_ASSIMP_DIR
#cd -
#build assimp

#ssh bjnix@${IVS_HOSTNAME} "cd ${DEST_ASSIMP_DIR} && rm -f CMakeCache.txt && /export/apps/src/cmake/2.8.9/cmake-2.8.9/bin/cmake -DASSIMP_BUILD_SAMPLES=ON -DASSIMP_BUILD_STATIC_LIB=ON -DASSIMP_ENABLE_BOOST_WORKAROUND=ON -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ ."

# recompile for safety's sake
#ssh bjnix@${IVS_HOSTNAME} "cd ${DEST_DIR} && make"

#make

