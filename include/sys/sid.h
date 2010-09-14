#ifndef _SPL_SID_H
#define _SPL_SID_H

typedef struct ksiddomain {
	uint_t		kd_ref;
	uint_t		kd_len;
	char		*kd_name;
#ifdef HAVE_ZPL
	avl_node_t	kd_link;
#endif
} ksiddomain_t;

static inline ksiddomain_t *
ksid_lookupdomain(const char *dom)
{
        ksiddomain_t *kd;
	int len = strlen(dom);

        kd = kmem_zalloc(sizeof(ksiddomain_t), KM_SLEEP);
        kd->kd_name = kmem_zalloc(len + 1, KM_SLEEP);
	memcpy(kd->kd_name, dom, len);

        return (kd);
}

static inline void
ksiddomain_rele(ksiddomain_t *ksid)
{
	kmem_free(ksid->kd_name, strlen(ksid->kd_name) + 1);
        kmem_free(ksid, sizeof(ksiddomain_t));
}

/* defined ksid_t which is used in zfs_create in zfs_vnops */
typedef int ksid_t;

/* Defined enum to the KSID_OWNER which is used by crgetsid in zfs_create */
typedef enum ksid_index {
        KSID_USER,
        KSID_GROUP,
        KSID_OWNER,
        KSID_COUNT                      /* Must be last */
} ksid_index_t;

/* Just defining ksid_getid which will not hit as we are returning NULL with
 * crgetsid*/
#define ksid_getid(a)  (0)
#endif /* _SPL_SID_H */
