#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/epoll.h>
#include <linux/input.h>

#define CNT 3

#define SZ 512

void input_event_read(int fd)
{
	int ret;
	struct input_event ev;

	ret = read(fd, &ev, sizeof(ev));
	assert(ret == sizeof(ev));

	switch (ev.type) {
		case EV_SYN:
			if (ev.code == SYN_REPORT) {
				printf("the time data is acquied end (SYN_REPORT)\n");
			} else if (ev.code == SYN_MT_REPORT) {
				printf("the data is acquied end , maybe...(SYN_MT_REPORT)\n");
			}
			break;
		case EV_KEY:
			printf("key code %d is %s.\n", ev.code, ev.value?"down":"up");
			break;
		case EV_REL:
			if (ev.code == REL_X) {
				printf("x = %d,", ev.value);
			} else if (ev.code == REL_Y) {
				printf("y = %d\n", ev.value);
			}
			break;
		case EV_ABS:
			if ((ev.code == ABS_X) || (ev.code == ABS_MT_POSITION_X)) {
				printf("x = %d,", ev.value);
			} else if ((ev.code == ABS_Y) || (ev.code == ABS_MT_POSITION_Y)) {
				printf("y = %d\n", ev.value);
			} else if ((ev.code == ABS_PRESSURE) || (ev.code == ABS_MT_PRESSURE)) {
				printf("%s ts...\n", ev.value?"down":"up");
			}
			break;
		default:
			break;
	}
}

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
		sprintf(buf, "/dev/input/event%d", i);
		fd = open(buf, O_RDONLY);
		assert(fd > 0);

		ev.events  = EPOLLIN;
		ev.data.fd = fd;

		epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
	}

	while (1) {
		ret = epoll_wait(epfd, fdinfo, CNT, -1);
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
				input_event_read(fdinfo[i].data.fd);
			}
		}

	}

	return 0;
}
