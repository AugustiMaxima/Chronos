#include <bwio.h>
#include <ts7200.h>

int* com1_flags = (int *)( UART1_BASE + UART_FLAG_OFFSET );
int* com1_data = (int *)( UART1_BASE + UART_DATA_OFFSET );
int* com2_flags = (int *)( UART2_BASE + UART_FLAG_OFFSET );
int* com2_data = (int *)( UART2_BASE + UART_DATA_OFFSET );

int main( int argc, char* argv[] ) {

	bwsetfifo(COM1, OFF);
	bwsetfifo(COM2, OFF);
	bwsetspeed(COM1, 2400);
    bwsetstopbits(COM1, ON);


	while (1) {

		if (
			(*com1_flags & RXFF_MASK)
		) {
			char ch = *com1_data;
            bwprintf(COM2, "COM1: %x\r\n", ch);
		}

		if (
		    (*com2_flags & RXFF_MASK)
		) {
			char ch = *com2_data;
            bwprintf(COM2, "%c", ch);

            if (ch == 's') {
                bwprintf(COM1, "%c", 133);
                bwprintf(COM2, "\r\n");
            }
            if (ch == 'q') {
                return 0;
            }
		}

	}

	return 0;
}
