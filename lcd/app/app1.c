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
#include <linux/fb.h>

int main(int argc, char **argv)
{
	int fd;
	int ret;
	struct fb_var_screeninfo arg;

	fd = open("/dev/fb0", O_RDWR);
	assert(fd > 0);

	ret = ioctl(fd, FBIOGET_VSCREENINFO, &arg);
	assert(ret == 0);

	printf("xres: %d, yres: %d.\n", arg.xres, arg.yres);
	printf("bits_per_pixel: %d.\n", arg.bits_per_pixel);
	printf("red: (%d, %d), green: (%d, %d), blue: (%d, %d)\n",
			arg.red.offset, arg.red.length,
			arg.green.offset, arg.green.length,
			arg.blue.offset, arg.blue.length);

	return 0;
}
