#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#define N 4

/*
 	./a.out  /dev/keys
 */
int main(int argc, char **argv)
{
	int fd;
	int ret;
	char curr[N] = { 0 };
	char last[N] = { 0 };
	int i;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s devicefile\n", argv[0]);
		exit(1);
	}

	fd = open(argv[1], O_RDONLY);
	assert(fd > 0);

	for (;;) { 
		ret = read(fd, curr, N); //////??????????????
		if (ret != N) {
			perror("read");
			exit(1);
		}
		for (i = 0; i < N; i++) {
			if (curr[i] != last[i]) {
				printf("key %d is %s.\n",
						i+1, curr[i]?"down":"up");
				last[i] = curr[i];
			}	
		}
	}

	close(fd);

	return 0;
}

