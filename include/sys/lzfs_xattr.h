#ifndef _LZFS_XATTR_H
#define _LZFS_XATTR_H

extern struct xattr_handler *lzfs_xattr_handlers[];

ssize_t
lzfs_listxattr(struct dentry *dentry, char *buffer, size_t size);

extern struct xattr_handler lzfs_xattr_user_handler;

int
lzfs_xattr_get(struct inode *inode, const char *name,
                    void *buffer, size_t size);
int
lzfs_removexattr(struct dentry *dentry, const char *name);
#endif /* _LZFS_XATTR_H */

