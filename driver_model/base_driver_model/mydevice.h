#ifndef MILLET_MYDEVICE_H_
#define MILLET_MYDEVICE_H_

#include <linux/device.h>

struct mydevice {
	struct device dev;
	char *name;
};

#endif
