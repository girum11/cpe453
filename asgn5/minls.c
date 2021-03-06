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
#include <string.h>
#include "superblock.h"
#include "file.h"
#include "partition.h"
#include "util.h"

static FILE *initalize(FILE *fp, Superblock *superblock,
		int *partition_sector_offset, char *filename, bool use_partition,
		bool use_subpartition, bool verbose) {
	PartitionTableEntry partition_table_entry = { 0 };

	// Open the image file.
	if ((fp = fopen(filename, "r")) == NULL ) {
		perror("fopen");
		exit(-1);
	}

	if (use_partition) {
		// Attempt to read the partition table, if it exists.
		read_partition_table(&partition_table_entry, fp);

		// If you do find a partition table, then offset the superblock to
		// point to its partitioned location.
		if (partition_table_entry.type == MINIX_PARTITION_TYPE) {
			// There's a partition table, so account for the superblock
			// offset for later.
			*partition_sector_offset = partition_table_entry.lFirst
					* SECTOR_SIZE;
		}
		// If you want to use partitioning but the partition table wasn't
		// found, then exit with failure.
		else {
			fprintf(stderr, "No partition table found\n");
			exit(-1);
		}
	} else {
		// If the user elected not to partition, then don't offset the
		// superblock.
		*partition_sector_offset = 0;
	}

	// Seek to the superblock, offsetting for the partitition if it's there.
	if (fseek(fp, *partition_sector_offset + ONE_KILOBYTE, SEEK_SET)) {
		perror("fseek");
		exit(-1);
	}

	// Read the superblock.
	fread(superblock, sizeof(Superblock), 1, fp);
	error_check_file_pointer(fp);

	// Sanity check on the superblock's magic number.
	if (superblock->magic != MINIX_MAGIC_NUMBER) {
		fprintf(stderr, "Bad magic number. (0x%x)\n", superblock->magic);
		fprintf(stderr, "This doesn't look like a MINIX filesystem.\n");
		exit(-1);
	}

	// If verbose, then print the superblock's data.
	if (verbose) {
		print_superblock(superblock);
	}

	return fp;
}

int main(int argc, char *argv[]) {
	FILE *fp = NULL;
	int i = 0;
	char *image_filename = NULL;
	char *path = NULL;
	bool verbose = false, use_partition = false, use_subpartition = false;
	int primary_partition = 0, subpartition = 0;
	Superblock superblock = { 0 };
	char *new_path = NULL;
	int partition_sector_offset = 0;
	int non_flag_arguments = argc - 1;

	for (i = 1; i < argc; i++) {

		// Flag processing. Check for a switch (leading "-").
		if (argv[i][0] == '-') {

			// Use the next character to decide what to do.
			switch (argv[i][1]) {
			case 'v':
				verbose = true;
				break;
			case 'p':
				use_partition = true;
				primary_partition = strtol(argv[i + 1], NULL, 10);
				non_flag_arguments -= 2;

				// Check for Subpartition flag.
				if ((argc > (i + 3)) && argv[i + 2][1] == 's') {
					use_subpartition = true;
					subpartition = strtol(argv[i + 3], NULL, 10);
					non_flag_arguments -= 2;
				}

				break;
			}
		}
	}

//	/* If there are three or more arguments and both the second to last
//	 * and last arguments aren't flags, then the user specified a path. */
//	if (argc >= 3 && argv[argc - 2][0] != '-' && argv[argc - 1][0] != '-') {
//		image_filename = argv[argc - 2];
//		path = argv[argc - 1];
//
//		/* "Paths that do not include a leading �/� are processed
//		 * relative to the root directory" */
//		if (path[0] != '/') {
//			new_path = malloc(sizeof(char) * strlen(path) + 2);
//			strcpy(new_path, "/");
//			strcat(new_path, path);
//			path = new_path;
//		}
//	}
//	// Otherwise, the user didn't specify a path after all.
//	else {
//		image_filename = argv[argc - 1];
//		path = "/";
//	}

	if (non_flag_arguments <= 0) {
		print_minls_usage();
		exit(-1);
	} else if (non_flag_arguments == 1) {
		image_filename = argv[argc - 1];
		path = "/";
	} else if (non_flag_arguments == 2) {
		image_filename = argv[argc - 2];
		path = argv[argc - 1];

		/* "Paths that do not include a leading �/� are processed
		 * relative to the root directory" */
		if (path[0] != '/') {
			new_path = malloc(sizeof(char) * strlen(path) + 2);
			strcpy(new_path, "/");
			strcat(new_path, path);
			path = new_path;
		}
	}

	printf("Opening file %s with path: %s\n", image_filename, path);

	// Open and initalize the image. Initialize the superblock.
	fp = initalize(fp, &superblock, &partition_sector_offset, image_filename,
			use_partition, use_subpartition, verbose);

	// Find and print the correct file/directory.
	find_file(fp, &superblock, partition_sector_offset, path, "/", 1, verbose);

	// If you had to malloc a new_path, then free it.
	if (new_path != NULL ) {
		free(new_path);
	}

	// Close the image.
	if (fclose(fp) != 0) {
		perror("fclose");
		exit(-1);
	}
	return EXIT_SUCCESS;
}
