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

	if (argc != 3) {
		fprintf(stderr, "Usage: %s devicefile r/w\n", argv[0]);
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
			if (!strncmp(argv[2], "r", 1)) {
				ret = read(fd, buf, SZ);
				assert(ret == SZ);
				printf("ret = %d\n", ret);
				printf("[app%d]: read %dBytes ok!\n", 
						getpid(), ret);
				close(fd);
				return;	
			} else if (!strncmp(argv[2], "w", 1)) {
				ret = write(fd, buf, SZ);
				assert(ret == SZ);
				printf("[app%d]: write %dBytes ok!\n", 
						getpid(), ret);
				close(fd);
				return;	
			}			
		}
	}

	for (i = 0; i < N; i++) {
		waitpid(pid[i], NULL, 0);
	}

	close(fd);

	return 0;
}

