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

#ifndef _SPL_UIO_H
#define _SPL_UIO_H

#include <linux/uio.h>
#include <asm/uaccess.h>
#include <sys/types.h>

#define	UIO_XUIO	0x004	/* Structure is xuio_t */
typedef struct iovec iovec_t;

typedef struct uioasync_s {
	boolean_t	enabled;	/* Is uioasync enabled? */
    	size_t		mincnt;		/* Minimum byte count for use of */
} uioasync_t;

typedef enum uio_rw {
	UIO_READ =	0,
	UIO_WRITE =	1,
} uio_rw_t;

typedef enum uio_seg {
	UIO_USERSPACE =	0,
	UIO_SYSSPACE =	1,
	UIO_USERISPACE=	2,
} uio_seg_t;

typedef struct uio {
	struct iovec	*uio_iov;
	int		uio_iovcnt;
	offset_t	uio_loffset;
	uio_seg_t	uio_segflg;
	uint16_t	uio_fmode;
	uint16_t	uio_extflg;
	offset_t	uio_limit;
	ssize_t		uio_resid;
} uio_t;
/*
 *  * Extended uio_t uioa_t used for asynchronous uio.
 *   *
 *    * Note: UIOA_IOV_MAX is defined and used as it is in "fs/vncalls.c"
 *     *	 as there isn't a formal definition of IOV_MAX for the kernel.
 *      */
#define	UIOA_IOV_MAX	16

typedef struct uioa_page_s {		/* locked uio_iov state */
	int	uioa_pfncnt;		/* count of pfn_t(s) in *uioa_ppp */
	void	**uioa_ppp;		/* page_t or pfn_t arrary */
	caddr_t	uioa_base;		/* address base */
	size_t	uioa_len;		/* span length */
} uioa_page_t;

typedef struct uioa_s {
	iovec_t		*uio_iov;	/* pointer to array of iovecs */
	int		uio_iovcnt;	/* number of iovecs */
	lloff_t		_uio_offset;	/* file offset */
	uio_seg_t	uio_segflg;	/* address space (kernel or user) */
	uint16_t	uio_fmode;	/* file mode flags */
	uint16_t	uio_extflg;	/* extended flags */
	lloff_t		_uio_limit;	/* u-limit (maximum byte offset) */
	ssize_t		uio_resid;	/* residual count */
	/*
	 * uioa extended members.
	 */
	uint32_t	uioa_state;	/* state of asynch i/o */
	ssize_t		uioa_mbytes;	/* bytes that have been uioamove()ed */
	uioa_page_t	*uioa_lcur;	/* pointer into uioa_locked[] */
	void		**uioa_lppp;	/* pointer into lcur->uioa_ppp[] */
	void		*uioa_hwst[4];	/* opaque hardware state */
	uioa_page_t	uioa_locked[UIOA_IOV_MAX]; /* Per iov locked pages */
} uioa_t;


typedef struct aio_req {
	uio_t		*aio_uio;
	void		*aio_private;
} aio_req_t;

typedef enum xuio_type {
	UIOTYPE_ASYNCIO,
	UIOTYPE_ZEROCOPY,
} xuio_type_t;


#define UIOA_IOV_MAX    16

typedef struct xuio {
	uio_t xu_uio;
	enum xuio_type xu_type;
	union {
		struct {
			uint32_t xu_a_state;
			ssize_t xu_a_mbytes;
			uioa_page_t *xu_a_lcur;
			void **xu_a_lppp;
			void *xu_a_hwst[4];
			uioa_page_t xu_a_locked[UIOA_IOV_MAX];
		} xu_aio;

		struct {
			int xu_zc_rw;
			void *xu_zc_priv;
		} xu_zc;
	} xu_ext;
} xuio_t;

#define XUIO_XUZC_PRIV(xuio)	xuio->xu_ext.xu_zc.xu_zc_priv
#define XUIO_XUZC_RW(xuio)	xuio->xu_ext.xu_zc.xu_zc_rw

/* XXX: Must be fully implemented when ZVOL is needed, for reference:
 *  * http://cvs.opensolaris.org/source/xref/onnv/onnv-gate/usr/src/uts/common/os/move.c
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
