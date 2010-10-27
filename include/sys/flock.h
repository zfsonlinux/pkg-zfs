#ifndef _SPL_FLOCK_H
#define _SPL_FLOCK_H

#include <sys/vnode.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/callb.h>

typedef enum {FLK_BEFORE_SLEEP, FLK_AFTER_SLEEP} flk_cb_when_t;

struct flk_callback {
    struct flk_callback *cb_next;	/* circular linked list */
    struct flk_callback *cb_prev;
   callb_cpr_t	*(*cb_callback)(flk_cb_when_t, void *);	/* fcn ptr */
    void		*cb_data;	/* ptr to callback data */
};

typedef struct flk_callback flk_callback_t;

int reclock(struct vnode *, struct flock64 *, int, int, u_offset_t, 
		flk_callback_t *);
int	chklock(struct vnode *, int, u_offset_t, ssize_t, int,
    		caller_context_t *);


int     convoff(struct vnode *, struct flock64 *, int, offset_t);

#endif /* _SPL_FLOCK_H */
