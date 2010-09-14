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

#include <linux/list.h>
#include <sys/thread.h>
#include <sys/kmem.h>
#include <sys/tsd_hashtable.h>
#include <sys/tsd_wrapper.h>

#ifdef DEBUG_SUBSYSTEM
#undef DEBUG_SUBSYSTEM
#endif

#define DEBUG_SUBSYSTEM S_TSD

struct tsd_hash_table *tsd_hash_table = NULL;
EXPORT_SYMBOL(tsd_hash_table);
static uint_t       tsd_nkeys;   /* size of destructor array */
static void         (**tsd_destructor)(void *);
static size_t  size_nkeys; /* Size of tsd_destructor array */

static DEFINE_MUTEX(wrap_mutex);
DEFINE_MUTEX(tsd_mutex);

void tsd_defaultdestructor(void *value)
{
	ENTRY;
	EXIT;
}
EXPORT_SYMBOL(tsd_defaultdestructor);


static void *
tsd_realloc(void *old, size_t osize, size_t nsize)
{
	void *new;

	new = kmem_zalloc(nsize, KM_SLEEP);
	if (!new)
	    return NULL;
	ENTRY;
	if (old) {
		memcpy(old, new, osize);
		kmem_free(old, osize);
	}
	return new;
}
EXPORT_SYMBOL(tsd_realloc);

void
tsd_create(uint_t *keyp, void (*destructor)(void *))
{
	int i;
	uint_t  nkeys;
	/*
	 * if key is allocated, do nothing
	 */
	ENTRY;
	mutex_lock(&tsd_mutex);
	if (*keyp) {
		mutex_unlock(&tsd_mutex);
		EXIT;
		return;
	}
	/*
	 * find an unused key
	 */
	if (destructor == NULL)
		destructor = tsd_defaultdestructor;

	for (i = 0; i < tsd_nkeys; ++i)
		if (tsd_destructor[i] == NULL)
			break;

	/*
	 * if no unused keys, increase the size of the destructor array
	 */
	if (i == tsd_nkeys) {
		if ((nkeys = (tsd_nkeys << 1)) == 0)
			nkeys = 1;
		tsd_destructor =
			(void (**)(void *))tsd_realloc((void *)tsd_destructor,
					(size_t)(tsd_nkeys * sizeof (void (*)(void *))),
					(size_t)(nkeys * sizeof (void (*)(void *))));
		size_nkeys = (size_t)(nkeys * sizeof (void (*)(void *)));
		tsd_nkeys = nkeys;
	}

	/*
	 * allocate the next available unused key
	 */
	tsd_destructor[i] = destructor;
	*keyp = i + 1;
	mutex_unlock(&tsd_mutex);
	EXIT;
}
EXPORT_SYMBOL(tsd_create);

inline tsd_wrap_thread_t *
get_tsd_thread(pid_t pid)
{
	tsd_wrap_thread_t *t;

	ENTRY;
	mutex_lock(&wrap_mutex);

	t = tsd_hash_search(tsd_hash_table, pid);

	if (t != NULL ) {
		mutex_unlock(&wrap_mutex);
		EXIT;
		return t;
	}
	mutex_unlock(&wrap_mutex);
	
	EXIT;
	return NULL;
}
EXPORT_SYMBOL(get_tsd_thread);


tsd_wrap_thread_t *
tsd_wrap_create(void)
{
	tsd_wrap_thread_t *temp;
	struct task_struct *task = curthread;

	ENTRY;
	temp = get_tsd_thread(task->pid);
	if (temp) {
	    EXIT;
	    return temp;
	}

	temp =  kmem_zalloc(sizeof (struct tsd_wrap_thread ), KM_SLEEP);
	if (!temp) {
	    EXIT;
	    return NULL;
	}
	temp->tpid = task->pid;

	EXIT;
	return temp;
}


int
tsd_agent_set(struct tsd_wrap_thread *tsd, uint_t key, void *value)
{
	ENTRY;
	if (key == 0) {
		EXIT;
		return EINVAL;
	}

	if (key <= tsd->ts_nkeys) {
		tsd->ts_value[key - 1] = value;
		EXIT;
		return 0;
	}

	ASSERT(key <= tsd_nkeys);
	/*
	 * lock out tsd_destroy()
	 */
	mutex_lock(&tsd_mutex);

	tsd_hash_add(tsd_hash_table, tsd);

	tsd->ts_value = tsd_realloc((tsd)->ts_value,
			(tsd)->ts_nkeys * sizeof (void *),
			key * sizeof (void *));
	if (tsd->ts_value == NULL) {
		EXIT;
		return EINVAL;
	}
	tsd->ts_nkeys = key;
	tsd->ts_value[key - 1] = value;

	mutex_unlock(&tsd_mutex);
	EXIT;

	return 0;
}

int
tsd_set(unsigned int key, void *value)
{
	tsd_wrap_thread_t *tsd = tsd_wrap_create();

	ENTRY;
	if(unlikely(!tsd)) {
		EXIT;
		return EINVAL;
	}
	EXIT;
	return  tsd_agent_set(tsd, key, value);
}
EXPORT_SYMBOL(tsd_set);


void *
tsd_agent_get(struct tsd_wrap_thread *tsd, uint_t key)
{
	ENTRY;
	if (key && tsd != NULL && key <= tsd->ts_nkeys) {
		EXIT;
		return (tsd->ts_value[key - 1]);
	}
	return (NULL);
}


void *
tsd_get(unsigned int key)
{
	struct task_struct *task = curthread;
	tsd_wrap_thread_t *tsd = get_tsd_thread(task->pid);

	ENTRY;

	if(unlikely(!tsd)) {
		EXIT;
		return NULL;
	}

	EXIT;
	return tsd_agent_get(tsd, key);
}
EXPORT_SYMBOL(tsd_get);

void
tsd_destroy(uint_t *keyp)
{
	uint_t key;
	struct task_struct *task = curthread;

	ENTRY;
	/*
	 * protect the key namespace and our destructor lists
	 */
	mutex_lock(&tsd_mutex);
	key = *keyp;
	*keyp = 0;

	ASSERT(key <= tsd_nkeys);
	if (key != 0) {
		uint_t k = key - 1;
		tsd_wrap_thread_t *tsd = get_tsd_thread(task->pid);
		if(tsd != NULL)  { /* No valid tsd for this key */
			if ((key <= tsd->ts_nkeys) && (tsd->ts_value[k]) &&
				tsd_destructor[k]) {
				(*tsd_destructor[k])(tsd->ts_value[k]);
				tsd->ts_value[k] = NULL;
			}
			tsd_destructor[k] = NULL;
		}
	}
	mutex_unlock(&tsd_mutex);
	EXIT;
}
EXPORT_SYMBOL(tsd_destroy);


void
tsd_exit(void)
{
	int i;
	struct task_struct *task = curthread;
	tsd_wrap_thread_t *tsd = get_tsd_thread(task->pid);

	ENTRY;
	if (tsd == NULL) {
		EXIT;
		return;
	}

	if (tsd->ts_nkeys == 0) {
		kmem_free(tsd, sizeof (*tsd));
		EXIT;
		return;
	}

	mutex_enter(&tsd_mutex);

	for (i = 0; i < tsd->ts_nkeys; i++) {
		if (tsd->ts_value[i] && tsd_destructor[i]) {
			(*tsd_destructor[i])(tsd->ts_value[i]);
			tsd->ts_value[i] = NULL;
		}
	}
	tsd_hash_del(tsd_hash_table, task->pid);
	mutex_exit(&tsd_mutex);
	EXIT;

}
EXPORT_SYMBOL(tsd_exit);

/* Free the space allocated to tsd_destructor */
void destroy_tsd_destructor()
{
	kmem_free(tsd_destructor, size_nkeys);
}
EXPORT_SYMBOL(destroy_tsd_destructor);
