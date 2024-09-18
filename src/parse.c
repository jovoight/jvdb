#include <stdbool.h>
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
	if (header == (void *)-1) {
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
	if (header == (void *)-1) {
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

int output_file(int fd, struct db_header_t *header, struct employee_t *employees) {
	// Error out if bad FD passed to the function
	if (fd < 0) {
		printf("Bad FD from client.\n");
		return STATUS_ERROR;
	}
	// Store the employee count before packing the header
	int count = header -> count;
	// Calculate file_size ahead of time to use for truncation
	header -> file_size = sizeof(struct db_header_t) + count * sizeof(struct employee_t);
	// Truncate the file to the file_size stored in the header
	if (ftruncate(fd, header -> file_size) == STATUS_ERROR) {
		printf("Failed to truncate DB file.\n");
		return STATUS_ERROR;
	}
	// Ensure network endianness of all data in the header
	header -> version = htons(header -> version);
	header -> count = htons(header -> count);
	header -> magic = htonl(header -> magic);
	header -> file_size = htonl(header -> file_size);
	// Return the cursor to the start of the file
	lseek(fd, 0, SEEK_SET);
	// Write the updated header to the file
	write(fd, header, sizeof(struct db_header_t));
	// Iterate through the employees
	for (int i = 0; i < count; i++) {
		// Ensure the employee's network endianness
		employees[i].hours = htonl(employees[i].hours);
		// Write the employee to the disk
		write(fd, &employees[i], sizeof(struct employee_t));
	}
	// Free allocated memory
	free(header);
	free(employees);
	// Return success
	return STATUS_SUCCESS;
}

int read_employees(int fd, struct db_header_t *header, struct employee_t **employees_out) {
	// Error out if bad FD passed to the function
	if (fd < 0) {
		printf("Bad FD from client.\n");
		return STATUS_ERROR;
	}
	// Get the number of employees from the DB header
	int count = header -> count;
	// Create a buffer to store the employees, error out if allocation fails
	struct employee_t *employees = calloc(count, sizeof(struct employee_t));
	if (employees == (void *)-1) {
		printf("Calloc failed to create employee buffer.\n");
		return STATUS_ERROR;
	}
	// Read the employees from the disk into the buffer in memory
	read(fd, employees, count * sizeof(struct employee_t));
	// Iterate through the employees and convert to host endianness
	for (int i = 0; i < count; i++) {
		employees[i].hours = ntohl(employees[i].hours);
	}
	// Pass the populated employees to employees_out
	*employees_out = employees;
	// Return success
	return STATUS_SUCCESS;
}

int add_employee(struct db_header_t *header, struct employee_t *employees, char *add_str) {
	// parse the string into its individual parts
	char *name = strtok(add_str, ",");
	char *address = strtok(NULL, ",");
	char *hours = strtok(NULL, ",");
	// 
	strncpy(employees[header -> count - 1].name, name, sizeof(employees[header -> count - 1].name));
	strncpy(employees[header -> count - 1].address, address, sizeof(employees[header -> count - 1].address));
	employees[header -> count - 1].hours = atoi(hours);
	// Return success
	return STATUS_SUCCESS;
}

void list_employees(struct db_header_t *header, struct employee_t *employees) {
	// Iterate through the employees
	for (int i = 0; i < header -> count; i++) {
		printf("Employee %d:\n", i + 1);
		printf("\tName: %s\n", employees[i].name);
		printf("\tAddress: %s\n", employees[i].address);
		printf("\tHours: %d\n", employees[i].hours);
	}
	return;
}

int delete_employee_by_name(struct db_header_t *header, struct employee_t *employees, char *name) {
	// Create a boolean to see if the employee was deleted
	bool deleted = false;
	// Iterate through the employees
	for (int i = 0; i < header -> count; i++) {
		// Check if the current employee should be deleted
		if (strcmp(employees[i].name, name) == 0) {
			// If so, shift all remaining employees back
			for (int j = i; j < header -> count; j++) {
				employees[j] = employees[j + 1];
			}
			// Update the deleted bool
			deleted = true;
			// Break from the loop
			break;
		}
	}
	// Check if the element to delete was found
	if (!deleted) {
		printf("Employee %s not found.\n", name);
		return STATUS_ERROR;
	}
	return STATUS_SUCCESS;
}

int update_employee_hours_by_name(struct db_header_t *header, struct employee_t *employees, char *name_and_hours) {
	// Split the input string into name and hours
	char *name = strtok(name_and_hours, ",");
	char *hours = strtok(NULL, ",");
	// Initialize a boolean representing whether the employee was successfully updated
	bool updated = false;
	// Iterate through the employees
	for (int i = 0; i < header -> count; i++) {
		// Check if the current employee should be updated
		if (strcmp(employees[i].name, name) == 0) {
			// If so, update the hours and updated boolean and break from the loop
			sscanf(hours, "%d", &employees[i].hours);
			updated = true;
			break;
		}
	}
	// Check if the employee to update was found
	if (!updated) {
		printf("Employee %s not found.\n", name);
		return STATUS_ERROR;
	}
	return STATUS_SUCCESS;
}
