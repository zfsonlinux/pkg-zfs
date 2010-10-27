/*****************************************************************************\
 *  Copyright (C) 2007-2010 Lawrence Livermore National Security, LLC.
 *  Copyright (C) 2007 The Regents of the University of California.
 *  Produced at Lawrence Livermore National Laboratory (cf, DISCLAIMER).
 *  Written by Brian Behlendorf <behlendorf1@llnl.gov>.
 *  UCRL-CODE-235197
 *
 *  This file is part of the SPL, Solaris Porting Layer.
 *  For details, see <http://github.com/behlendorf/spl/>.
 *
 *  The SPL is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation; either version 2 of the License, or (at your
 *  option) any later version.
 *
 *  The SPL is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with the SPL.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#ifndef _SPL_VNODE_H
#define _SPL_VNODE_H

#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/fcntl.h>
#include <linux/buffer_head.h>
#include <linux/dcache.h>
#include <linux/namei.h>
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/fs_struct.h>
#include <linux/mount.h>
#include <sys/kmem.h>
#include <sys/mutex.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/uio.h>
#include <sys/sunldi.h>
#include <sys/cred.h>
#include <sys/pathname.h>
#include <sys/taskq.h>
#include <sys/vfs.h>
#include <sys/fcntl.h>

#define XVA_MAPSIZE     3
#define XVA_MAGIC       0x78766174
#define O_RSYNC         O_SYNC

#define AV_SCANSTAMP_SZ 32              /* length of anti-virus scanstamp */

/*
 * Prior to linux-2.6.33 only O_DSYNC semantics were implemented and
 * they used the O_SYNC flag.  As of linux-2.6.33 the this behavior
 * was properly split in to O_SYNC and O_DSYNC respectively.
 */
#ifndef O_DSYNC
#define O_DSYNC		O_SYNC
#endif

#define FREAD		1
#define FWRITE		2
#define FCREAT		O_CREAT
#define FTRUNC		O_TRUNC
#define FOFFMAX		O_LARGEFILE
#define FSYNC		O_SYNC
#define FDSYNC		O_DSYNC
#define FRSYNC		O_RSYNC
#define FEXCL		O_EXCL
#define FDIRECT		O_DIRECT
#define FAPPEND		O_APPEND

#define FNODSYNC	0x10000 /* fsync pseudo flag */
#define FNOFOLLOW	0x20000 /* don't follow symlinks */

#define AT_TYPE		0x00001
#define AT_MODE		0x00002
#undef  AT_UID		/* Conflicts with linux/auxvec.h */
#define AT_UID          0x00004
#undef  AT_GID		/* Conflicts with linux/auxvec.h */
#define AT_GID          0x00008
#define AT_FSID		0x00010
#define AT_NODEID	0x00020
#define AT_NLINK	0x00040
#define AT_SIZE		0x00080
#define AT_ATIME	0x00100
#define AT_MTIME	0x00200
#define AT_CTIME	0x00400
#define AT_RDEV		0x00800
#define AT_BLKSIZE	0x01000
#define AT_NBLOCKS	0x02000
#define AT_SEQ		0x08000
#define AT_XVATTR	0x10000

#define CRCREAT		0x01
#define RMFILE		0x02

#define B_INVAL		0x01
#define B_TRUNC		0x02

#ifdef HAVE_PATH_IN_NAMEIDATA
# define nd_dentry	path.dentry
# define nd_mnt		path.mnt
#else
# define nd_dentry	dentry
# define nd_mnt		mnt
#endif

#define LOOKUP_XATTR	0x02	/*lookup extended attr dir */
#define IS_DEVVP(vp)	\
	((vp)->v_type == VCHR || (vp)->v_type == VBLK || (vp)->v_type == VFIFO)

typedef enum vtype {
	VNON		= 0,
	VREG		= 1,
	VDIR		= 2,
	VBLK		= 3,
	VCHR		= 4,
	VLNK		= 5,
	VFIFO		= 6,
	VDOOR		= 7,
	VPROC		= 8,
	VSOCK		= 9,
	VPORT		= 10,
	VBAD		= 11
} vtype_t;

/*
 * Permissions.
 */
#define VREAD    00400
#define VWRITE   00200
#define VEXEC	 00100

#define V_APPEND        0x2     /* want to do append only check */


typedef struct vattr {
	enum vtype	va_type;	/* vnode type */
	u_int		va_mask;	/* attribute bit-mask */
	u_short		va_mode;	/* acc mode */
	uid_t 		va_uid;		/* owner uid */
	uid_t	 	va_gid;		/* owner gid */
	long		va_fsid;	/* fs id */
	long		va_nodeid;	/* node # */
	uint32_t	va_nlink;	/* # links */
	u_long		va_size;	/* file size */
	uint32_t	va_blocksize;	/* block size */
	struct timespec va_atime;	/* last acc */
	struct timespec va_mtime;	/* last mod */
	struct timespec va_ctime;	/* last chg */
	dev_t		va_rdev;	/* dev */
	long		va_blocks;	/* space used */
    uint64_t    va_nblocks;     /* space used, TBD, remove above field */
} vattr_t;

typedef struct xoptattr {
	timestruc_t	xoa_createtime;	/* Create time of file */
	uint8_t		xoa_archive;
	uint8_t		xoa_system;
	uint8_t		xoa_readonly;
	uint8_t		xoa_hidden;
	uint8_t		xoa_nounlink;
	uint8_t		xoa_immutable;
	uint8_t		xoa_appendonly;
	uint8_t		xoa_nodump;
	uint8_t		xoa_settable;
	uint8_t		xoa_opaque;
	uint8_t		xoa_av_quarantined;
	uint8_t		xoa_av_modified;
} xoptattr_t;

typedef struct xvattr {
	vattr_t		xva_vattr;	/* Embedded vattr structure */
	uint32_t	xva_magic;	/* Magic Number */
	uint32_t	xva_mapsize;	/* Size of attr bitmap (32-bit words) */
	uint32_t	*xva_rtnattrmapp;	/* Ptr to xva_rtnattrmap[] */
	uint32_t	xva_reqattrmap[XVA_MAPSIZE];	/* Requested attrs */
	uint32_t	xva_rtnattrmap[XVA_MAPSIZE];	/* Returned attrs */
	xoptattr_t	xva_xoptattrs;	/* Optional attributes */
} xvattr_t;

typedef struct vsecattr {
	uint_t		vsa_mask;	/* See below */
	int		vsa_aclcnt;	/* ACL entry count */
	void		*vsa_aclentp;	/* pointer to ACL entries */
	int		vsa_dfaclcnt;	/* default ACL entry count */
	void		*vsa_dfaclentp;	/* pointer to default ACL entries */
	size_t		vsa_aclentsz;	/* ACE size in bytes of vsa_aclentp */
} vsecattr_t;

typedef struct vnode {
	struct file	*v_file;
	kmutex_t	v_lock;		/* protects vnode fields */
	uint_t		v_flag;		/* vnode flags (see below) */
	uint_t		v_count;	/* reference count */
	void		*v_data;	/* private data for fs */
	struct vfs	*v_vfsp;	/* ptr to containing VFS */
	struct stdata	*v_stream;	/* associated stream */
	enum vtype	v_type;		/* vnode type */
	dev_t		v_rdev;		/* device (VCHR, VBLK) */
	gfp_t		v_gfp_mask;	/* original mapping gfp mask */
	struct inode	v_inode;	/* Linux inode */
} vnode_t;

#define VN_SET_VFS_TYPE_DEV(vp, vfsp, type, dev)	{ \
	(vp)->v_vfsp = (vfsp); \
	(vp)->v_type = (type); \
	(vp)->v_rdev = (dev); \
}

vnode_t *specvp(struct vnode *vp, dev_t dev, vtype_t type, struct cred *cr);

#define LZFS_ITOV(inode)	(container_of(inode, vnode_t, v_inode))
#define LZFS_VTOI(vp)	(&(vp)->v_inode)

typedef struct vn_file {
	int		f_fd;		/* linux fd for lookup */
	struct file	*f_file;	/* linux file struct */
	atomic_t	f_ref;		/* ref count */
	kmutex_t	f_lock;		/* struct lock */
	loff_t		f_offset;	/* offset */
	vnode_t		*f_vnode;	/* vnode */
	struct list_head f_list;	/* list referenced file_t's */
} file_t;

typedef struct caller_context {
	pid_t		cc_pid;		/* Process ID of the caller */
	int		cc_sysid;	/* System ID, used for remote calls */
	u_longlong_t	cc_caller_id;	/* Identifier for (set of) caller(s) */
	ulong_t		cc_flags;
} caller_context_t;

extern vnode_t *vn_alloc(int flag);
void vn_free(vnode_t *vp);
extern int vn_open(const char *path, uio_seg_t seg, int flags, int mode,
		   vnode_t **vpp, int x1, void *x2);
extern int vn_openat(const char *path, uio_seg_t seg, int flags, int mode,
		     vnode_t **vpp, int x1, void *x2, vnode_t *vp, int fd);
extern int vn_rdwr(uio_rw_t uio, vnode_t *vp, void *addr, ssize_t len,
		   offset_t off, uio_seg_t seg, int x1, rlim64_t x2,
		   void *x3, ssize_t *residp);
extern int vn_close(vnode_t *vp, int flags, int x1, int x2, void *x3, void *x4);
extern int vn_seek(vnode_t *vp, offset_t o, offset_t *op, caller_context_t *ct);

extern int vn_remove(const char *path, uio_seg_t seg, int flags);
extern int vn_rename(const char *path1, const char *path2, int x1);
extern int vn_getattr(vnode_t *vp, vattr_t *vap, int flags, void *x3, void *x4);
extern int vn_fsync(vnode_t *vp, int flags, void *x3, void *x4);
extern file_t *vn_getf(int fd);
extern void vn_releasef(int fd);
extern int vn_set_pwd(const char *filename);
extern mode_t vn_vtype_to_if(vtype_t);
extern vtype_t vn_get_sol_type(mode_t);
extern void vn_exists(vnode_t *);
int vn_init(void);
extern void vn_invalid(vnode_t *);
void vn_fini(void);
extern int vn_has_cached_data(vnode_t *);
extern void vn_rele_async(vnode_t *, taskq_t *);

#define VN_HOLD(vp)		igrab(LZFS_VTOI(vp))
#define VN_RELE(vp)		iput(LZFS_VTOI(vp))
#define VN_RELE_ASYNC(vp, taskq) vn_rele_async(vp, taskq)

static __inline__ int
vn_putpage(vnode_t *vp, offset_t off, ssize_t size,
	   int flags, void *x1, void *x2) {
	return 0;
} /* vn_putpage() */

#define VOP_CLOSE				vn_close
#define VOP_SEEK				vn_seek
#define VOP_GETATTR				vn_getattr
#define VOP_FSYNC				vn_fsync
#define VOP_PUTPAGE				vn_putpage
#define vn_is_readonly(vp)			0
#define getf					vn_getf
#define releasef				vn_releasef

/*  Some values same as S_xxx entries from stat.h for convenience.
 */
#define        VSUID           04000           /* set user id on execution */
#define        VSGID           02000           /* set group id on execution */
#define        VSVTX           01000           /* save swapped text even after use */

#define VOP_CREATE(dvp, p, vap, ex, mode, vpp, cr, flag, ct, vsap) \
        zfs_create(dvp, p, vap, ex, mode, vpp, cr, flag, ct, vsap)
#define VOP_MKDIR(dp, p, vap, vpp, cr, ct, f, vsap) \
        zfs_mkdir(dp, p, vap, vpp, cr, ct, f, vsap)
#define VOP_SYMLINK(dvp, lnm, vap, tnm, cr, ct, f, vpp) \
        zfs_symlink(dvp, lnm, vap, tnm, cr, ct, f, vpp)
#define VOP_REMOVE(dvp, p, cr, ct, f) \
        zfs_remove(dvp, p, cr, ct, f)
#define VOP_LINK(tdvp, fvp, p, cr, ct, f) \
        zfs_link(tdvp, fvp, p, cr, ct, f)
#define VOP_RENAME(fvp, fnm, tdvp, tnm, cr, ct, f) \
        zfs_rename(fvp, fnm, tdvp, tnm, cr, ct, f)
#define VOP_RMDIR(dp, p, cdir, cr, ct, f) \
        zfs_rmdir(dp, p, cdir, cr, ct, f)
#define VOP_SETATTR(vp, vap, f, cr, ct) \
        zfs_setattr(vp, vap, f, cr, ct)
#define VOP_WRITE(vp , uio, f , cr, ct) \
	zfs_write(vp, uio , f, cr, ct)
#define VOP_SPACE(vp, cmd, fl, f, off, cr, ct) \
	zfs_space(vp, cmd, fl, f, off, cr, ct)

#define MODEMASK              07777
/*
 *Check whether mandatory file locking is enabled.
*/
#define	MANDMODE(mode)		(((mode) & (VSGID|(VEXEC>>3))) == VSGID)
#define	MANDLOCK(vp, mode)	((vp)->v_type == VREG && MANDMODE(mode))


/*
 * VOP_ACCESS flags
 */
#define        V_ACE_MASK      0x1     /* mask represents  NFSv4 ACE permissions */
/* Added as vcexcl_t is one the parameter in zfs_create */
/*
 * Flags for vnode operations.
 */
enum vcexcl    { NONEXCL, EXCL };              /* (non)excl create */

typedef enum vcexcl    vcexcl_t;
#define IFTOVT(mode)			vn_get_sol_type(mode)
#define VTTOIF(vtype)                   vn_vtype_to_if(vtype)
#define Z_IFMT                          0xF000  

extern vnode_t *rootdir;

/* root of its file system */
#define VROOT 0x01
#define VMMAPPED 0x02


void
zfs_inactive(vnode_t *vp, struct cred *cr, caller_context_t *ct);

int
zfs_lookup(vnode_t *dvp, char *nm, vnode_t **vpp, struct pathname *pnp,
    int flags, vnode_t *rdir, struct cred *cr,  caller_context_t *ct,
    int *direntflags, pathname_t *realpnp);

int
zfs_create(vnode_t *dvp, char *name, vattr_t *vap, vcexcl_t excl,
    int mode, vnode_t **vpp, struct cred *cr, int flag, caller_context_t *ct,
    vsecattr_t *vsecp);

int
zfs_readdir(vnode_t *vp, uio_t *uio, struct cred *cr, int *eofp,
	    caller_context_t *ct, int flags, filldir_t filldir, uint64_t *pos);

int
zfs_link(vnode_t *tdvp, vnode_t *svp, char *name, struct cred *cr,
	 caller_context_t *ct, int flags);
int
zfs_remove(vnode_t *dvp, char *name, struct cred *cr, caller_context_t *ct,
	   int flags);
int
zfs_mkdir(vnode_t *dvp, char *dirname, vattr_t *vap, vnode_t **vpp, 
	  struct cred *cr,
	  caller_context_t *ct, int flags, vsecattr_t *vsecp);
int
zfs_rmdir(vnode_t *dvp, char *name, vnode_t *cwd, struct cred *cr,
	  caller_context_t *ct, int flags);
int
zfs_symlink(vnode_t *dvp, char *name, vattr_t *vap, char *link, 
	    struct cred *cr,
	    caller_context_t *ct, int flags, vnode_t **vpp);
int
zfs_rename(vnode_t *sdvp, char *snm, vnode_t *tdvp, char *tnm, struct cred *cr,
	   caller_context_t *ct, int flags);
int
zfs_setattr(vnode_t *vp, vattr_t *vap, int flags, struct cred *cr,
	    caller_context_t *ct);
int
zfs_readlink(vnode_t *vp, uio_t *uio, struct cred *cr, caller_context_t *ct);

int
zfs_read(vnode_t *vp, uio_t *uio, int ioflag,  cred_t *cr,
	 caller_context_t *ct);

int zfs_file_accessed(vnode_t *vp);

int zfs_file_modified(vnode_t *vp);

int
zfs_write(vnode_t *vp, uio_t *uio, int ioflag, cred_t *cr, 
	  caller_context_t *ct);

int
zfs_getattr(vnode_t *vp, vattr_t *vap, int flags, struct cred *cr,
		    caller_context_t *ct);
int
zfs_fsync(vnode_t *vp, int syncflag, struct cred *cr,
		    caller_context_t *ct);
void
xva_init(xvattr_t *xvap);

int zfs_space(vnode_t *vp, int cmd, flock64_t *bfp, int flag,
    offset_t offset, struct cred *cr, caller_context_t *ct);

#ifdef  __cplusplus
}
#endif

#endif /* SPL_VNODE_H */
