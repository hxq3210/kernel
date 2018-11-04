#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/epoll.h>

#define CNT 200

#define SZ 512
/*
 	同一时刻监听200个打开的设备文件，如果有设备可读则读，否则阻塞。
 */
int main(int argc, char **argv)
{
	int fd;
	int epfd;
	int cnt;
	int i;
	int ret;
	char buf[SZ];
	struct epoll_event ev;
	struct epoll_event fdinfo[CNT];

	epfd = epoll_create(CNT);
	assert(epfd > 0);

	for (i = 0; i < CNT; i++) {
		sprintf(buf, "/dev/kbuf%d", i);
		fd = open(buf, O_RDONLY);
		assert(fd > 0);

		ev.events  = EPOLLIN;
		ev.data.fd = fd;

		epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
	}

	while (1) {
		ret = epoll_wait(epfd, fdinfo, CNT, 500);
		if (ret == 0) {
			printf("timeout...\n");
			continue;
		} else if (ret < 0) {
			perror("epoll");
			exit(1);
		}

		cnt = ret;
		for (i = 0; i < cnt; i++) {
			if (fdinfo[i].events & EPOLLIN) {
				ret = read(fdinfo[i].data.fd, buf, SZ-1);
				assert(ret > 0);
				buf[ret] = '\0';
				printf("%s", buf);
			}
		}

	}

	return 0;
}
