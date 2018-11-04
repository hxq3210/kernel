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
#include <sys/mman.h>


int xres, yres, sz;
char *fb; //指向内核空间对应窗口显示图片的缓存地址所映射到的用户空间地址

void draw_point(int x, int y, int col)
{
	*((int *)fb + y*xres + x) = col;
}

int main(int argc, char **argv)
{
	int fd;
	int i,j;
	int ret;
	struct fb_var_screeninfo arg;

	fd = open("/dev/fb0", O_RDWR);
	assert(fd > 0);

	ret = ioctl(fd, FBIOGET_VSCREENINFO, &arg);
	assert(ret == 0);

	xres = arg.xres;
	yres = arg.yres;
	sz = xres*yres*arg.bits_per_pixel >> 3; 

	fb = mmap(NULL, sz,  PROT_READ|PROT_WRITE,  MAP_SHARED, fd, 0);
	if (fb == MAP_FAILED) {
		perror("mmap");
		exit(1);
	}

	for (i = 0; i < yres; i++) {
		for (j = 0; j < xres; j++) {
			if ((i < yres/2) && (j < xres/2)) {
				draw_point(j, i, 0xff0000);
			} else if ((i < (yres/2)) && (j >= xres/2)) {
				draw_point(j, i, 0xff00);
			} else if ((i >= yres/2) && (j < xres/2)) {
				draw_point(j, i, 0xff);
			} else {
				draw_point(j, i, 0x9527ff);
			}
		}
	}

	getchar();

	munmap(fb, sz);

	close(fd);

	return 0;
}
