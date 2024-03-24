#define BLOCK0 0
#define SUPERBLOCKENTRY 80
#define BLOCKSIZE 256

#define INODEPERSECTOR 16
#define INODEZEROSECTOR 41

#define DIR_ENTRIES 16
#define DIR_BLOCKS 4
#define FILENAMELENGTH 8

#define BVSECTORSSIZE 80   
#define BVINODESSIZE 80

// Type: Regular | Directory
#define EMPTY_FILE 0
#define REGULAR_FILE 1
#define DIRECTORY_FILE 2

// Mode: Read | Write | Append | Read+Write
#define FLAG_NONE 0
#define FLAG_READ 1
#define FLAG_WRITE 2
#define FLAG_APPEND 3
#define FLAG_READWRITE 4

// Directory Entry Status
#define EMPTY -1
#define EXISTS 0
#define DELETED_EMPTY 1

// File Descriptor Table Status
#define MAX_FD_SIZE 32
// #define EMPTY -1 // reused previous one
#define ENTRY_EXISTS 0
#define ENTRY_NOT_EXISTS 1


#define newl "\n"

#define WHITESPACE ' '
#define HASH '#'