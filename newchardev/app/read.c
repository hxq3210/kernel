#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SZ 512

int main(int argc, char **argv)
{
	int fd;
	int ret;
	char buf[SZ];
	int pid;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s devicefile\n", argv[0]);
		exit(1);
	}

	fd = open(argv[1], O_RDONLY | O_NDELAY);
	assert(fd > 0);

	ret = read(fd, buf, SZ-1);
	if (ret < 0) {
		perror("read");
		exit(1);
	}

	buf[ret] = '\0';

	printf("[app]: read %dBytes ok!\n", ret);
	printf("%s\n", buf);

	close(fd);

	return 0;
}

