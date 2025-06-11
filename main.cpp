
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <stdio.h>
#define PINLED    PB3

#define LED_ON()  (PORTB |= (1 << PINLED))
#define LED_OFF() (PORTB &= ~(1 << PINLED))


#define PINBUZZER PB4
#define BUZZER_ON() (PORTB |= (1 << PINBUZZER))
#define BUZZER_OFF() (PORTB &= ~(1 << PINBUZZER))

#define ACGROUND PB1    //Analog Comparator Ground PIN
#define ACPOSITIVE PB2  //Analog Comparator Positive PIN

ISR(WDT_vect)
{
    wdt_reset(); // Reset watchdog timer
}

void init_buzzer()
{
    // Set buzzer pin as output
    DDRB |= (1 << PINBUZZER);
    BUZZER_OFF(); // Ensure buzzer is off initially
}

void int_led()
{
    // Set LED pin as output
    DDRB |= (1 << PINLED);
    LED_OFF(); // Ensure LED is off initially
}
void init_analog_comparator()
{
    //Disable Digital Input on ACGROUND and ACPOSITIVE
    DIDR0 |= (1 << AIN1D) | (1 << AIN0D);
}

//Check if positive input is higher than ground input
//Before checking, delay 5 clock cycles to allow the comparator to stabilize
bool IsLeaking()
{
    _delay_us(1); // Delay to allow the comparator to stabilize (5 clock cycles at 1 MHz)
    return (ACSR & (1 << ACO)) != 0; 
}

//Use watchdog timer to wake up from sleep mode every 4 seconds
//using interrup and System Reset mode
void init_watchdog()
{
    cli();
    wdt_reset(); // Reset watchdog timer
    WDTCR = (1 << WDCE) | (1 << WDE); // Enable watchdog change
    WDTCR = (1 << WDTIE) | (1<<WDTIE) | (1 << WDP3) | (0 << WDP2) | (0 << WDP1) | (0 << WDP0); // Enable watchdog interrupt
    sei();
}


int main()
{
    //Minimum power consumption
    PRR |= 1<<PRADC; // Disable ADC
    PRR |= 1<<PRTIM0; // Disable Timer0
    //enable pull-up resistors on unused pins
    PORTB |= (1 << PB3) |  (1 << PB2); // Enable pull-up resistors on unused pins
    init_buzzer();
    int_led();
    while(true)
    {
        
        init_watchdog();
        // entering powder down mode to save power
        set_sleep_mode(SLEEP_MODE_PWR_DOWN);
        sleep_enable();
        sleep_bod_disable(); // Disable brown-out detection during sleep
        sleep_cpu();     // Enter power down mode
        sleep_disable(); // Wake up from power down mode

        if(IsLeaking())
        {
            BUZZER_ON(); // Turn on buzzer if leak is detected
            _delay_ms(1000); // Keep buzzer on for 1 second
            BUZZER_OFF(); // Turn off buzzer
        }
        else
        {
            LED_ON(); // Turn on LED if no leak is detected
            BUZZER_OFF(); // Ensure buzzer is off if no leak is detected
            _delay_ms(100); 
            LED_OFF(); // Turn off LED
        }
    }

   return 1;
}

