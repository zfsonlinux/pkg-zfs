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

#ifndef _SPL_SID_H
#define _SPL_SID_H

#include <sys/kmem.h>

typedef struct ksiddomain {
	char		*kd_name;
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
 *  * crgetsid*/
#define ksid_getid(a)  (0)

#endif /* _SPL_SID_H */
