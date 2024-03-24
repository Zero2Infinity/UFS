# UFS
User-level Unix File System for learning purpose
Date: 2012-16

## Basic Structure of FS 

- Block 0 		: BootSector + SuperBlock
- Block 1 to 40		: Inodes
- Block 41 to 639 	: Data Blocks

### BootSector
As of now, this stores disk related information.

### SuperBlock
This keeps track of inodes and blocks information. The management of the free inodes/blocks can achieved using bit-map array.

### Inode
Able to create simple inode.

### Root Directory [inode 0 on Block 41-44]
I've decided to hard-code the location to simplify the retrieval process. 
It can hold upto 64 files (each file info = 16B). 
Potentially, this UFS can hold upto 150 files (600 Data Blocks; 4 Data Blocks per file; 1 root directory; 149 files).

### Limitations
- This simple unix file system is implemented at user-level process.
- This required disk space of 160KB. Pseudo Disk consists of Disk Sector of 256B, 16 Sectors per track, 10 tracks per cylinder, and 4 cylinders. 
- Assume that the maximum size of each file restricted to 4 blocks.
- One-level of inode structure (one "root" directory)


 
