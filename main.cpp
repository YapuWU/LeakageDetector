
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/cpufunc.h>

#define PIN_LED    PB5
#define PIN_BUZZER PB4

#define TurnOnLED()     (PORTB |= (1 << PIN_LED))    
#define TurnOffLED()    (PORTB &= ~(1 << PIN_LED))   
#define TurnOnBuzzer()  (PORTB |= (1 << PIN_BUZZER)) 
#define TurnOffBuzzer() (PORTB &= ~(1 << PIN_BUZZER))

ISR(WDT_vect)
{
    
}

void EnableWDTAndSleep(uint8_t timeout)
{
    wdt_enable(timeout);
    sei();
    _WD_CONTROL_REG |= _BV(WDIE);       // Enable WDT interrupt
    set_sleep_mode(SLEEP_MODE_PWR_DOWN); 
    sleep_enable();     
    sleep_cpu();         // Enter sleep mode    
    sleep_disable();    // Wake up from sleep
    _WD_CONTROL_REG |= _BV(WDIE); // Re-enable WDT interrupt             
}

bool IsLeakageDetected()
{
    bool leakageDetected = false; 
    ACSR &= ~_BV(ACD); // Enable Analog Comparator
    //Wait for the comparator to stabilize
    _NOP();
    _NOP();
    _NOP();
    _NOP();
    _NOP();
    leakageDetected = bit_is_set(ACSR, ACO);    
    ACSR |= _BV(ACD);       // Disable Analog Comparator
    
    return leakageDetected; 
}

FUSES = 
{
    .low = FUSE_SPIEN & FUSE_CKDIV8 & FUSE_CKSEL1 & FUSE_SUT0,
    .high = HFUSE_DEFAULT
};

int main()
{
    // This remember the count that the leakage is detected
    // if the count is greater than 5, it means that the leakage is detected
    // This is used to prevent the leakage from being detected by an external noise
    uint8_t leakageCount = 0;

    wdt_reset();       
    power_all_disable(); 

    //Enable pull-up resistors on all pins to reduce power consumption during sleep
    PORTB = 0xFF;
    DIDR0 |= _BV(AIN1D) | _BV(AIN0D); // Disable digital input on AIN1 and AIN0

    // initialize LED and Buzzer pins
    DDRB  |=  (1 << PIN_LED) | (1 << PIN_BUZZER);
    
    TurnOffBuzzer();
    TurnOffLED();
   
    while (true)
    {
        // Sleep 4 Seconds before checking the leakage
        EnableWDTAndSleep(WDTO_4S);
        if(IsLeakageDetected())
        {
            leakageCount++;
        }
        else
        {
            leakageCount = 0; // Reset count if no leakage is detected
        }
        if (leakageCount >= 5)
        {
            // If leakage is detected, sound an alert for 500ms
            TurnOnBuzzer();
            EnableWDTAndSleep(WDTO_500MS);
            TurnOffBuzzer();
        }

        // Flash LED to indicate activity
         TurnOnLED();
         EnableWDTAndSleep(WDTO_60MS);
         TurnOffLED();
    }

    return 1;
}
