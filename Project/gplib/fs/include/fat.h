/****************************************************************/
/*                                                              */
/*                            fat.h                             */
/*                                                              */
/*       FAT File System data structures & declarations         */
/*                                                              */
/*                      November 26, 1991                       */
/*                                                              */
/*                      Copyright (c) 1995                      */
/*                      Pasquale J. Villani                     */
/*                      All Rights Reserved                     */
/*                                                              */
/* This file is part of DOS-C.                                  */
/*                                                              */
/* DOS-C is free software; you can redistribute it and/or       */
/* modify it under the terms of the GNU General Public License  */
/* as published by the Free Software Foundation; either version */
/* 2, or (at your option) any later version.                    */
/*                                                              */
/* DOS-C is distributed in the hope that it will be useful, but */
/* WITHOUT ANY WARRANTY; without even the implied warranty of   */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See    */
/* the GNU General Public License for more details.             */
/*                                                              */
/* You should have received a copy of the GNU General Public    */
/* License along with DOS-C; see the file COPYING.  If not,     */
/* write to the Free Software Foundation, 675 Mass Ave,         */
/* Cambridge, MA 02139, USA.                                    */
/****************************************************************/
#ifndef __FAT_H__
#define __FAT_H__

#define D_OPEN_DIR_FOR_STAT	0x80	/* open directory anyway    */

#define D_LFN (D_RDONLY | D_HIDDEN | D_SYSTEM | D_VOLID)

/* FAT file name constants                                              */
#define FNAME_SIZE              8
#define FEXT_SIZE               3

/* FAT deleted flag                                                     */
#define DELETED         '\x5'    /* if first char, delete file   */
#define EXT_DELETED     '\xe5'   /* external deleted flag */

/* Test for 16 bit or 12 bit FAT                                        */
#define SIZEOF_CLST16   2
#define SIZEOF_CLST32   4

/* FAT cluster special flags                                            */
#define FREE                    0x000

#if WITHFAT32 == 1
#define LONG_LAST_CLUSTER       0x0FFFFFFFUL
#define LONG_BAD                0x0FFFFFF7UL
#else
#define LONG_LAST_CLUSTER       0xFFFF
#define LONG_BAD                0xFFF7
#endif
#define MASK16                  0xFFF8
#define BAD16                   0xFFF7
#define MASK12                  0xFF8
#define BAD12                   0xFF7

/* magic constants: even though FF7 is BAD so FF6 could be a valid cluster
   no., MS docs specify that FF5 is the maximal possible cluster number

   for FAT12; similar for 16 and 32 */

#define FAT_MAGIC       4085
#define FAT_MAGIC16     65525U
#define FAT_MAGIC32     268435455UL

#define ISFAT32(x) _ISFAT32(x)

/*
#define _ISFAT32(dpbp)  (((dpbp)->dpb_size)>FAT_MAGIC16 && ((dpbp)->dpb_size)<=FAT_MAGIC32 )
*/
#define _ISFAT32(dpbp)  (((dpbp)->dpb_fatsize)==0)
#define ISFAT16(dpbp)   (((dpbp)->dpb_size)>FAT_MAGIC   && ((dpbp)->dpb_size)<=FAT_MAGIC16 )
#define ISFAT12(dpbp)   (((dpbp)->dpb_size) && ((((dpbp)->dpb_size)-1)<FAT_MAGIC) )
/* dpb_size == 0 for FAT32, hence doing -1 here */

/* FAT file system directory entry                                      */
struct dirent {
	INT8U dir_name[FNAME_SIZE + FEXT_SIZE];   /* Filename + extension in FCB format */
	INT8U dir_attrib;             /* File Attribute               */
	INT8U dir_case;               /* File case                    */
	INT8U dir_crtimems;           /* Milliseconds                 */
	INT16U dir_crtime;             /* Creation time                */
	INT16U dir_crdate;             /* Creation date                */
	INT16U dir_accdate;            /* Last access date             */
	INT16U dir_start_high;         /* High word of the cluster     */
	dostime dir_time;             /* Time file created/updated    */
	dosdate dir_date;             /* Date file created/updated    */
	INT16U dir_start;              /* Starting cluster             */
								  /* 1st available = 2            */
	INT32U dir_size;               /* File size in bytes           */
};

struct lfn_entry {
	INT8U lfn_id;                 /* Sequence number for this LFN entry      */
	INT16U lfn_name0_4[5];       /* First 5 characters of LFN               */
	INT8U lfn_attrib;             /* LFN attribute, should be D_LFN == 0x0f  */
	INT8U lfn_reserved1;
	INT8U lfn_checksum;           /* Checksum for the corresponding 8.3 name */
	INT16U lfn_name5_10[6];      /* Next 6 characters of LFN                */
	INT16U lfn_reserved2;
	INT16U lfn_name11_12[2];     /* Last 2 characters of LFN                */
};

#define DIR_NAME        0
#define DIR_EXT         FNAME_SIZE
#define DIR_ATTRIB      (FNAME_SIZE+FEXT_SIZE)
#define DIR_RESERVED    (FNAME_SIZE+FEXT_SIZE+1)
#define DIR_TIME        (FNAME_SIZE+FEXT_SIZE+11)
#define DIR_DATE        (FNAME_SIZE+FEXT_SIZE+13)
#define DIR_START       (FNAME_SIZE+FEXT_SIZE+15)
#define DIR_SIZE        (FNAME_SIZE+FEXT_SIZE+17)

#define DIRENT_SIZE     32

#endif


