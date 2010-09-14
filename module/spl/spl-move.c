#include <sys/uio.h>
#include <sys/types.h>

/*
 * Move "n" bytes at byte address "p"; "rw" indicates the direction
 * of the move, and the I/O parameters are provided in "uio", which is
 * update to reflect the data which was moved.  Returns 0 on success or
 * a non-zero errno on failure.
 */
int
uiomove(void *p, size_t n, enum uio_rw rw, struct uio *uio)
{
	struct iovec *iov;
	ulong_t cnt;
	
	/* Currently uiomove only for copy data in the sysspace.
	 */
	ASSERT(uio->uio_segflg == UIO_SYSSPACE);

	while (n && uio->uio_resid) {
		iov = uio->uio_iov;
		cnt = MIN(iov->iov_len, n);
		if (cnt == 0l) {
			uio->uio_iov++;
			uio->uio_iovcnt--;
			continue;
		}
		switch (uio->uio_segflg) {
		case UIO_USERSPACE:
		case UIO_USERISPACE:
			ASSERT(0);
#ifdef HAVE_ZPL
			if (rw == UIO_READ) {
				error = xcopyout_nta(p, iov->iov_base, cnt,
				    (uio->uio_extflg & UIO_COPY_CACHED));
			} else {
				error = xcopyin_nta(iov->iov_base, p, cnt,
				    (uio->uio_extflg & UIO_COPY_CACHED));
			}

			if (error)
				return (error);
			break;
#endif /* HAVE_ZPL */
		case UIO_SYSSPACE:
			if (rw == UIO_READ)
				bcopy(p, iov->iov_base, cnt);
			else
				bcopy(iov->iov_base, p, cnt);
			break;
		}
		iov->iov_base += cnt;
		iov->iov_len -= cnt;
		uio->uio_resid -= cnt;
		uio->uio_loffset += cnt;
		p = (caddr_t)p + cnt;
		n -= cnt;
	}
	return (0);
}
EXPORT_SYMBOL(uiomove);

/*
 * Fault in the pages of the first n bytes specified by the uio structure.
 * 1 byte in each page is touched and the uio struct is unmodified. Any
 * error will terminate the process as this is only a best attempt to get
 * the pages resident.
 */
void
uio_prefaultpages(ssize_t n, struct uio *uio)
{
	struct iovec *iov;
	ulong_t cnt, incr;
	caddr_t p;
	uint8_t tmp;
	int iovcnt;

	iov = uio->uio_iov;
	iovcnt = uio->uio_iovcnt;

	ASSERT(uio->uio_segflg == UIO_SYSSPACE);

	while ((n > 0) && (iovcnt > 0)) {
		cnt = MIN(iov->iov_len, n);
		if (cnt == 0) {
			/* empty iov entry */
			iov++;
			iovcnt--;
			continue;
		}
		n -= cnt;
		/*
		 * touch each page in this segment.
		 */
		p = iov->iov_base;
		while (cnt) {
			switch (uio->uio_segflg) {
			case UIO_USERSPACE:
			case UIO_USERISPACE:
				ASSERT(0);
#ifdef HAVE_ZPL
				if (fuword8(p, &tmp))
					return;
#endif /*HAVE_ZPL*/
				break;
			case UIO_SYSSPACE:
				bcopy(p, &tmp, 1);
				break;
			}
			incr = MIN(cnt, PAGESIZE);
			p += incr;
			cnt -= incr;
		}
		/*
		 * touch the last byte in case it straddles a page.
		 */
		p--;
		switch (uio->uio_segflg) {
		case UIO_USERSPACE:
		case UIO_USERISPACE:
			ASSERT(0);
#ifdef HAVE_ZPL
			if (fuword8(p, &tmp))
				return;
#endif /*HAVE_ZPL*/
			break;
		case UIO_SYSSPACE:
			bcopy(p, &tmp, 1);
			break;
		}
		iov++;
		iovcnt--;
	}
}
EXPORT_SYMBOL(uio_prefaultpages);

/*
 * same as uiomove() but doesn't modify uio structure.
 * return in cbytes how many bytes were copied.
 */
int
uiocopy(void *p, size_t n, enum uio_rw rw, struct uio *uio, size_t *cbytes)
{
	struct iovec *iov;
	ulong_t cnt;
	int iovcnt;
	
	ASSERT(uio->uio_segflg == UIO_SYSSPACE);

	iovcnt = uio->uio_iovcnt;
	*cbytes = 0;

	for (iov = uio->uio_iov; n && iovcnt; iov++, iovcnt--) {
		cnt = MIN(iov->iov_len, n);
		if (cnt == 0)
			continue;

		switch (uio->uio_segflg) {

		case UIO_USERSPACE:
		case UIO_USERISPACE:
			ASSERT(0);
#ifdef HAVE_ZPL
			if (rw == UIO_READ) {
				error = xcopyout_nta(p, iov->iov_base, cnt,
				    (uio->uio_extflg & UIO_COPY_CACHED));
			} else {
				error = xcopyin_nta(iov->iov_base, p, cnt,
				    (uio->uio_extflg & UIO_COPY_CACHED));
			}

			if (error)
				return (error);
#endif /*HAVE_ZPL*/
			break;

		case UIO_SYSSPACE:
			if (rw == UIO_READ)
				bcopy(p, iov->iov_base, cnt);
			else
				bcopy(iov->iov_base, p, cnt);
			break;
		}
		p = (caddr_t)p + cnt;
		n -= cnt;
		*cbytes += cnt;
	}
	return (0);
}
EXPORT_SYMBOL(uiocopy);

/*
 * Drop the next n chars out of *uiop.
 */
void
uioskip(uio_t *uiop, size_t n)
{
	if (n > uiop->uio_resid)
		return;
	while (n != 0) {
		iovec_t	*iovp = uiop->uio_iov;
		size_t		niovb = MIN(iovp->iov_len, n);

		if (niovb == 0) {
			uiop->uio_iov++;
			uiop->uio_iovcnt--;
			continue;
		}
		iovp->iov_base += niovb;
		uiop->uio_loffset += niovb;
		iovp->iov_len -= niovb;
		uiop->uio_resid -= niovb;
		n -= niovb;
	}
}
EXPORT_SYMBOL(uioskip);

