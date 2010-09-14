#ifndef _SPL_DNLC_H
#define _SPL_DNLC_H

#define dnlc_reduce_cache(percent)	((void)0)

extern vnode_t * dnlc_lookup(vnode_t *dp, char *name);

extern void dnlc_update(vnode_t *dp, char *name, vnode_t *vp);

extern void dnlc_remove(vnode_t *dp, char *name);

extern int dnlc_purge_vfsp(vfs_t *vfsp, int count);


extern vnode_t neg_cache_vnode;
#define DNLC_NO_VNODE &neg_cache_vnode
#endif /* SPL_DNLC_H */
