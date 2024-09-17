#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>

#include "../include/common.h"
#include "../include/file.h"
#include "../include/parse.h"

void print_usage(char *argv[]) {
	printf("Usage: %s -n -f <file_path>\n", argv[0]);
	printf("\t-n\tCreate new database file\n");
	printf("\t-f\t(REQUIRED) Path to database file\n");
	return;
}

int main(int argc, char *argv[]) {

	int c;
	bool new_file = false;
	char *file_path = NULL;
	int db_fd = -1;

	while ((c = getopt(argc, argv, "nf:")) != -1) {
		switch (c) {
			case 'n':
				new_file = true;
				break;
			case 'f':
				file_path = optarg;
				break;
			case '?':
				printf("Unknown option -%c\n", c);
				break;
			default:
				return -1;
		}
	}

	if (file_path == NULL) {
		printf("File path is a required argument.\n");
		print_usage(argv);
		return 0;
	}

	if (new_file) {
		db_fd = create_db_file(file_path);
		if (db_fd == STATUS_ERROR) {
			printf("Unable to create database file.\n");
			return -1;
		}
	} else {
		db_fd = open_db_file(file_path);
		if (db_fd == STATUS_ERROR) {
			printf("Unable to open database file.\n");
			return -1;
		}
	}

	printf("New File: %d\n", new_file);
	printf("File Path: %s\n", file_path);

	return 0;
}
