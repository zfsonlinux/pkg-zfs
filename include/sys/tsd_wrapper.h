/*
 *
 *  Copyright (c) 2010 Knowledge Quest Infotech Pvt. Ltd. 
 *  Produced at Knowledge Quest Infotech Pvt. Ltd. 
 *  Written by: Knowledge Quest Infotech Pvt. Ltd. 
 *              zfs@kqinfotech.com
 *
 *  This is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA.
 */
#ifndef _SPL_TSD_WRAPPER_H
#define _SPL_TSD_WRAPPER_H

#include <sys/thread.h>
#include <linux/sched.h>
#include <sys/mutex.h>
#include <linux/list.h>

typedef struct tsd_wrap_thread {
	pid_t			tpid;
	unsigned int		ts_nkeys;
	void			**ts_value;
} tsd_wrap_thread_t;

void tsd_create(uint_t *, void (*)(void *));
void tsd_destroy(uint_t *);
void *tsd_get(uint_t);
int tsd_set(uint_t, void *);
void tsd_exit(void);
void destroy_tsd_destructor(void);

#endif /* _SPL_ACL_IMPL_H */
