#ifndef _SPL_FLOCK_H
#define _SPL_FLOCK_H

#include <sys/vnode.h>
#include <sys/fcntl.h>
#include <sys/types.h>

int     convoff(struct vnode *, struct flock64 *, int, offset_t);

#endif /* _SPL_FLOCK_H */
