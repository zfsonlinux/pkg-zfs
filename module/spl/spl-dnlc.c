/*
 *  This file is part of the LZPL: Linux ZFS Posix Layer
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

#include<sys/vnode.h>
#include<sys/vfs.h>

vnode_t neg_cache_vnode;
EXPORT_SYMBOL(neg_cache_vnode);
/*
 * We never find anything in our dnlc cache. Note that negative entry
 * DNLC_NO_VNODE  should be other than NULL
 */

vnode_t *
dnlc_lookup(vnode_t *dp, char *name)
{
	return NULL;

}
EXPORT_SYMBOL(dnlc_lookup);
void
dnlc_update(vnode_t *dp, char *name, vnode_t *vp)
{

}
EXPORT_SYMBOL(dnlc_update);
void
dnlc_remove(vnode_t *dp, char *name)
{

}
EXPORT_SYMBOL(dnlc_remove);
int
dnlc_purge_vfsp(vfs_t *vfsp, int count)
{
	return count;
}
EXPORT_SYMBOL(dnlc_purge_vfsp);

