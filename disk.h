/*************************
Creator: Rahul A. Patel	 *
Date: July 2012			 *
**************************/

#ifndef _DISK_H_
#define _DISK_H_

#define SECTOR_SIZE 256
#define SECTORS_PER_TRACK 16
#define TRACKS_PER_CYLINDER 10
#define N_CYLINDERS 4
#define DISK_NAME "DISK"

#ifdef __cplusplus
extern "C" {
#endif

// Initialized the disk
int intialize_disk(int);

// block size 256 bytes
// block number - 0 to 639
int disk_read(int, char*);

// block size 256 bytes
// block number - 0 to 639
int disk_write(int, char*);

//disk's activities- 
//ns = the total number of cylinders that the disk has seeked
// since initialized_disk was called.
//nt = the number of transfer that has occurered
void disk_stats(int*, int*);

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif //_DISK_H_
