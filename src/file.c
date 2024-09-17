#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../include/file.h"
#include "../include/common.h"

int create_db_file(char *file_path) {
	// Check if the file already exists
	int fd = open(file_path, O_RDONLY);
	if (fd != -1) {
		close(fd);
		printf("File already exists.\n");
		return STATUS_ERROR;
	}
	// If not, create it
	fd = open(file_path, O_RDWR | O_CREAT, 0644);
	if (fd == -1) {
		perror("open");
		return STATUS_ERROR;
	}

	return fd;
}

int open_db_file(char *file_path) {
	// Open the file
	int fd = open(file_path, O_RDWR, 0644);
	if (fd == -1) {
		perror("open");
		return STATUS_ERROR;
	}

	return fd;
}
