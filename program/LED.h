#ifndef _LED_H_
#define _LED_H_

#include "sys.h"  

#define LED_1  PBout(8)

#define LED_B  PBout(12)
#define LED_G  PBout(13)
#define LED_R  PBout(14)

void LED_init(void);

#endif
