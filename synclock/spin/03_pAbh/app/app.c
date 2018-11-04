#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#define N 98 

#define SZ 512

/*
 	./a.out /dev/semaphore  r/w
 */
int main(int argc, char **argv)
{
	int fd;
	int ret;
	int i;
	pid_t pid[N];
	char buf[SZ];

	if (argc != 2) {
		fprintf(stderr, "Usage: %s devicefile\n", argv[0]);
		exit(1);
	}

	fd = open(argv[1], O_RDWR | O_NDELAY);
	assert(fd > 0);

	for (i = 0; i < N; i++) { 
		if ((pid[i] = fork()) < 0) {
			perror("fork");
			exit(1);
		}

		if (pid[i] == 0) {
			sprintf(buf, "hello, %d.", getpid());
			ret = ioctl(fd, 0, buf);
			assert(ret == 0);
			return;
		}
	}

	for (i = 0; i < N; i++) {
		waitpid(pid[i], NULL, 0);
	}

	close(fd);

	return 0;
}

