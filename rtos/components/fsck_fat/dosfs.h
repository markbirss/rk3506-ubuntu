/*-
 * SPDX-License-Identifier: BSD-2-Clause-FreeBSD
 *
 * Copyright (C) 1995, 1996, 1997 Wolfgang Solfrank
 * Copyright (c) 1995 Martin Husemann
 * Some structure declaration borrowed from Paul Popelka
 * (paulp@uts.amdahl.com), see /sys/msdosfs/ for reference.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *	$NetBSD: dosfs.h,v 1.4 1997/01/03 14:32:48 ws Exp $
 * $FreeBSD$
 */

#ifndef DOSFS_H
#define DOSFS_H

/* support 4Kn disk reads */
#define DOSBOOTBLOCKSIZE_REAL 512
#define DOSBOOTBLOCKSIZE 4096
#define FAT_FS_INFO_SIZE (2 * DOSBOOTBLOCKSIZE)

typedef	unsigned int	cl_t;	/* type holding a cluster number */
typedef	unsigned short	u_int;	/* type holding a cluster number */
typedef	unsigned int	uint;	/* type holding a cluster number */
typedef	unsigned char	u_char;	/* type holding a cluster number */

#define powerof2(x) ((((x)-1)&(x))==0)
#define LONG_BIT	(8 * sizeof(unsigned long))
#define roundup2(x, y)  (((x)+((y)-1))&(~((y)-1))) /* if y is powers of two */

size_t strlcpy(char *dest, const char *src, size_t size);
/*
 * architecture independent description of all the info stored in a
 * FAT boot block.
 */
struct bootblock {
	u_int	bpbBytesPerSec;		/* bytes per sector */
	u_int	bpbSecPerClust;		/* sectors per cluster */
	u_int	bpbResSectors;		/* number of reserved sectors */
	u_int	bpbFATs;		/* number of bpbFATs */
	u_int	bpbRootDirEnts;		/* number of root directory entries */
	unsigned int bpbSectors;		/* total number of sectors */
	u_int	bpbMedia;		/* media descriptor */
	u_int	bpbFATsmall;		/* number of sectors per FAT */
	u_int	SecPerTrack;		/* sectors per track */
	u_int	bpbHeads;		/* number of heads */
	unsigned int bpbHiddenSecs;	/* # of hidden sectors */
	unsigned int bpbHugeSectors;	/* # of sectors if bpbbpbSectors == 0 */
	cl_t	bpbRootClust;		/* Start of Root Directory */
	u_int	bpbFSInfo;		/* FSInfo sector */
	u_int	bpbBackup;		/* Backup of Bootblocks */
	cl_t	FSFree;			/* Number of free clusters acc. FSInfo */
	cl_t	FSNext;			/* Next free cluster acc. FSInfo */

	/* and some more calculated values */
	u_int	flags;			/* some flags: */
#define	FAT32		1		/* this is a FAT32 file system */
					/*
					 * Maybe, we should separate out
					 * various parts of FAT32?	XXX
					 */
	int	ValidFat;		/* valid fat if FAT32 non-mirrored */
	cl_t	ClustMask;		/* mask for entries in FAT */
	cl_t	NumClusters;		/* # of entries in a FAT */
	unsigned int NumSectors;		/* how many sectors are there */
	unsigned int FATsecs;		/* how many sectors are in FAT */
	unsigned int NumFatEntries;	/* how many entries really are there */
	u_int	FirstCluster;		/* at what sector is Cluster CLUST_FIRST */
	u_int	ClusterSize;		/* Cluster size in bytes */

	/* Now some statistics: */
	u_int	NumFiles;		/* # of plain files */
	u_int	NumFree;		/* # of free clusters */
	u_int	NumBad;			/* # of bad clusters */
};

#define	CLUST_FREE	0		/* 0 means cluster is free */
#define	CLUST_FIRST	2		/* 2 is the minimum valid cluster number */
#define	CLUST_RSRVD	0xfffffff6	/* start of reserved clusters */
#define	CLUST_BAD	0xfffffff7	/* a cluster with a defect */
#define	CLUST_EOFS	0xfffffff8	/* start of EOF indicators */
#define	CLUST_EOF	0xffffffff	/* standard value for last cluster */
#define	CLUST_DEAD	0xfdeadc0d	/* error encountered */

/*
 * Masks for cluster values
 */
#define	CLUST12_MASK	0xfff
#define	CLUST16_MASK	0xffff
#define	CLUST32_MASK	0xfffffff

#define	DOSLONGNAMELEN	256		/* long name maximal length */
#define LRFIRST		0x40		/* first long name record */
#define	LRNOMASK	0x1f		/* mask to extract long record
					 * sequence number */

/*
 * Architecture independent description of a directory entry
 */
struct dosDirEntry {
	struct dosDirEntry
		*parent,		/* previous tree level */
		*next,			/* next brother */
		*child;			/* if this is a directory */
	char name[8+1+3+1];		/* alias name first part */
	char lname[DOSLONGNAMELEN];	/* real name */
	uint flags;			/* attributes */
	cl_t head;			/* cluster no */
	unsigned int size;			/* filesize in bytes */
	uint fsckflags;			/* flags during fsck */
};
/* Flags in fsckflags: */
#define	DIREMPTY	1
#define	DIREMPWARN	2

/*
 *  TODO-list of unread directories
 */
struct dirTodoNode {
	struct dosDirEntry *dir;
	struct dirTodoNode *next;
};

#endif
