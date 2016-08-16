#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

#include "driverlib/can.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"

#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"

volatile int c2dflag = 0;
volatile int d2cflag = 0;
volatile char c2d;
volatile char d2c;

void UART0IntHandler(void)
{
	if(c2dflag == 0)
	{
		uint32_t intStatus = UARTIntStatus(UART0_BASE, false);
		UARTIntClear(UART0_BASE, intStatus);
		if(intStatus == UART_INT_RT || intStatus == UART_INT_RX) c2d = UARTCharGet(UART0_BASE);
		c2dflag = 1;
	}
}

void XBEEIntHandler(void)
{
	if(d2cflag == 0)
	{
		uint32_t intStatus = UARTIntStatus(UART1_BASE, false);
		UARTIntClear(UART1_BASE, intStatus);
		if(intStatus == UART_INT_RT || intStatus == UART_INT_RX) d2c = UARTCharGet(UART1_BASE);
		d2cflag = 1;
	}
}

int main(void) {
	SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
	uint32_t SysClkFreq = SysCtlClockGet();
	uint32_t baudrate = 115200;

	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralReset(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	UARTConfigSetExpClk(UART0_BASE, SysClkFreq, baudrate, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
	IntEnable(INT_UART0);
	SysCtlPeripheralEnable(GPIO_PORTA_BASE);
	GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_3);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
	SysCtlPeripheralReset(SYSCTL_PERIPH_UART1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	GPIOPinConfigure(GPIO_PB0_U1RX);
	GPIOPinConfigure(GPIO_PB1_U1TX);
	GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	UARTConfigSetExpClk(UART1_BASE, SysClkFreq, baudrate, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
	IntEnable(INT_UART1);

	UARTIntRegister(UART0_BASE, UART0IntHandler);
	UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
	UARTIntRegister(UART1_BASE, XBEEIntHandler);
	UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_RT);
	IntMasterEnable();

	/*while(1)
	{
		if(c2dflag == 1)
		{
			UARTCharPut(UART1_BASE, c2d);
			c2dflag = 0;
		}
		if(d2cflag == 1)
		{
			UARTCharPut(UART0_BASE, d2c);
			d2cflag = 0;
		}
	}*/
	while(1)
	{
		UARTCharPut(UART0_BASE, 'c');
		UARTCharPut(UART1_BASE, 'c');
	}
	return 0;
}
