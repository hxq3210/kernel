#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <sys/time.h>

#define CNT 200

#define SZ 512
/*
 	同一时刻监听200个打开的设备文件，如果有设备可读则读，否则阻塞。
 */
int main(int argc, char **argv)
{
	int fd;
	int i;
	int maxfd = 0;
	int ret;
	char buf[SZ];
	int cnt;
	fd_set readsets, storsets;
	struct timeval t = {.tv_sec = 5, .tv_usec = 0}, tt;

	FD_ZERO(&readsets);
	for (i = 0; i < CNT; i++) {
		sprintf(buf, "/dev/kbuf%d", i);
		fd = open(buf, O_RDONLY);
		assert(fd > 0);
		FD_SET(fd, &readsets);
		maxfd = maxfd < fd? fd : maxfd;
	}

	storsets = readsets;
	tt = t;
	while (1) {
		readsets = storsets;
		t = tt;
		ret = select(maxfd+1, &readsets, NULL, NULL, &t);
		if (ret == 0) {
			printf("timeout ...\n");
			continue;
		} else if (ret < 0) {
			perror("select");
			exit(1);
		}

		cnt = ret;
		for (i = 0; i < maxfd+1; i++) {
			if (FD_ISSET(i, &readsets)) {
				ret = read(i, buf, SZ-1);
				assert(ret > 0);
				buf[ret] = '\0';
				printf("%s", buf);
				cnt--;
				if (cnt == 0) {
					break;
				}
			}
		}

	}
	return 0;
}

