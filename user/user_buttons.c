#include "user_buttons.h"

static volatile os_timer_t debounce_timer;

void ICACHE_FLASH_ATTR
buttons_debounce()
{
    gpio_pin_intr_state_set(GPIO_ID_PIN(12), GPIO_PIN_INTR_POSEDGE);
    display_next_page();
}


LOCAL void
buttons_intr_handler(int8_t key)
{
    //Not that sure what this does yet and where the register is used for
    uint32 gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);

    //disable interrupt
    gpio_pin_intr_state_set(GPIO_ID_PIN(12), GPIO_PIN_INTR_DISABLE);

    //clear interrupt status
    GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status & BIT(12));

    //Disarm timer
    os_timer_disarm(&debounce_timer);

    //Setup timer
    os_timer_setfn(&debounce_timer, (os_timer_func_t *)buttons_debounce, NULL);

    //Arm the debounce timer
    os_timer_arm(&debounce_timer, 500, 0);
}

void ICACHE_FLASH_ATTR
buttons_init()
{
    //Attach pin 0 to the interrupt thing
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

    //re-enable gpio0 interrupt
    gpio_pin_intr_state_set(GPIO_ID_PIN(12), GPIO_PIN_INTR_POSEDGE);

    //Global re-enable interrupts
    ETS_GPIO_INTR_ENABLE();

}
