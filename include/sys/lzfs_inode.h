#ifndef _LZFS_INODE_H
#define _LZFS_INODE_H

extern void
lzfs_set_inode_ops(struct inode *inode);

extern struct file_system_type lzfs_fs_type;
#endif /* _LZFS_INODE_H */
