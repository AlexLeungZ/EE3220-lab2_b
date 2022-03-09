 /***************************************************************************
                        MULTIPLE INTERRUPT PROGRAM
 **************************************************************************/
#include "mbed.h"

#define BLINKING_RATE    1000ms // Blinking rate in milliseconds
#define BLINKING_RATE_2   600ms // Blinking rate in milliseconds

static UnbufferedSerial serial_port(USBTX, USBRX);
static volatile int  serial_rx;
static volatile int  number_to_blink;
static volatile int  timer_timeout;
Timeout  timer;      // define ticker timer 

char msg1[] = "Serial Interrupt: LED will blink "; 
char msg2[] = " times!\r\n"; 

void isr_timer()
{
    timer_timeout = 1;
}

void isr_serial()
{
    char c;
    if (serial_port.read(&c, 1)) {
        serial_rx = 1;
        number_to_blink = (int)c - (int)48 ;
        serial_port.write(msg1, sizeof(msg1));
        serial_port.write(&c, 1);
        serial_port.write(msg2, sizeof(msg2));
    }
}

int main(void)
{
    char timer_blink;
    static DigitalOut led(LED1);
    // Set desired properties (9600-8-N-1).
    serial_port.baud(9600);
    serial_port.format(
        /* bits */ 8,
        /* parity */ SerialBase::None,
        /* stop bit */ 1
    ); 
    serial_port.attach(&isr_serial, SerialBase::RxIrq);
    timer.attach(&isr_timer, 8000ms); 
 
    serial_rx = 0;
    number_to_blink = 0;
    timer_blink = 4;
    while (true) {
        if(serial_rx == 1){  
            while(number_to_blink >0){
                led = 1;
                ThisThread::sleep_for(BLINKING_RATE);
                led = 0;
                ThisThread::sleep_for(BLINKING_RATE);
                number_to_blink -= 1;
            }
            serial_rx = 0;
        }else if(timer_timeout){
            printf("Timer timeout occured ! \r\n");
            timer_timeout = 0;
            while(timer_blink){
                led = !led;
                ThisThread::sleep_for(BLINKING_RATE);
                timer_blink -= 1;
            }
         }else {
            led = 1; 
         }
     }
}