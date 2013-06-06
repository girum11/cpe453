/*
 * superblock.c
 *
 *  Created on: Jun 6, 2013
 *      Author: girum
 */

#include "superblock.h"
#include <stdio.h>
#include "util.h"

void print_superblock(Superblock *superblock) {
	printf("Superblock Contents:\n");
	printf("Stored fields:\n");
	printf("  ninodes: %d\n", superblock->ninodes);
	printf("  i_blocks: %d\n", superblock->i_blocks);
	printf("  z_blocks: %d\n", superblock->z_blocks);
	printf("  firstdata: %d\n", superblock->firstdata);
	printf("  log_zone_zine: %d\n", superblock->log_zone_size);
	printf("  max_file: %d\n", superblock->max_file);
	printf("  magic: 0x%x\n", superblock->magic);
	printf("  zones: %d\n", superblock->zones);
	printf("  blocksize: %d\n", superblock->blocksize);
	printf("  subversion: %d\n", superblock->subversion);
	printf("Computed fields:\n");
	printf("  firstIblock: UNKNOWN\n");
	printf("  zonesize: %d\n", get_zonesize(superblock));
	printf("  ptrs_per_zone: UNKNOWN\n");
	printf("  ino_per_block: UNKNOWN\n");
	printf("  wrongended: UNKNOWN\n\n");
}

int get_first_iblock(Superblock *superblock) {
	return 0;
}

int get_zonesize(Superblock *superblock) {
	return superblock->blocksize << superblock->log_zone_size;
}

int get_ptrs_per_zone(Superblock *superblock) {
	return 0;
}

int get_ino_per_block(Superblock *superblock) {
	return 0;
}

int get_wrongended(Superblock *superblock) {
	return 0;
}

int get_inode_index_for_number(Superblock *superblock, int inode_number) {
	const int inode_index = inode_number - 1;

	return (2 + superblock->i_blocks + superblock->z_blocks + inode_index)
			* superblock->blocksize;
}
