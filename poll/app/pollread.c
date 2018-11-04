#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/poll.h>

#define CNT 200

#define SZ 512
/*
 	同一时刻监听200个打开的设备文件，如果有设备可读则读，否则阻塞。
 */
int main(int argc, char **argv)
{
	int fd;
	int cnt;
	int i;
	int ret;
	char buf[SZ];
	struct pollfd fdsets[CNT];

	for (i = 0; i < CNT; i++) {
		sprintf(buf, "/dev/kbuf%d", i);
		fd = open(buf, O_RDONLY);
		assert(fd > 0);
		fdsets[i].fd = fd;
		fdsets[i].events = POLLIN;
	}

	while (1) {
		ret = poll(fdsets, CNT, 3000);
		if (ret == 0) {
			printf("timeout ...\n");	
			continue;
		} else if (ret < 0) {
			perror("poll");
			exit(1);
		}

		cnt = ret;
		for (i = 0; i < CNT; i++) {
			if (fdsets[i].revents & POLLIN) {
				ret = read(fdsets[i].fd, buf, SZ-1);
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

