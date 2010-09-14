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

#include <asm/types.h>
#include <linux/errno.h>
#include <linux/hash.h>
#include <sys/tsd_hashtable.h>
#include <linux/list.h>
#include <linux/slab.h>

#ifdef DEBUG_SUBSYSTEM
#undef DEBUG_SUBSYSTEM
#endif

#define DEBUG_SUBSYSTEM S_TSD

static inline struct tsd_hash_entry *
			__tsd_hash_search(struct tsd_hash_table *, int);

/*
 * init_tsd_hash_table - allocates and initializes a hash table
 * @bits: 2^bits bins will be allocated
 */
struct tsd_hash_table *
init_tsd_hash_table(unsigned long bits)
{
	int size = (1 << bits);
	struct hlist_head *bins;
	unsigned int *count;
	struct tsd_hash_table *table;

	bins = (struct hlist_head *) kzalloc(sizeof
				(struct hlist_head) * size, GFP_KERNEL);
	if (bins == NULL)
	    return NULL;
	    
	count = (unsigned int *) kzalloc(sizeof(unsigned int) * size,
				GFP_KERNEL);
	if (count == NULL) {
	    kfree(bins);
	    return NULL;
	}

	table = (struct tsd_hash_table *)
				 kzalloc(sizeof(struct tsd_hash_table),	GFP_KERNEL);
	if (table == NULL) {
	    kfree(bins);
	    kfree(count);
	    return NULL;
	}

	ENTRY;
	table->bits = bits;
	table->bins = bins;
	table->count = count;
	table->entries = 0;

	EXIT;
	return table;
}
EXPORT_SYMBOL(init_tsd_hash_table);


/*
 * fini_tsd_hash_table - dellocates a  hash table
 */
void
fini_tsd_hash_table(struct tsd_hash_table * tsd_hash_table)
{
	if(tsd_hash_table) {
		kfree(tsd_hash_table->count);
		kfree(tsd_hash_table->bins);
	        kfree(tsd_hash_table);
	}
}
EXPORT_SYMBOL(fini_tsd_hash_table);


/*
 * tsd_hash_add - adds an entry to hash table
 * @table: hash table
 *
 * Returns non-zero if the entry cannot be added to the hash table.
 * Assumes that the data is not present in the table.
 */
int
tsd_hash_add(struct tsd_hash_table *table, tsd_wrap_thread_t *tsd_wrap)
{
	unsigned long hash = hash_long((unsigned long)tsd_wrap->tpid, table->bits);
	struct hlist_head *bin = &table->bins[hash];
	struct tsd_hash_entry *new =
		(struct tsd_hash_entry *) kmalloc(sizeof(struct tsd_hash_entry),
				GFP_KERNEL);

	ENTRY;
	if (unlikely(!new)) {
		EXIT;
		return -ENOMEM;
	}
	new->pid = tsd_wrap->tpid;
	new->tsd_wrap = tsd_wrap;
	INIT_HLIST_NODE(&new->list);
	hlist_add_head(&new->list, bin);
	table->entries++;
	table->count[hash]++;

	EXIT;
	return 0;
}


/*
 * tsd_hash_del - delete an entry from hash table
 * @table: hash table
 * @pid: search pid
 *
 * Returns non-zero if entry is not found.
 */
int
tsd_hash_del(struct tsd_hash_table *table, pid_t pid)
{
	struct tsd_hash_entry *entry = __tsd_hash_search(table, pid);

	ENTRY;
	/* Entry not found in table */
	if (!entry) {
		EXIT;
		return -ENODATA;
	}

	hlist_del(&entry->list);
	kfree(entry);

	EXIT;
	return 0;
}


/*
 * __tsd_hash_search - searches hash table for tsd_hash_entry
 * @table: hash table
 * @pid: search pid
 */
struct tsd_hash_entry *
__tsd_hash_search(struct tsd_hash_table *table, pid_t pid)
{
	unsigned long hash = hash_long((unsigned long) pid, table->bits);
	struct hlist_head *bin = &table->bins[hash];
	struct hlist_node *hcurrent;
	struct tsd_hash_entry *entry = NULL;
	struct tsd_hash_entry *match = NULL;
	ENTRY;
	/* Search through chain to find pid */
	hlist_for_each_entry(entry, hcurrent, bin, list) {
		if (entry->pid == pid) {
			match = entry;
			break;
		}
	}
	EXIT;
	return match;
}


/*
 * tsd_hash_search - searches hash table
 * @table: hash table
 * @pid: search pid
 *
 * Returns tsd_wrap data stored in the tsd_hash_entry.
 */
tsd_wrap_thread_t *
tsd_hash_search(struct tsd_hash_table *table, pid_t pid)
{
	struct tsd_hash_entry *entry;

	ENTRY;

	entry = __tsd_hash_search(table, pid);

	/* Search failed */
	if (entry == NULL) {
		return NULL;
	}
	EXIT;
	return entry->tsd_wrap;
}
