#include "user_buttons.h"

extern volatile uint32_t PIN_IN;
#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

static volatile os_timer_t debounce_timer;

void ICACHE_FLASH_ATTR
buttons_debounce(uint8 direction)
{
    gpio_pin_intr_state_set(GPIO_ID_PIN(12), GPIO_PIN_INTR_ANYEGDE);

    if (direction == 1)
    {
        os_printf("Up");
        display_next_page();
    }
    else
    {
        os_printf("Down");
        display_prev_page();
    }
}


LOCAL void
buttons_intr_handler(int8_t key)
{
    uint8 direction = 0;
    uint32 inputs;
    
    os_delay_us(1);
    inputs = PIN_IN;

    //Read rotary state
    if (CHECK_BIT(inputs,13) == CHECK_BIT(inputs,12))
        direction = 1;

    //Not that sure what this does yet and where the register is used for
    uint32 gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);

    //disable interrupt
    gpio_pin_intr_state_set(GPIO_ID_PIN(12), GPIO_PIN_INTR_DISABLE);

    //clear interrupt status
    GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status & BIT(12));

    //Disarm timer
    os_timer_disarm(&debounce_timer);

    //Setup timer
    os_timer_setfn(&debounce_timer, (os_timer_func_t *)buttons_debounce, direction);

    //Arm the debounce timer
    os_timer_arm(&debounce_timer, 500, 0);
}

void ICACHE_FLASH_ATTR
buttons_init()
{
    //Attach the interrupt thing
    ETS_GPIO_INTR_ATTACH(buttons_intr_handler,12);

    //Disable interrupts
    ETS_GPIO_INTR_DISABLE();

    //Set GPIO 0 to IO
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_GPIO12);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_GPIO13);

    //Set the GPIO to input
    gpio_output_set(0, 0, 0, GPIO_ID_PIN(12));
    gpio_output_set(0, 0, 0, GPIO_ID_PIN(13));

    //Not sure what this does
    gpio_register_set(GPIO_PIN_ADDR(12), GPIO_PIN_INT_TYPE_SET(GPIO_PIN_INTR_DISABLE)
          | GPIO_PIN_PAD_DRIVER_SET(GPIO_PAD_DRIVER_DISABLE)
          | GPIO_PIN_SOURCE_SET(GPIO_AS_PIN_SOURCE));

    gpio_register_set(GPIO_PIN_ADDR(13), GPIO_PIN_INT_TYPE_SET(GPIO_PIN_INTR_DISABLE)
          | GPIO_PIN_PAD_DRIVER_SET(GPIO_PAD_DRIVER_DISABLE)
          | GPIO_PIN_SOURCE_SET(GPIO_AS_PIN_SOURCE));

    //clear gpio status
    GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, BIT(12));
    GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, BIT(13));

    //re-enable gpio0 interrupt
    gpio_pin_intr_state_set(GPIO_ID_PIN(12), GPIO_PIN_INTR_ANYEGDE);

    //Global re-enable interrupts
    ETS_GPIO_INTR_ENABLE();

}
