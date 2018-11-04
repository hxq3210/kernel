#ifndef MILLET_LED_H_
#define MILLET_LED_H_

#include <linux/ioctl.h>

#define LEDTYPE 'L'

#define LEDON 	_IOW(LEDTYPE, 0, int)
#define LEDOFF 	_IOW(LEDTYPE, 1, int)
#define LEDSTAT	_IOR(LEDTYPE, 2, int)

#endif
