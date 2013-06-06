/*
 ============================================================================
 Name        : minls.c
 Author      : Girum Ibssa
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "superblock.h"
#include "inode.h"
#include "util.h"

/**
 * Useful constants.
 */
#define PARTITION_TABLE_ADDRESS 0x1BE // location of the partition table
#define MINIX_PARTITION_TYPE 0x81 // partition type for Minix
#define BYTE_510 0x55 /* byte 510 of a boot sector with
						 a valid partition table */
#define BYTE_511 0xAA /* byte 511 of a boot sector with
						 a valid partition table */
#define MINIX_MAGIC_NUMBER 0x4D5A // the minix magic number
#define MINIX_MAGIC_NUMBER_REVERSED 0x5A4D /* minix magic number on
											  a byte-reversed �lesystem */
#define INODE_SIZE 64 // size of an inode in bytes
#define DIRECTORY_ENTRY_SIZE 64 // size of a directory entry in bytes
/*
 * End useful constants.
 */

int main(int argc, char *argv[]) {
	FILE *fp = NULL;
	int i = 0;
//	char *image_filename = NULL;
//	char *path = NULL;
	bool verbose = false, partition = false, subpartition = false;
//	int result = 0;
	Superblock superblock = { 0 };
	Inode firstInode = { 0 };

	for (i = 1; i < argc; i++) {

		// Flag processing. Check for a switch (leading "-").
		if (argv[i][0] == '-') {

			// Use the next character to decide what to do.
			switch (argv[i][1]) {
			case 'v':
				printf("Verbose!\n");
				verbose = true;
				break;
			case 'p':
				printf("Partition!\n");
				partition = true;

				// Check for Subpartition flag.
				if (i + 3 < argc && argv[i + 2][1] == 's') {
					printf("Subpartition!\n");
					subpartition = true;
				}

				break;
			}
		}
	}

	// Open the image file.
	if ((fp = fopen(argv[argc - 1], "r")) == NULL ) {
		perror("fopen");
		exit(-1);
	}

	// Seek to the superblock.
	if (fseek(fp, ONE_KILOBYTE, SEEK_SET)) {
		perror("fseek");
		exit(-1);
	}

	// Read the superblock.
	fread(&superblock, sizeof(Superblock), 1, fp);

	// Check for errors on the fread().
	if (ferror(fp)) {
		perror("file error");
		exit(-1);
	}

	// Print the superblock.
	print_superblock(&superblock);

	// Seek to the first inode in the system?
	if (fseek(fp, get_first_iblock(&superblock), SEEK_SET)) {
		perror("fseek");
		exit(-1);
	}

	// Read the first inode?
	fread(&firstInode, sizeof(Inode), 1, fp);

	// Check for errors on the fread().
	if (ferror(fp)) {
		perror("file error");
		exit(-1);
	}

	// Print out the first inode?
	print_inode(&firstInode);

	// Close the image file.
	if (fclose(fp) != 0) {
		perror("fclose");
		exit(-1);
	}
	return EXIT_SUCCESS;
}