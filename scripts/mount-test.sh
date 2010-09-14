#!/bin/bash
#
# A simple script to simply the loading/unloading the ZFS module stack.

SCRIPT_COMMON=common.sh
if [ -f ./${SCRIPT_COMMON} ]; then
. ./${SCRIPT_COMMON}
elif [ -f /usr/libexec/zfs/${SCRIPT_COMMON} ]; then
. /usr/libexec/zfs/${SCRIPT_COMMON}
else
echo "Missing helper script ${SCRIPT_COMMON}" && exit 1
fi

PROG=mount-test.sh

usage() {
cat << EOF
USAGE:
$0 [hvcm] 

DESCRIPTION:
This testcase is used to test the mount/unmout functionality.
e.g. 
./mount-test.sh -c <zpool_configuration> -m <mount/umount count>
./mount-test.sh -c file-raid0 -m 5

It will mount/unmount the pool 5 times. 

OPTIONS:
	-h      Show this message
	-v      Verbose
        -c      Configuration for zpool
	-m 	Mount count 

EOF
}

check_config() {

	if [ ! -f ${ZPOOL_CONFIG} ]; then
		local NAME=`basename ${ZPOOL_CONFIG} .sh`
		ERROR="Unknown config '${NAME}', available configs are:\n"

		for CFG in `ls ${ZPOOLDIR}/ | grep ".sh"`; do
			local NAME=`basename ${CFG} .sh`
			ERROR="${ERROR}${NAME}\n"
		done

		return 1
	fi

	return 0
}

ZPOOL_CONFIG=unknown
ZPOOL_NAME=tank
MOUNT_POINT=/mnt/zfs
MOUNT_COUNT=0

while getopts 'hvc:m:' OPTION; do
	case $OPTION in
	h)
		usage
		exit 1
		;;
	v)
		VERBOSE=1
		;;
	c)
		ZPOOL_CONFIG=${ZPOOLDIR}/${OPTARG}.sh
		ZPOOL_CONFIGURATION=${OPTARG}
		;;
	m)
		MOUNT_COUNT=${OPTARG}
		;;
	?)
		usage
		exit
		;;
	esac
done

if [ $(id -u) != 0 ]; then
	die "Must run as root"
fi

check_config || die "${ERROR}"
. ${ZPOOL_CONFIG}

#Insert the ZFS modules
./zfs.sh

if [ $? -ne 0 ]
then 
   exit 1	
fi 

if [ $? -eq 0 ]
then 
	./zpool-create.sh  -c $ZPOOL_CONFIGURATION -v 
fi 

#Unmount the FS if already mounted
grep $ZPOOL_NAME /proc/mounts
if [ $? -eq 0 ]
then 
	umount /$ZPOOL_NAME
fi

while [ $MOUNT_COUNT -gt 0 ]
do 
	mount -t zfs $ZPOOL_NAME $MOUNT_POINT
	sleep 5
	umount $MOUNT_POINT
	let MOUNT_COUNT=$MOUNT_COUNT-1
done

# Destroy the pool
./zpool-create.sh -d  -c $ZPOOL_CONFIGURATION -v 

#Remove the ZFS modules
sh zfs.sh -u 

exit 0
