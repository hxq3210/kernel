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
	int val;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s devicefile\n", argv[0]);
		exit(1);
	}

	fd = open(argv[1], O_RDONLY);
	assert(fd > 0);

	while (1) {
		ret = read(fd, &val, sizeof(int));

		if (ret < 0) {
			fprintf(stderr, "read error...\n");
		}

		printf("digital: %d, sample: %dmv\n", val, (val*1800)/4095);
		sleep(1);
	}

	close(fd);

	return 0;
}

