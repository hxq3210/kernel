#ifndef MILLET_BEEP_IOCTL_H_
#define MILLET_BEEP_IOCTL_H_

#include <linux/ioctl.h>

#define BEEPTYPE 'b'

#define BEEP_SET_HZ  _IOW(BEEPTYPE, 0, int)
#define BEEP_STOP    _IO(BEEPTYPE, 1)

#endif
