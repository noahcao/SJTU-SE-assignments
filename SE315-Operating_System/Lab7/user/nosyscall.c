// hello, world
#include <inc/lib.h>

#define COM1		0x3F8
#define COM_TX		0	// Out: Transmit buffer (DLAB=0)
#define COM_LSR		5	// In:	Line Status Register
#define   COM_LSR_TXRDY	0x20	//   Transmit buffer avail

static inline uint8_t
inb(int port)
{
	uint8_t data;
	asm volatile("inb %w1,%0" : "=a" (data) : "d" (port));
	return data;
}

static inline void
outb(int port, uint8_t data)
{
	asm volatile("outb %0,%w1" : : "a" (data), "d" (port));
}

static void
delay(void)
{
	inb(0x84);
	inb(0x84);
	inb(0x84);
	inb(0x84);
}

static void
serial_putc(int c)
{
	int i;

	for (i = 0;
	     !(inb(COM1 + COM_LSR) & COM_LSR_TXRDY) && i < 12800;
	     i++)
		delay();

	outb(COM1 + COM_TX, c);
}

static void
puts(char *s)
{
	for (char *p = s; *p; p++)
		serial_putc(*p);
}

void
_start()
{
	puts("hello, world!\r\n");
	for (;;);
}
