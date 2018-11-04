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
	int fdev, fdsrc;
	int ret;
	char buf[SZ];

	if (argc != 3) {
		fprintf(stderr, "Usage: %s devicefile  srcfile\n", argv[0]);
		exit(1);
	}

	fdev = open(argv[1], O_WRONLY);
	assert(fdev > 0);

	fdsrc = open(argv[2], O_RDONLY | O_NDELAY);
	assert(fdsrc > 0);

	ret = read(fdsrc, buf, SZ);
	if (ret < 0) {
		perror("read");
		exit(1);
	}

	ret = write(fdev, buf, ret);
	if (ret < 0) {
		perror("read");
		exit(1);
	}
	printf("[app]: write %dBytes ok!\n", ret);

	return 0;
}

