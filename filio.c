#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <fcntl.h>

struct database_header_t {
	unsigned short version;
	unsigned short employees;
	unsigned int filesize;
};

int main(int argc, char *argv[]) {
	struct database_header_t head = {0};
	struct stat db_stat = {0};

	if (argc != 2) {
		printf("Usage: %s <filename>\n", argv[0]);
		return 0;
	}

	int fd = open(argv[1], O_RDWR | O_CREAT, 0644);
	if (fd == -1) {
		perror("open");
		return -1;
	}

	if (read(fd, &head, sizeof(head)) != sizeof(head)) {
		perror("read");
		close(fd);
		return -1;
	}
	
	if (fstat(fd, &db_stat) != 0) {
		perror("stat");
		close(fd);
		return -1;
	}

	printf("DB File Length, Reported by Stat: %llu\n", db_stat.st_size);
	if (db_stat.st_size != head.filesize) {
		printf("Hack attempted: Abort program.");
		close(fd);
		return -2;
	}

	close(fd);
	return 0;
}
