#ifndef _SPL_FCNTL_H
#define _SPL_FCNTL_H

#define F_FREESP 11

typedef struct flock64 {
	short   l_type;
	short   l_whence;
	loff_t  l_start;
	loff_t  l_len;
	pid_t   l_pid;
} flock64_t;
#endif /* _SPL_FCNTL_H */
