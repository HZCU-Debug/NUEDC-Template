#include <stdint.h>

extern int main(void);
extern uint32_t _data_load;
extern uint32_t _data_start;
extern uint32_t _data_end;
extern uint32_t _bss_start;
extern uint32_t _bss_end;
extern uint32_t _stack_top;
#if TI_CPP_RUNTIME
extern void __libc_init_array(void);
#endif

static void reset_handler(void);

#if TI_CPP_RUNTIME
/**
 * @brief Provides the C runtime initialization hook
 */
void _init(void)
{
}

/**
 * @brief Provides the C runtime finalization hook
 */
void _fini(void)
{
}
#endif

/**
 * @brief Stops execution for an unhandled interrupt
 */
void default_handler(void);

/**
 * @brief Declares an interrupt entry that application code can override
 */
#define WEAK_HANDLER(name) \
    void name(void) __attribute__((weak, alias("default_handler")))

WEAK_HANDLER(NMI_Handler);
WEAK_HANDLER(HardFault_Handler);
WEAK_HANDLER(SVC_Handler);
WEAK_HANDLER(PendSV_Handler);
WEAK_HANDLER(SysTick_Handler);
WEAK_HANDLER(GROUP0_IRQHandler);
WEAK_HANDLER(GROUP1_IRQHandler);
WEAK_HANDLER(TIMG8_IRQHandler);
WEAK_HANDLER(UART3_IRQHandler);
WEAK_HANDLER(ADC0_IRQHandler);
WEAK_HANDLER(ADC1_IRQHandler);
WEAK_HANDLER(CANFD0_IRQHandler);
WEAK_HANDLER(DAC0_IRQHandler);
WEAK_HANDLER(SPI0_IRQHandler);
WEAK_HANDLER(SPI1_IRQHandler);
WEAK_HANDLER(UART1_IRQHandler);
WEAK_HANDLER(UART2_IRQHandler);
WEAK_HANDLER(UART0_IRQHandler);
WEAK_HANDLER(TIMG0_IRQHandler);
WEAK_HANDLER(TIMG6_IRQHandler);
WEAK_HANDLER(TIMA0_IRQHandler);
WEAK_HANDLER(TIMA1_IRQHandler);
WEAK_HANDLER(TIMG7_IRQHandler);
WEAK_HANDLER(TIMG12_IRQHandler);
WEAK_HANDLER(I2C0_IRQHandler);
WEAK_HANDLER(I2C1_IRQHandler);
WEAK_HANDLER(AES_IRQHandler);
WEAK_HANDLER(RTC_IRQHandler);
WEAK_HANDLER(DMA_IRQHandler);

__attribute__((section(".intvecs"), used))
static void (*const interrupt_vectors[48])(void) = {
    [0] = (void (*)(void)) &_stack_top,
    [1] = reset_handler,
    [2] = NMI_Handler,
    [3] = HardFault_Handler,
    [11] = SVC_Handler,
    [14] = PendSV_Handler,
    [15] = SysTick_Handler,
    [16] = GROUP0_IRQHandler,
    [17] = GROUP1_IRQHandler,
    [18] = TIMG8_IRQHandler,
    [19] = UART3_IRQHandler,
    [20] = ADC0_IRQHandler,
    [21] = ADC1_IRQHandler,
    [22] = CANFD0_IRQHandler,
    [23] = DAC0_IRQHandler,
    [25] = SPI0_IRQHandler,
    [26] = SPI1_IRQHandler,
    [29] = UART1_IRQHandler,
    [30] = UART2_IRQHandler,
    [31] = UART0_IRQHandler,
    [32] = TIMG0_IRQHandler,
    [33] = TIMG6_IRQHandler,
    [34] = TIMA0_IRQHandler,
    [35] = TIMA1_IRQHandler,
    [36] = TIMG7_IRQHandler,
    [37] = TIMG12_IRQHandler,
    [40] = I2C0_IRQHandler,
    [41] = I2C1_IRQHandler,
    [44] = AES_IRQHandler,
    [46] = RTC_IRQHandler,
    [47] = DMA_IRQHandler,
};

static void reset_handler(void)
{
    uint32_t *source = &_data_load;

    for (uint32_t *destination = &_data_start; destination < &_data_end;) {
        *destination++ = *source++;
    }

    for (uint32_t *destination = &_bss_start; destination < &_bss_end;) {
        *destination++ = 0;
    }

#if TI_CPP_RUNTIME
    __libc_init_array();
#endif
    main();
    default_handler();
}

void default_handler(void)
{
    while (1) {
    }
}
