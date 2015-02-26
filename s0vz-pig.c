#include <stdio.h>
#include <pigpio.h>

void alert(int pin, int level, uint32_t tick);

int main(int argc, char *argv[])
{
	if (gpioInitialise()<0) return 1;

	gpioSetAlertFunc(24, alert); /* call alert when LDR changes state */
	
	return 0;
}

void alert(int pin, int level, uint32_t tick)
{
	printf("/**********************/\n");
	printf("Pin:\t%u\n", pin);
	printf("Level\t:%u\n", level);
	printf("Tick:\t%u\n", tick);
	printf("/**********************/\n");
}