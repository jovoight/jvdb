#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#include "../include/common.h"
#include "../include/parse.h"

int create_db_header(int fd, struct db_header_t **header_out) {
	// Create a new header in the heap, error out if allocation fails
	struct db_header_t *header = calloc(1, sizeof(struct db_header_t));
	if (header == NULL) {
		printf("Calloc failed to create DB header.\n");
		return STATUS_ERROR;
	}
	// Initialize the header with basic values
	header -> version = 0x1;
	header -> count = 0;
	header -> magic = HEADER_MAGIC;
	header -> file_size = sizeof(struct db_header_t);
	// Assign the new header pointer to header_out
	*header_out = header;
	// return success
	return STATUS_SUCCESS;
}

int validate_db_header(int fd, struct db_header_t **header_out) {
	// Error out if bad FD passed to the function
	if (fd < 0) {
		printf("Bad FD from client.\n");
		return STATUS_ERROR;
	}
	// Create an empty header on the heap, error out if allocation fails
	struct db_header_t *header = calloc(1, sizeof(struct db_header_t));
	if (header == NULL) {
		printf("Calloc failed to create DB header.\n");
		return STATUS_ERROR;
	}
	// Read the header from the DB file on disc into the empty one on the heap, error out if process fails
	if (read(fd, header, sizeof(struct db_header_t)) != sizeof(struct db_header_t)) {
		perror("read");
		free(header);
		return STATUS_ERROR;
	}
	// Ensure host endianness of all data in the header
	header -> version = ntohs(header -> version);
	header -> count = ntohs(header -> count);
	header -> magic = ntohl(header -> magic);
	header -> file_size = ntohl(header -> file_size);
	// Perform data validation
	if (header -> magic != HEADER_MAGIC) {
		printf("Unparsable file.\n");
		free(header);
		return STATUS_ERROR;
	}
	if (header -> version != 1) {
		printf("This utility is for DB version 1 only.\n");
		free(header);
		return STATUS_ERROR;
	}
	struct stat db_stats = {0};
	fstat(fd, &db_stats);
	if (header -> file_size != db_stats.st_size) {
		printf("Database is corrupted.\n");
		free(header);
		return STATUS_ERROR;
	}
	// Assign the validated header pointer to header_out
	*header_out = header;
	// If all validation passes, return success
	return STATUS_SUCCESS;
}

int output_file(int fd, struct db_header_t *header) {
	// Error out if bad FD passed to the function
	if (fd < 0) {
		printf("Bad FD from client.\n");
		return STATUS_ERROR;
	}
	// Ensure network endianness of all data in the header
	header -> version = htons(header -> version);
	header -> count = htons(header -> version);
	header -> magic = htonl(header -> magic);
	header -> file_size = htonl(header -> file_size);
	// Return the cursor to the start of the file
	lseek(fd, 0, SEEK_SET);
	// Write the updated data to the file
	write(fd, header, sizeof(struct db_header_t));
	// Free allocated memory
	free(header);
	// Return success
	return STATUS_SUCCESS;
}
