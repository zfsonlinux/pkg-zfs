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

#ifndef _SPL_HASHTABLE_H
#define _SPL_HASHTABLE_H

#include <linux/list.h>
#include <sys/tsd_wrapper.h>

struct tsd_hash_table {
	unsigned long bits;
	struct hlist_head *bins;
	unsigned int *count;
	unsigned long entries;
};

struct tsd_hash_entry {
	int pid;
	tsd_wrap_thread_t *tsd_wrap;
	struct hlist_node list;
};

struct tsd_hash_table *init_tsd_hash_table(unsigned long);
void fini_tsd_hash_table(struct tsd_hash_table *);
int tsd_hash_add(struct tsd_hash_table *, tsd_wrap_thread_t *);
int tsd_hash_del(struct tsd_hash_table *, int);
tsd_wrap_thread_t *tsd_hash_search(struct tsd_hash_table *, int);

#endif /* _SPL_HASHTABLE_H */
