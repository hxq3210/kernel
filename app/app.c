#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SZ 512

int myfork(void)
{
	int ret;

	__asm__ __volatile__ (
		"svc #(0x900000+2)\n"		
		"mov %[ret], r0\n"

		:[ret]"=&r"(ret)
	);

	return ret;
}

ssize_t myread(int fd, void *buf, size_t count)
{
	ssize_t ret;

	__asm__ __volatile__ (
	
		"mov r0,%[fd]\n"	
		"mov r1,%[buf]\n"	
		"mov r2,%[count]\n"

		"svc #(0x900000 + 3)\n"	

		"mov %[ret], r0\n"

		:[ret]"=&r"(ret)
		:[fd]"r"(fd),[buf]"r"(buf),[count]"r"(count)
		:"r0","r1","r2"
	);

	return ret;
}


int main(int argc, char **argv)
{
	int fd;
	int ret;
	char buf[SZ];
	int pid;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s file.\n", argv[0]);
		exit(1);
	}

	if ((pid = myfork()) < 0) {
		perror("fork");
		exit(1);
	} else if (pid == 0) {
		fd = open(argv[1], O_RDONLY | O_NDELAY);
		assert(fd > 0);

		while ((ret = myread(fd, buf, SZ-1)) > 0) {
			buf[ret] = '\0';
			printf("%s", buf);
		}

		if (ret < 0) {
			perror("read");
			exit(1);
		}

		close(fd);

		exit(0);
	}

	waitpid(pid, NULL, 0);

	return 0;
}

