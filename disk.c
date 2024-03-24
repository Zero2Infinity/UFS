/*************************
Creator: Rahul A. Patel	 *
Date: July 2012			 *
**************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <math.h>
#include "disk.h"

static int N_SECTORS = SECTORS_PER_TRACK * TRACKS_PER_CYLINDER * N_CYLINDERS;
static FILE *FD = NULL;
// help to calculate performance of the disk
static int n_seeks = 0;
static int n_transfers = 0;
static int current_cylinder = 0;
static int current_track = 0;
int totalBytes = 0;

// local method declaration section
static int create_dummy_disk();
static int LBA_to_CTS(int n, int *c, int *t, int *s);

// static methods: Local to the file and it's playing supporting role.
static int create_dummy_disk()
{
 	int *pData = NULL;
 	int disk_size = SECTOR_SIZE * N_SECTORS;

	pData = (int *) calloc(disk_size, 1);

	FD = fopen(DISK_NAME, "w+");
		if (!FD)
		{
			fprintf(stderr, "Error: Failed to initialized disk\n");
			return -1;
		}

		//initialize disk with all 0
		totalBytes = fwrite(pData, 1, disk_size, FD);

		if (totalBytes != disk_size)
		{
			fprintf(stderr, "Error: Disk is corrupted ...\n");
			return -1;
		}
		return 0;
}		

static int N_to_CTS(int n, int *c, int *t, int *s)
{
	if (n >= N_SECTORS || n < 0) return -1;
	// cylinders: 0 - 3 
	*c = (int)(n / (TRACKS_PER_CYLINDER * SECTORS_PER_TRACK));
	// track: 0 - 9
	*t = (int)floor(fmod((n / SECTORS_PER_TRACK), TRACKS_PER_CYLINDER));
	// sectors: 0 - 15
	*s = (int)fmod(n, SECTORS_PER_TRACK);

	return 0;
}

//methods that are accessible outside the file
int initialize_disk(int clr)
{
 	//file size
 	struct stat fileStat;
 	int disk_size = SECTOR_SIZE * N_SECTORS;

	if (clr && !FD)	// clr = 1 creates the 160KB file named under current directory
	{
		fprintf(stdout, "Initializing the disk ...\n");
		create_dummy_disk();

	}
	else if (!clr)	// used existing DISK.
	{
		FD = fopen(DISK_NAME, "r+");
		if (!FD)	// file not exist
		{
			//create disk
			fprintf(stderr, "DISK doesn't found ...\nCreating a new disk ...\n");
			create_dummy_disk();

		}
		else	// check if the size of DISK is correct
		{
			// if file exist then check it size
			if (stat(DISK_NAME, &fileStat) != 0) 
				return -1;	//not able to retrieve stat info
			else
				totalBytes = (int) fileStat.st_size;
			
			if (totalBytes != disk_size)	
			{
				fprintf(stderr, "Error: DISK is corrupted. Please create new one ...\n");
				return -1;
			}
		}
	}

	return 0; // everything went alright :)
}

int disk_read(int number, char *buf)
{
	int cylinder = 0;
	int track = 0;
	int sector = 0;
	int seeks = 0; // helps to calculate total seek time

	//Logical block to Physical location on HDD triplet(cylinder, track, sector) 
	N_to_CTS(number, &cylinder, &track, &sector);
	//printf("Log: Cylinder: %d Track: %d Sector: %d\n", cylinder, track, sector);

	// calculate seek time 
	seeks = cylinder - current_cylinder;
	if (seeks < 0) seeks = -seeks;
	n_seeks += seeks;
	current_cylinder = cylinder;

	// Since file has to access using sequential access. 
	//In real world, HDD read in semi-random access using CTS conversion.
	if ( fseek(FD, (long) number * SECTOR_SIZE, SEEK_SET) != 0)
	{
		fprintf(stderr, "Error: Not able to access %d sector\n", number);
		return -1;
	} 
	
	// read 256 bytes block and increment transfer on block read
	if (fread(buf, 1, SECTOR_SIZE, FD) == SECTOR_SIZE)
	{
		n_transfers ++;
	}
	else
	{
		fprintf(stderr, "Error: Not able to retrieve %d sector\n", number);
		return -1;
	}

	return 0;
}

int disk_write(int number, char *buf)
{
	int cylinder = 0;
	int track = 0;
	int sector = 0;
	int seeks = 0; // helps to calculate total seek time

	//Logical block to Physical location on HDD triplet(cylinder, track, sector) 
	N_to_CTS(number, &cylinder, &track, &sector);
	//printf("Log: Cylinder: %d Track: %d Sector: %d\n", cylinder, track, sector);

	// calculate seek time 
	seeks = cylinder - current_cylinder;
	if (seeks < 0) seeks = -seeks;
	n_seeks += seeks;
	current_cylinder = cylinder;

	// Since file has to access using sequential access. 
	//In real world, HDD read in semi-random access using CTS conversion.
	if ( fseek(FD, (long) number * SECTOR_SIZE, SEEK_SET) != 0)
	{
		fprintf(stderr, "Error: Not able to access %d sector\n", number);
		return -1;
	} 

	// read 256 bytes block and increment transfer on block read
	if (fwrite(buf, 1, SECTOR_SIZE, FD) == SECTOR_SIZE)
	{
		n_transfers++;
	}
	else
	{
		fprintf(stderr, "Error: Not able to retrieve %d sector\n", number);
		return -1;
	}

	//flush the buffer to make sure it write data to the DISK and doesn't keep in the memory
	fflush(FD);


	return 0;
}

void disk_stats(int *ns , int *nt)
{
	*ns = n_seeks;
	*nt = n_transfers;
}


// int main()
// {
// 	int status;
// 	char writeData[SECTOR_SIZE] = "Hi 12345 Rahul!!!";
// 	char readData[SECTOR_SIZE];
// 	int ns = 0;
// 	int nt = 0;

// 	status = initialize_disk(1);

// 	disk_write(10, writeData);
// 	disk_read(10, readData);
// 	printf("read data = %s", readData);
// 	disk_stats(&ns, &nt);
// 	printf("ns: %d nt: %d", ns , nt);

// 	return 0;
// }


