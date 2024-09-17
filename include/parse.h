#ifndef PARSE_H
#define PARSE_H

#define HEADER_MAGIC 0x4c4c4144

struct db_header_t {
	unsigned int magic;
	unsigned short version;
	unsigned short count;
	unsigned int file_size;
};
struct employee_t {
	char name[256];
	char address[256];
	unsigned int hours;
};

int create_db_header(int fd, struct db_header_t **header_out);
int validate_db_header(int fd, struct db_header_t **header_out);
int read_employees(int fd, struct db_header_t *header, struct employee_t **employees_out);
int output_file(int fd, struct db_header_t *header);
void list_employees(struct db_header_t *header, struct employee_t *employees);
int add_employee(struct db_header_t *header, struct employee_t *employees, char *add_str);

#endif
