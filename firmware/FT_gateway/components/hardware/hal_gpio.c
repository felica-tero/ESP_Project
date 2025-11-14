#include "hal_gpio.h"

static void hal_gpio_setupOutput(dig_out_info_t * dig_out_info);
static void hal_gpio_setupInput(dig_in_info_t * dig_in_info);

void hal_gpio_init(void)
{
    gpio_install_isr_service(ESP_INTR_FLAG_LEVEL3);
}


void hal_gpio_setupDigOut(  dig_out_info_t * dig_out_info,
                            uint8_t gpio_pin,
                            uint8_t pull_up,
                            uint8_t pull_down,
                            uint8_t start_level)
{
    dig_out_info->gpio_pin      = gpio_pin;
    dig_out_info->pull_up       = pull_up;
    dig_out_info->pull_down     = pull_down;
    dig_out_info->start_level   = start_level;

    hal_gpio_setupOutput(dig_out_info);
}

void hal_gpio_setupDigIn(   dig_in_info_t * dig_in_info,
                            uint8_t gpio_pin,
                            uint8_t pull_up,
                            uint8_t pull_down,
                            uint8_t intr_type,
                            gpio_in_interrupt_fn_t cb_function)
{
    dig_in_info->gpio_pin   = gpio_pin;
    dig_in_info->pull_up    = pull_up;
    dig_in_info->pull_down  = pull_down;
    dig_in_info->intr_type  = intr_type;
    dig_in_info->cb_function= cb_function;

    hal_gpio_setupInput(dig_in_info);
}


static void hal_gpio_setupOutput(dig_out_info_t * dig_out_info)
{
    gpio_config_t io_conf =
    {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1 << dig_out_info->gpio_pin),
        .pull_down_en = dig_out_info->pull_down,
        .pull_up_en = dig_out_info->pull_up,
    };
    gpio_config(&io_conf);
}


static void hal_gpio_setupInput(dig_in_info_t * dig_in_info)
{
      //zero-initialize the config structure.
    gpio_config_t io_conf =
    {
        .intr_type = dig_in_info->intr_type,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1 << dig_in_info->gpio_pin),
        .pull_down_en = dig_in_info->pull_down,
        .pull_up_en = dig_in_info->pull_up,
    };
    gpio_config(&io_conf);
    gpio_intr_enable(dig_in_info->gpio_pin);
    gpio_isr_handler_add(dig_in_info->gpio_pin, dig_in_info->cb_function, NULL);
}


uint8_t hal_gpio_setOutput(uint8_t pin, uint8_t level)
{
    if(ESP_OK == gpio_set_level(pin, level))
    {
        return TRUE;
    }
    return FALSE;
}

