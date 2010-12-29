#include <stdio.h>
#include <mntent.h>
#include <malloc.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <unistd.h>

#include <libzfs_mtab.h>

#if !defined(FLOCK_DIR)
#define FLOCK_DIR   "/var/lock/zfs/"
#endif

#if !defined(FLOCK_NAME)
#define FLOCK_NAME  "mtab_lock"
#endif

#if !defined(FLOCK_PATH)
#define FLOCK_PATH  (FLOCK_DIR FLOCK_NAME)
#endif

/* Add entry in the /etc/mtab */
int zfs_linux_add_entry(char *mountpoint, char *zfs_name, const char *mtab_file,
	char *mountopt)
{
	struct mntent mnt;
	FILE *mtab = NULL;
	int lock_fd;

	bzero(&mnt, sizeof(struct mntent));

	mnt.mnt_dir    =  mountpoint;
	mnt.mnt_type   = "zfs";
	mnt.mnt_fsname = zfs_name;
	if (!system("selinuxenabled > /dev/null 2>&1")) {
		mnt.mnt_opts = (strcmp(mountopt, MNTOPT_RW) == 0) ? 
			"rw,context=\"system_u:object_r:file_t:s0\"" :
			"ro,context=\"system_u:object_r:file_t:s0\"";
	} else {
		mnt.mnt_opts = mountopt;
	}
	mnt.mnt_freq   = 0;
	mnt.mnt_passno = 0;

	if ((lock_fd = open(FLOCK_PATH, O_RDONLY)) < 0) {
		fprintf(stderr, "%s(%d): open: %s.\n", __FUNCTION__, __LINE__, 
				strerror(errno));
		return -1;
	}

	if (flock(lock_fd, LOCK_EX) < 0) {
		fprintf(stderr, "%s(%d): flock: %s.\n", __FUNCTION__, __LINE__, 
				strerror(errno));
		close(lock_fd);
		return -1;
	}

	if (NULL == (mtab = setmntent(mtab_file, "a+"))) {
		flock(lock_fd, LOCK_UN);
		close(lock_fd);
		return -1;
	}

	if (addmntent(mtab, &mnt) != 0) {
		fprintf(stderr, "%s(%d): addmntent: %s.\n", __FUNCTION__, __LINE__, 
				strerror(errno));
		endmntent(mtab);
		flock(lock_fd, LOCK_UN);
		close(lock_fd);
		return -1;
	}

	endmntent(mtab);

	if (flock(lock_fd, LOCK_UN) < 0) {
		fprintf(stderr, "%s(%d): flock: %s.\n", __FUNCTION__, __LINE__, 
				strerror(errno));
		close(lock_fd);
		return -1;
	}
	close(lock_fd);

	return 0;
}

int zfs_linux_remove_entry(const char *mountpoint, const char *zfs_name, const char *mtab_file)
{
	char   dup_mtab_file[PATH_MAX];
	FILE   *mtab            = NULL;
	FILE   *dmtab           = NULL;
	struct mntent *mntent   = NULL;
	struct stat buf;
	int lock_fd = -1;
	int lock    =  0;
	int rc      = -1;

	sprintf(dup_mtab_file, "%s.tmp.%d", mtab_file, getpid());

	if (lstat(mtab_file, &buf) < 0) {
		perror("lstat ");
		goto ERROR;
	}

	if (S_ISLNK(buf.st_mode)) {
		/*
		 * probably, /etc/mtab is link to /proc/mounts
		 * we don't modify the /proc/mounts return success
		 * */
		return 0;
	}

	if ((lock_fd = open(FLOCK_PATH, O_RDONLY)) < 0) {
		goto ERROR;
	}

	if (flock(lock_fd, LOCK_EX) < 0) {
		fprintf(stderr, "%s(%d): flock: %s.\n", __FUNCTION__, __LINE__, 
				strerror(errno));
		goto ERROR;
	}
	lock = 1; /* file is locked */

	if (NULL == (mtab = setmntent(mtab_file, "r"))) {
		perror("setmntent ");
		goto ERROR;
	}

	if (NULL == (dmtab = setmntent(dup_mtab_file, "w"))) {
		perror("setmntent ");
		goto ERROR;
	}

	/* skip one entry and add rest of entries in the a duplicate file */
	while (NULL != (mntent = getmntent(mtab))) {
		if (!strcmp(mntent->mnt_dir, mountpoint))
			continue;
		if (addmntent(dmtab, mntent) != 0) {
			perror("addmntent ");
			goto ERROR;
		}
	}
	endmntent(dmtab);
	dmtab = NULL;

	if (rename(dup_mtab_file, mtab_file) < 0) {
		fprintf(stderr, "%s(%d): rename: %s.\n", __FUNCTION__, __LINE__, 
				strerror(errno));
		goto ERROR;
	}

	if (flock(lock_fd, LOCK_UN) < 0) {
		fprintf(stderr, "%s(%d): flock: %s.\n", __FUNCTION__, __LINE__, 
				strerror(errno));
		goto ERROR;
	}
	lock = 0; /* file no more locked */

	rc = 0;
ERROR:
	if (mtab)
		endmntent(mtab);

	if (dmtab)
		endmntent(dmtab);

	if (lock)
		flock(lock_fd, LOCK_UN);

	if (lock_fd > 0)
		close(lock_fd);
	return rc;
}
