#include "user_display.h"

volatile uint8 display_page = 1;
static volatile os_timer_t refresh_timer;

void ICACHE_FLASH_ATTR
display_refresh(void)
{
    //Refresh the display
    display_redraw();
}

void ICACHE_FLASH_ATTR 
display_draw_page(uint8 page)
{
    switch(page) {
        case 1:
            LCD_print("Page 1");
        break;
        case 2:
            LCD_print("Page 2");
        break;
        case 3:
            LCD_print("Page 3");
        break;
        case 4:
            LCD_print("Page 4");
        break;
        case 5:
            LCD_print("Page 5");
        break;
    }
}
void ICACHE_FLASH_ATTR 
display_redraw(void)
{
    char buffer[20];
    os_sprintf(buffer, "%i/%i",display_page,DISPLAY_PAGE_MAX);
    LCD_clear();
    LCD_setCursor(17,3);
    LCD_print(buffer);
    LCD_setCursor(0,0);
    display_draw_page(display_page);    
}

void ICACHE_FLASH_ATTR 
display_next_page(void)
{
    display_page++;
    if (display_page > DISPLAY_PAGE_MAX) display_page = 1;
    display_redraw();

    //Reset refresh timer
    os_timer_disarm(&refresh_timer);
    os_timer_setfn(&refresh_timer, (os_timer_func_t *)display_refresh, NULL);
    os_timer_arm(&refresh_timer, 5000, 0);
}

void ICACHE_FLASH_ATTR
display_init(void)
{
    i2c_init();
    while (!LCD_init())
    {
        os_printf("LCD not found\n\r");
    }
    LCD_setCursor(0,2);
    LCD_print("Home status Display");
    os_delay_us(5000000);
    display_redraw();

    os_timer_disarm(&refresh_timer);
    os_timer_setfn(&refresh_timer, (os_timer_func_t *)display_refresh, NULL);
    os_timer_arm(&refresh_timer, 5000, 0);
}

