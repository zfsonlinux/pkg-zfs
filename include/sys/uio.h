#ifndef _SPL_UIO_H
#define _SPL_UIO_H

#include <linux/uio.h>
#include <asm/uaccess.h>
#include <sys/types.h>

typedef struct iovec iovec_t;

typedef enum uio_rw {
	UIO_READ =	0,
	UIO_WRITE =	1,
} uio_rw_t;

typedef enum uio_seg {
	UIO_USERSPACE =	0,
	UIO_SYSSPACE =	1,
	UIO_USERISPACE =2,
} uio_seg_t;

typedef struct uio {
	struct iovec	*uio_iov;	/* pointer to array of iovecs */
	int		uio_iovcnt;	/* number of iovecs */
	offset_t	uio_loffset;	/* file offset */
	uio_seg_t	uio_segflg;	/* address space (kernel or user) */
	uint16_t	uio_fmode;	/* file mode flags */
	uint16_t	uio_extflg;	/* extended flags */
	offset_t	uio_limit;	/* u-limit (maximum byte offset) */
	ssize_t		uio_resid;	/* residual count */
} uio_t;

typedef struct aio_req {
	uio_t		*aio_uio;	/* UIO for this request */
	void		*aio_private;
} aio_req_t;

/* XXX: Must be fully implemented when ZVOL is needed, for reference:
 * http://cvs.opensolaris.org/source/xref/onnv/onnv-gate/usr/src/uts/common/os/move.c
 */
extern int
uiomove(void *p, size_t n, enum uio_rw rw, struct uio *uio);

extern void
uio_prefaultpages(ssize_t n, struct uio *uio);

extern int
uiocopy(void *p, size_t n, enum uio_rw rw, struct uio *uio, size_t *cbytes);

extern void
uioskip(uio_t *uiop, size_t n);



#endif /* SPL_UIO_H */
