#ifndef _SPL_ZFS_H
#define _SPL_ZFS_H

#include <sys/vnode.h>

/*
 *  Defining magic for ZFS for linux, 0xZFSLZFSL
 */

#define ZFS_MAGIC   0x2fc12fc1

typedef struct vfs {
	int		is_snap;
	struct dentry	*zfsctl_dir_dentry;
	struct dentry	*snap_dir_dentry;
	struct vnode	*vfs_snap_dir; /* snapshot directory pointer */	
	uint32_t	vfs_magic; 		/* magic number */
	uint32_t	vfs_flag;		/* flags */
	uint32_t	vfs_bsize;		/* native block size */
	int		vfs_fstype;		/* file system type index */
  //fsid_t		vfs_fsid;		/* file system id */
	void		*vfs_data;		/* private data */
	dev_t		vfs_dev;		/* device of mounted VFS */
	struct super_block	*vfs_super;	/* pointer to linux super 
						 * block */
	struct vfsmount *vfsmnt;
	void (*vfs_set_inode_ops)(struct inode *inode); /* set inode ops */
} vfs_t;

#define ZFSVFSTOSUPER(zfsvfs)   (zfsvfs->z_vfs->vfs_super)

/*
 * VFS flags.
 */
#define	VFS_RDONLY	0x01		/* read-only vfs */
#define	VFS_NOMNTTAB	0x02		/* vfs not seen in mnttab */
#define	VFS_NOSETUID	0x08		/* setuid disallowed */
#define	VFS_REMOUNT	0x10		/* modify mount options only */
#define	VFS_NOTRUNC	0x20		/* does not truncate long file names */
#define	VFS_UNLINKABLE	0x40		/* unlink(2) can be applied to root */
#define	VFS_PXFS	0x80		/* clustering: global fs proxy vfs */
#define	VFS_UNMOUNTED	0x100		/* file system has been unmounted */
#define	VFS_NBMAND	0x200		/* allow non-blocking mandatory locks */
#define	VFS_XATTR	0x400		/* fs supports extended attributes */
#define	VFS_NODEVICES	0x800		/* device-special files disallowed */
#define	VFS_NOEXEC	0x1000		/* executables disallowed */
#define	VFS_STATS	0x2000		/* file system can collect stats */
#define	VFS_XID		0x4000		/* file system supports extended ids */

#define VFS_ATIME   0x8000		/* file system can modify atime */
#define VFS_SUID    0x10000		/* file system has suid flag set */

#define	VFS_NORESOURCE	"unspecified_resource"
#define	VFS_NOMNTPT	"unspecified_mountpoint"


/*
 * VFS features are implemented as bits set in the vfs_t.
 * The vfs_feature_t typedef is a 64-bit number that will translate
 * into an element in an array of bitmaps and a bit in that element.
 * Developers must not depend on the implementation of this and
 * need to use vfs_has_feature()/vfs_set_feature() routines.
 */
typedef	uint64_t	vfs_feature_t;

#define	VFSFT_XVATTR		0x100000001	/* Supports xvattr for attrs */
#define	VFSFT_CASEINSENSITIVE	0x100000002	/* Supports case-insensitive */
#define	VFSFT_NOCASESENSITIVE	0x100000004	/* NOT case-sensitive */
#define	VFSFT_DIRENTFLAGS	0x100000008	/* Supports dirent flags */
#define	VFSFT_ACLONCREATE	0x100000010	/* Supports ACL on create */
#define	VFSFT_ACEMASKONACCESS	0x100000020	/* Can use ACEMASK for access */
#define	VFSFT_SYSATTR_VIEWS	0x100000040	/* Supports sysattr view i/f */

#define MAXFIDSZ	64

#ifdef HAVE_ZPL
typedef struct fid {
#else
typedef struct lzfs_fid {
#endif
	union {
		long fid_pad;
		struct {
			ushort_t len;		/* length of data in bytes */
			char     data[MAXFIDSZ];/* data (variable len) */
		} _fid;
	} un;
} fid_t;

#define	fid_len		un._fid.len
#define	fid_data	un._fid.data

#ifndef FSTYPSZ
#define FSTYPSZ 16 /* max size of fs identifier */
#endif

typedef struct statvfs64 {
	unsigned long f_bsize;  	/* preferred file system block size */
	unsigned long f_frsize; 	/* fundamental file system block size */
	uint64_t f_blocks;		/* total blocks of f_frsize */
	uint64_t f_bfree;		/* total free blocks of f_frsize */
	uint64_t f_bavail;		/* free blocks avail to non-superuser */
	uint64_t f_files;		/* total # of file nodes (inodes) */
	uint64_t f_ffree;		/* total # of free file nodes */
	uint64_t f_favail;		/* free nodes avail to non-superuser */
	unsigned long f_fsid;		/* file system id (dev for now) */
	char f_basetype[FSTYPSZ];	/* target fs type name, */
	/* null-terminated */
	unsigned long f_flag;		/* bit-mask of flags */
	unsigned long f_namemax;	/* maximum file name length */
	char f_fstr[32];		/* filesystem-specific string */
#if !defined(_LP64)
	unsigned long f_filler[16]; /* reserved for future expansion */
#endif/* _LP64 */
} statvfs64_t;

/*
 * VFS_SYNC flags.
 */
#define SYNC_ATTR       0x01            /* sync attributes only */

#define vfs_devismounted(dev)  (0)
static inline void vfs_clearmntopt(struct vfs *vfs, const char *s) 
{ }

static inline void vfs_setmntopt(struct vfs *vfs, const char *s1, 
		const char *s2, int s3) 
{ }

int	vfs_optionisset(const struct vfs *, const char *, char **);

static inline bool vfs_isoptionset(const struct vfs *vfs, int flag)
{
	return (vfs->vfs_flag & flag);
}

static inline bool vfs_issuid(const struct vfs *vfs)
{
	return (vfs_isoptionset(vfs, VFS_SUID));
}

static inline bool vfs_isexec(const struct vfs *vfs)
{
	return (!vfs_isoptionset(vfs, VFS_NOEXEC));
}

static inline bool vfs_issetuid(const struct vfs *vfs)
{
	return (!vfs_isoptionset(vfs, VFS_NOSETUID));
}

static inline bool vfs_isdevice(const struct vfs *vfs)
{
	return (vfs_isoptionset(vfs, VFS_NODEVICES));
}

static inline bool vfs_isatime(const struct vfs *vfs)
{
	return (vfs_isoptionset(vfs, VFS_ATIME));
}

static inline bool vfs_isxattr(const struct vfs *vfs)
{
	return (vfs_isoptionset(vfs, VFS_XATTR));
}

static inline bool vfs_isreadonly(const struct vfs *vfs)
{
	return (vfs_isoptionset(vfs, VFS_RDONLY));
}

struct mounta { 
	char	*spec;
	char	*dir;
	int	flags;
	char	*fstype;
	char	*dataptr;
	int	datalen;
};

/* VFS_HOLD rele are not implemented for linux port, vfs is just a layered
 * structure.
 */

#define VFS_HOLD(vfs_t)
#define VFS_RELE(vfs_t)

#endif /* SPL_ZFS_H */ 

