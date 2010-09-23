/*
 *  This file is part of the LZPL: Linux ZFS Posix Layer
 *
 *  Copyright (c) 2010 Knowledge Quest Infotech Pvt. Ltd. 
 *  Produced at Knowledge Quest Infotech Pvt. Ltd. 
 *  Written by: Knowledge Quest Infotech Pvt. Ltd. 
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

#ifndef _LZFS_EXPORTFS_H
#define _LZFS_EXPORTFS_H

#include <linux/exportfs.h>
#include <linux/stddef.h>

extern const struct export_operations zfs_export_ops;

#define LZFS_FILEID_INO64_GEN 4
#define LZFS_FILEID_INO64_GEN_PARENT 8
#endif
