#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>

#include "../include/common.h"
#include "../include/file.h"
#include "../include/parse.h"
// Show how to use the utility if commands are passed incorrectly
void print_usage(char *argv[]) {
	printf("Usage: %s -n -f <file_path> -a <name,address,hours> -l -r <name> -u <name,hours> -h\n", argv[0]);
	printf("\t-n\tCreate new database file\n");
	printf("\t-f\t(REQUIRED) Path to database file\n");
	printf("\t-a\tAdd a new employee to the database\n");
	printf("\t-l\tList all employees currently stored in the database\n");
	printf("\t-r\tRemove an employee from the database by name\n");
	printf("\t-u\tUpdate an employee's hours by name\n");
	printf("\t-h\tHelp\n");
	return;
}

int main(int argc, char *argv[]) {
	// Initialize all needed variables
	int command;
	bool new_file = false;
	char *file_path = NULL;
	int db_fd = STATUS_ERROR;
	struct db_header_t *header = NULL;
	struct employee_t *employees = NULL;
	char *add_str = NULL;
	bool list = false;
	char *delete_name = NULL;
	char *name_and_hours = NULL;
	// Handle all applicable flags
	while ((command = getopt(argc, argv, "nf:a:lr:u:h")) != -1) {
		switch (command) {
			case 'n':
				new_file = true;
				break;
			case 'f':
				file_path = optarg;
				break;
			case 'a':
				add_str = optarg;
				break;
			case 'l':
				list = true;
				break;
			case 'r':
				delete_name = optarg;
				break;
			case 'u':
				name_and_hours = optarg;
				break;
			case 'h':
				print_usage(argv);
				break;
			case '?':
				printf("Unknown option -%c\n", command);
				print_usage(argv);
				return STATUS_ERROR;
			default:
				return STATUS_ERROR;
		}
	}
	// Ensure file path is passed
	if (file_path == NULL) {
		printf("File path is a required argument.\n");
		print_usage(argv);
		return STATUS_SUCCESS;
	}
	// Handle new file and header creation
	if (new_file) {
		db_fd = create_db_file(file_path);
		if (db_fd == STATUS_ERROR) {
			printf("Unable to create database file.\n");
			return STATUS_ERROR;
		}
		if (create_db_header(db_fd, &header) == STATUS_ERROR) {
			printf("Failed to create database header.\n");
			return STATUS_ERROR;
		}
	} else {
	// Handle usage of existing DB file
		db_fd = open_db_file(file_path);
		if (db_fd == STATUS_ERROR) {
			printf("Unable to open database file.\n");
			return STATUS_ERROR;
		}

		if (validate_db_header(db_fd, &header) == STATUS_ERROR) {
			printf("Database header validation failed.\n");
			return STATUS_ERROR;
		}
	}
	// Read the employees from the disk into memory
	if (read_employees(db_fd, header, &employees) == STATUS_ERROR) {
		printf("Failed to read employees.\n");
		return STATUS_ERROR;
	}
	// Handle adding an employee (if selected)
	if (add_str) {
		header -> count++;
		employees = realloc(employees, header -> count * sizeof(struct employee_t));
		add_employee(header, employees, add_str);
	}
	// Handle deleting an employee by name (if selected)
	if (delete_name) {
		if (delete_employee_by_name(header, employees, delete_name) == STATUS_ERROR) {
			printf("Failed to delete employee %s\n", delete_name);
			return STATUS_ERROR;
		}
		header -> count--;
	}
	// Handle updating employee hours by name (if selected)
	if (name_and_hours) {
		if (update_employee_hours_by_name(header, employees, name_and_hours) == STATUS_ERROR) {
			printf("Failed to update employee %s\n", strtok(name_and_hours, ","));
		}
	}
	// Handle listing employees (if selected)
	if (list) { list_employees(header, employees); }
	// Print output
	printf("New File: %d\n", new_file);
	printf("File Path: %s\n", file_path);
	//Write the updated data back to the disk
	output_file(db_fd, header, employees);
	// Return success
	return STATUS_SUCCESS;
}
