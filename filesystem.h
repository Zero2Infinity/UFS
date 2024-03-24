/*************************
Creator: Rahul A. Patel	 *
Date: July 2012			 *
**************************/

#ifndef _FILESYSTEM_
#define _FILESYSTEM_
#include <string>
#include "constants.h"
#include "disk.h"

class CBootSector {
	protected: 
		char diskname[15];						// 15 bytes
		char owner[15];			 				// 15 bytes
		unsigned int disksize;					// 4 bytes
		unsigned int usedspace;					// 4 bytes
	public:
		CBootSector();
		// setter and getter methods related to Disk
		int setDiskName(const char*);
		int getDiskName(const char*);
		int setOwner(const char*);
		int getOwner(const char*);
		int setSize(int);
		int getSize(int);
		int setUsedSpace(int);
		int getUsedSpace(int);
		// release the Block 0
		~CBootSector();		
};

class CSuperBlock {
	protected:
		unsigned char bvInodes[BVINODESSIZE];	// 80 bytes
		unsigned char bvSectors[BVSECTORSSIZE]; // 80 bytes
	public:
		// parameter range 0...639
		CSuperBlock();
		void _flip(unsigned char*, int);
		int _checkBit(unsigned char*, int);
		void _locationInBitVector(int**, int*, int*);
		int isInodeFree(int*);
		int isSectorFree(int*);
		void setInode(int*);
		void resetInode(int*);
		void setSector(int*);
		void resetSector(int*);
		int _scanBitVector(const unsigned char*, int**);
		int findFreeInode(int*);
		int findFreeSector(int*);
		~CSuperBlock();
};

class CSectorZero : public CBootSector, public CSuperBlock {
	protected:
		CBootSector *pBootSector;
		CSuperBlock *pSuperBlock;
		char Block0Data[BLOCKSIZE];
	public:
		CSectorZero();
		CBootSector* getBootSector();
		CSuperBlock* getSuperBlock();
		void clearSectorZero();
		~CSectorZero();	
};

class CInode {
	protected:
		int ftype;							// 4 bytes
		int fsize; 							// 4 bytes
		unsigned short int dblocks[4];		// 8 bytes
		int calBlockNumFromInodeNum;		// class variable
		char InodeBlockData[BLOCKSIZE];
		CInode *pInode;
	public:	
		CInode();
		void selectInode(int inode = 0);
		void setFileType(int tp = EMPTY_FILE);
		int getFileType();
		void setFileSize (int);
		int getFileSize();
		int addDataBlock(unsigned short int);
		int removeDataBlock(unsigned short int);
		int saveNode();
		//display function: just for debugging purpose
		void display();
		//int clearAllInodes(); Clear any particular Inode.
		~CInode();
};

class CDirectoryEntry {
	protected:
		int status;							// 4 bytes
		char fileName[FILENAMELENGTH];		// 8 bytes
		int iNode;							// 4 bytes
	public:
		CDirectoryEntry();
		void setStatus(int);
		int getStatus();
		void setFname(const char*);
		void getFname(char*);
		void setInode(int);
		int getInode();
		~CDirectoryEntry();
};

class CDirectory : public CDirectoryEntry {
	protected:
		CDirectoryEntry *pDirEntry;
		int currentBlock;	
		char DirectoryBlockData[BLOCKSIZE];
	public:	
		CDirectory();						// point pDirEntry pointer to first entry.
		void initializeStatus();			// Before using this object, one must call this method. 
		CDirectoryEntry* getDirEntryPointer();
		int addEntry(const char*, int);
		int removeEntry(const char*);
		int isFileNameExists(const char*); 
		int resetDirEntryPointer();
		int writeCurrentBlock(int);
		~CDirectory();	
};

class CFileDescriptor {
	private:
		char fileName[FILENAMELENGTH];
		int fileMode;
		int inode;
		int byteOffset;
		int status;
		CInode iNode;
		int createNewFileDescriptorEntry(const char*);
	public: 
		CFileDescriptor();
		void setFileName(const char*);
		void getFileName(char*);
		void setFileMode(int);
		int getFileMode();
		void setInodeEntry(int);
		CInode getInodeEntry();
		void setByteOffset(int);
		int getByteOffset();
		void setStatus(int);
		int getStatus();
		void openFileCmd(const char*, int);
		void writeToFileCmd(unsigned int, std::string);
		~CFileDescriptor();		
};

#endif // _FILESYSTEM_