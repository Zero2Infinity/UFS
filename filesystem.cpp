/*************************
Creator: Rahul A. Patel	 *
Date: July 2012			 *
**************************/

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include "filesystem.h"

using namespace std;

/* BOOTSECTOR class definition */
CBootSector :: CBootSector() {
	//cout << "CBootSector" << newl;
}

int CBootSector :: setDiskName(const char *d) {
	strcpy(diskname, d);
	return 0;
}

int CBootSector :: getDiskName(const char *d) {
	cout << "Disk Name: " << diskname << newl;
	d = diskname;
	return 0;
}

int CBootSector :: setOwner(const char *o) {
	strcpy(owner, o);
	return 0;
}

int CBootSector :: getOwner(const char *o) {
	cout << "Owner Name: " << owner << newl;
	o = owner;
	return 0;
}

int CBootSector :: setSize(int size) {
	disksize = size;
	return 0;
}

int CBootSector :: getSize(int size) {
	cout << "Disk Size: " << disksize << newl;
	size = disksize;
	return 0;
}

int CBootSector :: setUsedSpace(int usize) {
	usedspace = usize;
	return 0;
}

int CBootSector :: getUsedSpace(int usize) {
	cout << "Disk Used Space: " << usedspace << newl;
	usize = usedspace;
	return 0;
}

CBootSector :: ~CBootSector() {
	//cout << "~CBootSector" << newl;
}


/* SUPERBLOCK class definition */
CSuperBlock :: CSuperBlock() {
	//cout << "CSuperBlock" << newl;
}

void CSuperBlock :: _flip(unsigned char *c, int bitPos) {
	*c ^= ( 1 << bitPos );
}

int CSuperBlock :: _checkBit(unsigned char *c, int bitPos) {
	// return 0 = when bit is reset
	// 		  1 = when bit is set
	return *c & ( 1 << bitPos );
}

void CSuperBlock :: _locationInBitVector(int **num, int *charPos, int *offset) {
	*charPos = **num / 8;
	*offset = **num % 8;
}

int CSuperBlock :: isInodeFree(int *nInode) {
	int charPos = 0; 	// char position in []
	int offset = 0;		// bit location inside char

	_locationInBitVector(&nInode, &charPos, &offset);
	return _checkBit(&bvInodes[charPos], offset) ? 1 : 0;
}

int CSuperBlock :: isSectorFree(int *nSector) {
	int charPos = 0; 
	int offset = 0;

	_locationInBitVector(&nSector, &charPos, &offset);
	return _checkBit(&bvSectors[charPos], offset) ? 1 : 0;
}

void CSuperBlock :: setInode(int *nInode) {
	int charPos = 0; 
	int offset = 0;
	_locationInBitVector(&nInode, &charPos, &offset);
	if (_checkBit(&bvInodes[charPos], offset) == 0)
		_flip(&bvInodes[charPos], offset);
	else 
		cout << "Warning: something is fishy inside setInode" << newl;
}

void CSuperBlock :: resetInode(int *nInode) {
	int charPos = 0; 
	int offset = 0;

	_locationInBitVector(&nInode, &charPos, &offset);
	if (_checkBit(&bvInodes[charPos], offset) == 1)
		_flip(&bvInodes[charPos], offset);
	else 
		cout << "Warning: something is fishy inside resetInode" << newl;
}

void CSuperBlock :: setSector(int *nSector) {
	int charPos = 0; 
	int offset = 0;

	_locationInBitVector(&nSector, &charPos, &offset);
	if (_checkBit(&bvSectors[charPos], offset) == 0)
		_flip(&bvSectors[charPos], offset);
	else 
		cout << "Warning: something is fishy inside setSector" << newl;
}

void CSuperBlock :: resetSector(int *nSector) {
	int charPos = 0; 
	int offset = 0;

	_locationInBitVector(&nSector, &charPos, &offset);
	if (_checkBit(&bvSectors[charPos], offset) == 1)
		_flip(&bvSectors[charPos], offset);
	else 
		cout << "Warning: something is fishy inside resetSector" << newl;
}

int CSuperBlock :: _scanBitVector(const unsigned char *bv, int **num) {
	bool found = false;
	for (int i = 0 ; i < SUPERBLOCKENTRY ; i++)
	{
		unsigned char c = *(bv + i);
		for (int j = 0 ; j < 8 ; j++)
		{
			if ( _checkBit(&c, j) == 0 ) {
				// first empty value found in Bit Vector Array
				// 0: first Inode or sector
				//639: last Inode  or sector
				**num = (i * 8 + j) + 1;
				found = true;
			}
			if (found) break;
		}
		if (found) break;
	}
	if (found) { // on success
		return 0;
	}
	else {
		cout << "Warning: Disk is Full!! Please remove some unecessary files." << newl;
		return -1;
	}
}

int CSuperBlock :: findFreeInode(int *nInode) {
	return _scanBitVector(bvInodes, &nInode);
}

int CSuperBlock :: findFreeSector(int *nSector) {
	return _scanBitVector(bvSectors, &nSector);
}

CSuperBlock :: ~CSuperBlock() {
	//cout << "~CSuperBlock" << newl;
}


/* SECTORZERO class definition */
CSectorZero :: CSectorZero() {
	//cout << "CSectorZero" << newl;
	// initialized CBootSector and CSuperBlock object
	if (disk_read(BLOCK0, Block0Data) == 0) {
		pBootSector = (CBootSector *) (Block0Data);
		pSuperBlock = (CSuperBlock *) (Block0Data + SUPERBLOCKENTRY);

		if (pBootSector == NULL)
			cout << "pBootSector is NULL.. not good :(" << newl;	

		if (pSuperBlock == NULL) 
			cout << "pSuperBLock is NULL.. not good :(" << newl;	
	}
	else {
		cout << "SectorZero isn't accessible " << newl; 
	}
}

CBootSector* CSectorZero :: getBootSector() {
	return pBootSector;
}

CSuperBlock* CSectorZero :: getSuperBlock() {
	return pSuperBlock;
}

void CSectorZero :: clearSectorZero() {
	memset(Block0Data, 0, sizeof(Block0Data));
}

CSectorZero :: ~CSectorZero() {
	//cout << "~CSectorZero" << newl;
	if (disk_write(BLOCK0, Block0Data) != 0)
		cout << "Disk isn't able to write at Block 0" << newl;
}


/* INODE class definition */
CInode :: CInode() {
	//cout << "CInode" << newl;
}

void CInode :: selectInode(int inode /* = 0 */) {
	// calculate block number from inode number
	calBlockNumFromInodeNum = (inode / INODEPERSECTOR) + 1;
	int offset = (inode % INODEPERSECTOR);
	if (disk_read(calBlockNumFromInodeNum, InodeBlockData) == 0) 
		pInode = (CInode *) (InodeBlockData + offset);
	else
		pInode = NULL;
	cout << "pInode = " << pInode << newl;
}

void CInode :: setFileType(int tp) {
	pInode->ftype = tp;
}

int CInode :: getFileType() {
	return pInode->ftype;
}

void CInode :: setFileSize(int fs) {
	pInode->fsize = fs;
}

int CInode :: getFileSize() {
	return pInode->fsize;
}

int CInode :: addDataBlock(unsigned short int nblock) {
	for (int block=0; block<4; block++) {
		if (pInode->dblocks[block] == 0) {
			pInode->dblocks[block] = nblock;
			return 0;
		}
	}
	return -1; // all blocks are occupied
}

int CInode :: removeDataBlock(unsigned short int nblock) {
	if (pInode->dblocks[nblock] != 0) {
		pInode->dblocks[nblock] = 0;
		return 0;
	}
	return -1; // all blocks are occupied
}

int CInode :: saveNode() {
	// save block 
	if (disk_write(calBlockNumFromInodeNum, InodeBlockData) != 0) {
		cout << "Disk isn't able to writeback Inode Block #" << calBlockNumFromInodeNum << newl;
		return -1; // failure
	}
	return 0; // success
}

void CInode :: display() {
	cout << "File Type = " << getFileType() << newl;
	cout << "File Size = " << getFileSize() << newl;
	for (int sectors=0; sectors<4; sectors++) {
		int block = pInode->dblocks[sectors];
		cout << "Blocks[sectors] = sector #" << block << newl;
	}
}

CInode :: ~CInode() {
	// cout << "~CInode" << newl;
}

CDirectoryEntry :: CDirectoryEntry() {
	//cout << "CDirectoryEntry" << newl;
}

void CDirectoryEntry :: setStatus(int s) {
	this->status = s;
}

int CDirectoryEntry :: getStatus() {
	return this->status;
}

void CDirectoryEntry :: setFname(const char *fn) {
	strncpy(this->fileName, fn, FILENAMELENGTH);
} 

void CDirectoryEntry :: getFname(char *fname) {
	if (strcmp(this->fileName, "") != 0) {
		strncpy(fname, this->fileName, FILENAMELENGTH);
	} 
}

void CDirectoryEntry :: setInode(int in) {
	this->iNode = in;
}

int CDirectoryEntry :: getInode() {
	return this->iNode;
}

CDirectoryEntry :: ~CDirectoryEntry() {
	//cout << "~CDirectoryEntry" << newl;
}

/* CDirectory class definition */
CDirectory :: CDirectory() : currentBlock(INODEZEROSECTOR) {
	resetDirEntryPointer();
}

void CDirectory :: initializeStatus() {
	// Inode for Directory file should be created before initialization

	// status = -1 to all entries in the directory
	for (int b=0; b<DIR_BLOCKS; b++) {
		currentBlock = INODEZEROSECTOR + b;
		for (int e=0; e<DIR_ENTRIES; e++, pDirEntry++) {
			pDirEntry->setStatus(EMPTY);
		}
		// write block back
		writeCurrentBlock(currentBlock);
		// read next one till last block
		if (currentBlock < INODEZEROSECTOR + DIR_BLOCKS) {
			if(disk_read(currentBlock + 1, DirectoryBlockData) == 0) 
				pDirEntry = (CDirectoryEntry *) DirectoryBlockData;
		}
	}
	resetDirEntryPointer();
}

CDirectoryEntry* CDirectory :: getDirEntryPointer() {
	return pDirEntry;
}

int CDirectory :: addEntry(const char *fn, int in) {
	if (pDirEntry == NULL) {
		return -1;
	} else {
		int res = isFileNameExists(fn);
		//cout << "is File "<< fn << " exists? " << (res ? "NO" : "YES") << newl;
		if (res != -1) {
			if (res) {
				// pDirEntry should alrady point to empty dir entry.
				pDirEntry->setStatus(EXISTS);
				pDirEntry->setFname(fn);
				pDirEntry->setInode(in);
				cout << "Added "<< fn << " to the directory!" << newl;
				// Write current block after adding new entry
				writeCurrentBlock(currentBlock);
				resetDirEntryPointer();
			}
			else {
				//cout << "File name already exists; Please try other name" << newl;
				return 1;
			}
		}
	}
	return 0; 
}

int CDirectory :: removeEntry(const char *fn) {
	if (pDirEntry == NULL) {
		return -1;
	} else {
		int res = isFileNameExists(fn);
		if (res != -1) {
			if (res == 0) { // found the file
				pDirEntry->setStatus(DELETED_EMPTY);
				pDirEntry->setFname("");
				pDirEntry->setInode(0);	// [REVIEW] Inode 0 = Root directory. Safe to represent empty Inode.
				writeCurrentBlock(currentBlock);
				resetDirEntryPointer();
				return 1;
			} else {
				cout << fn << " doesn't exist" << newl;
			}
		}
	}	
	return 0; // could not found the filename
}

int CDirectory :: isFileNameExists(const char *fn) {
	int dirEntryStatus;
	char *tmpFname = (char*) malloc(FILENAMELENGTH);
	if (fn == NULL) {
		cout<< "filename is empty!!" << newl;
		return -1; // Error: user error
	}
	
	resetDirEntryPointer(); // Do we need to reset pDirEntry pointer?
	
	// for loop - start scanning blocks 41-44
	for (int b=0; b<DIR_BLOCKS; b++) {
		currentBlock = INODEZEROSECTOR + b;
		for (int e=0; e<DIR_ENTRIES; e++, pDirEntry++) {
			dirEntryStatus = pDirEntry->getStatus(); 
			//cout << "Scanning:" << b << e << " dirEntryStatus=" << dirEntryStatus << newl;
			if (dirEntryStatus == EMPTY && b == 0 &&  e == 0) {
				cout<< "Looks like an empty directory." << newl;
				return 1;
			} else if (dirEntryStatus == EMPTY) {
				//must be the last entry; No need to go further
				return 1;
			} else {
				pDirEntry->getFname(tmpFname);	// two possibilities: DELETED_EMPTY or EXISTS
				if (strcmp(fn, tmpFname) == 0) {
					cout << "FOUND AT: Block="<< b << " Entry=" << e << " filename=" << tmpFname << newl;
					return 0; // found it
				}
			}
		}
		if (currentBlock < INODEZEROSECTOR + DIR_BLOCKS) {
			if(disk_read(currentBlock + 1, DirectoryBlockData) == 0) 
				pDirEntry = (CDirectoryEntry *) DirectoryBlockData;
		} else {	
			cout << "Directory could be Full..." << newl;
		}
	}
	cout << "Warning: You shouldn't be here!" << newl;
	return -1; // Error: directory is full  
}

int CDirectory :: resetDirEntryPointer() {
	// Reset pDirEntry to point to first entry.
	if (disk_read(INODEZEROSECTOR, DirectoryBlockData) == 0) {
		pDirEntry = (CDirectoryEntry *) DirectoryBlockData;
		currentBlock = INODEZEROSECTOR;
	}

	if (pDirEntry == NULL) {
		cout << "Not able to access directory entry" << newl; 	
		return 0; // error
	}

	return 1; // success
}

int CDirectory :: writeCurrentBlock(int currentBlock) {
	if (disk_write (currentBlock, DirectoryBlockData) != 0) {
		cout << "Unable to write current block [" << currentBlock << "] back to disk" << newl;
		return 0;
	}
	return 1;
}

CDirectory :: ~CDirectory() {
	//cout << "~CDirectory" << newl;
}

CFileDescriptor :: CFileDescriptor() : status(EMPTY) {
	//cout << "CFileDescriptor" << newl;
}

void CFileDescriptor :: setFileName(const char* fn) {
	strncpy(this->fileName, fn, FILENAMELENGTH);
}

void CFileDescriptor :: getFileName(char* fn) {
	if (strcmp(this->fileName, "") != 0) {
		strncpy(fn, this->fileName, FILENAMELENGTH);
	} 	
}

void CFileDescriptor :: setFileMode(int fm) {
	this->fileMode = fm;
}

int CFileDescriptor :: getFileMode() {
	return this->fileMode;
}

void CFileDescriptor :: setInodeEntry(int n) {
	inode = n;
	iNode.selectInode(inode);

}

CInode CFileDescriptor :: getInodeEntry() {
	return iNode;
}

void CFileDescriptor :: setByteOffset(int offset) {
	this->byteOffset = offset;
}

int CFileDescriptor :: getByteOffset() {
	return this->byteOffset;
}

void CFileDescriptor :: setStatus(int status) {
	this->status = status;
}

int CFileDescriptor :: getStatus() {
	return this->status;
}

int CFileDescriptor :: createNewFileDescriptorEntry(const char* filename) {
	cout <<"Inside createNewFile method" << newl;
	CSectorZero sz;
	int node;
	if (sz.getSuperBlock()->findFreeInode(&node) == 0) {
		cout << "First empty iNode #" << node << newl;
		sz.getSuperBlock()->setInode(&node);
		setInodeEntry(node);
	} else {
		return -1; // Error
	}
	return 0;
}

void CFileDescriptor :: openFileCmd(const char* filename, int mode) {	
	cout << "Inside openFileCmd method" << newl;
	CDirectory root;
	root.initializeStatus(); // [TODO] Make sure this initialized one time during entire execution
	int res = root.isFileNameExists(filename);
	if (res == 0) {	// file already exists
		int _inode = root.getDirEntryPointer()->getInode(); // Get the exisiting file iNode#
		cout << "Debug: _inode = " << _inode << newl;
		iNode.selectInode(_inode);
		// Update Bytes offset appropriately based on mode
		if (mode == FLAG_APPEND) {
			setByteOffset(iNode.getFileSize());	// [TODO] keep a check on this call
		} else {
			setByteOffset(0);
		}
		setStatus(EXISTS); // Update FD Status
	} else if (res == 1) {	// file not found; must be a new entry!
		if (mode == FLAG_WRITE || mode == FLAG_READWRITE) { // doesn't make sense to append to new file.
			int _status = createNewFileDescriptorEntry(filename);
			if (_status == 0) {
				setByteOffset(0); 
				setStatus(EXISTS);
			} else {
				cout<< "Cannot create new file entry in FileDescriptor Table" << newl;
			}
		} else {
			cout << "Error: You're trying access a file which is not present.." << newl;
		}
	}
}

void CFileDescriptor :: writeToFileCmd(unsigned int nBytes, string text) {
	cout << "Text value = " << text << newl;
	CDirectory root;
	// Update to directory entry
	int res = root.addEntry(this->fileName, this->inode);
	
	// Update the iNode fields with incoming information.
	if (nBytes == 0) {
		iNode.setFileType(EMPTY_FILE);
		iNode.setFileSize(nBytes);
		return; // no need to acquire sector/block
	}	
	// [Question] How do I know if I need one or more sectors?
	if (nBytes >= BLOCKSIZE) {
		cout << "Warning: Could not write more than 256bytes at this point." << newl; 
		return;
	}

	// find the first empty sector and add to inode
	int emptySector;
	CSectorZero sz;
	if (sz.getSuperBlock()->findFreeSector(&emptySector) == 0) {
		cout << "First empty Sector #" << emptySector << newl;
		// Add this sector to iNode field
		sz.getSuperBlock()->setSector(&emptySector);
		iNode.addDataBlock(emptySector);
	} else {
		cout << "Could not find a free sector" << newl;
		return; 
	}
	// write to sector and count written bytes and update inode field
	char *data = new char[text.length()];
	strncpy(data, text.c_str() + 1, nBytes);
	data[nBytes + 1] = '\0';
	//cout << "Text length =" << data << newl;
	cout << "Write status = " << disk_write(emptySector, data) << newl;
	iNode.setFileType(REGULAR_FILE);
	iNode.setFileSize(nBytes);
	iNode.saveNode();
	// Testing: display Inode information and sector data
	iNode.display();
	cout << "Read status = " << disk_read(emptySector, data) << newl;
	//cout << "Debug: Sector #" << emptySector << " data = " << data << newl;
}

CFileDescriptor :: ~CFileDescriptor() {
	//cout << "~CFileDescriptor" << newl;
}

