#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>

// #include "common.h"
// #include "file.h"
// #include "parse.h"

void print_usage(char *argv[]) {
	printf("Usage: %s -n -f <file_path>\n", argv[0]);
	printf("\t-n\tCreate new database file\n");
	printf("\t-f\t(REQUIRED) Path to database file\n");
	return;
}

int main(int argc, char *argv[]) {

	int c;
	bool newfile = false;
	char *filepath = NULL;
	while ((c = getopt(argc, argv, "nf:")) != -1) {
		switch (c) {
			case 'n':
				newfile = true;
				break;
			case 'f':
				filepath = optarg;
				break;
			case '?':
				printf("Unknown option -%c\n", c);
				break;
			default:
				return -1;
		}
	}

	if (filepath == NULL) {
		printf("File path is a required argument.\n");
		print_usage(argv);
		return 0;
	}

	printf("New File: %d\n", newfile);
	printf("File Path: %s\n", filepath);

	return 0;
}
