#ifndef __USER_DISPLAY_H__
#define __USER_DISPLAY_H__

#include "ets_sys.h"
#include "i2c/i2c.h"
#include "i2c_hd44780/i2c_hd44780.h"
#include "osapi.h"
#include "os_type.h"
#include "user_interface.h"
#include "config.h"
#include "user_display.h"

#define DISPLAY_PAGE_MAX 5

void display_redraw(void);
void display_next_page(void);
void display_init(void);

#endif
