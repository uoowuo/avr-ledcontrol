/**
 * avr-ledcontrol, v1.0.2
 *
 * This is a simple program to cycle between predefined combinations of LED levels ("colors")
 * with crossfading and delays. It supports any number of LED types, but one per pin.
 * It was written for ATtiny45, but should be easily portable to other devices.
 *
 * 
 * Pin diagram for DIP8 package:
 *
 *       +---+ VCC
 *       |o  | 
 *  LED2 |   | LED1
 *   GND +---+ LED0
 *
 * ATtiny will toast at over 40 mA per output pin, so use MOSFETs with appropriate heatsinks.
 *
 * In my setup I used red, green & blue LEDs, and preset color format { R 0..255, G 0..255, B 0..255},
 * but you can use any.
 */

#define F_CPU 1000000             // Clock frequency, Hz
#include <avr/io.h>
#include <util/delay.h>

#define true 1
#define false !(true)

#define LED0 0                    // Red
#define LED1 1                    // Green
#define LED2 2                    // Blue
#define LED_COUNT 3               // How many LEDs to use
#define PRESET_COUNT 5            // How many preset LED level combinations to use
#define CROSSFADE_SLOWNESS 80     // How long to wait between crossfading steps, ms
#define PRESET_DISPLAY_TIME 9001  // How long to show a particular preset unchanged, ms

int main(void) {
    

      ///////////
     // Setup //
    ///////////

    // Data structures
    struct Led {
        uint8_t pin;              // LED pin
        uint8_t volatile *pwm;    // Associated PWM register
        uint8_t level;            // Current brightness level, 0..255
        uint8_t mismatch;         // Whether LED's level matches target preset, boolean
    };

    // Preset LED level combinations ("colors") to cycle through
    uint8_t presets[][LED_COUNT] = {
        {255, 80, 80},
        {80, 255, 80},
        {50, 255, 0},
        {255, 0, 0},
        {255, 0, 120}
    };

    // LED setup information
    struct Led leds[LED_COUNT];
    leds[0].pin = PB0;
    leds[1].pin = PB1;
    leds[2].pin = PB4;
    leds[0].pwm = &OCR0A;
    leds[1].pwm = &OCR0B;
    leds[2].pwm = &OCR1B;

    
    // Register setup
    // @todo use a function or macro to set and unset registers
    DDRB |= (1 << leds[0].pin) | (1 << leds[1].pin) | (1 << leds[2].pin);      // Configure LED pins as output
    PORTB &= ~((1 << leds[0].pin) | (1 << leds[1].pin) | (1 << leds[2].pin));  // Pull all LEDs to low
    *leds[0].pwm = *leds[1].pwm = *leds[2].pwm = 255;                          // Set initial PWM values @todo start with 0
    
    TCCR0A = (1 << COM0A1) | (1 << COM0B1) | (1 << WGM00);  // Phase correct PWM mode for PB0, PB1
    GTCCR  = (1 << PWM1B) | (1 << COM1B0);                  // Phase correct PWM mode for PB4
    TCCR0B = (1 << CS01);                                   // Clock source = CLK/8, start PWM for PB0, PB1
    TCCR1  = (4 << CS10);                                   // Clock source = CLK/8, start PWM for PB4


      //////////////////////
     // Crossfading loop //
    //////////////////////

    // Switch between presets continuously
    while (true) {
        
        // For every preset in sequence
        for(uint8_t preset = 0; preset < PRESET_COUNT; preset++) {

            // Assume all LEDs mismatch and mark them as such too
            int ledsMismatch = LED_COUNT;
            for (uint8_t led = 0; led < LED_COUNT; led++) {
                leds[led].mismatch = true;
            }
            
            // Zero in on the preset until current and preset LED levels match
            while (ledsMismatch) {
                
                // For each LED, make a step up or down, or mark as on target
                for (uint8_t led = 0; led < LED_COUNT; led++) {

                    if (! leds[led].mismatch) continue;  // Don't touch matching LEDs

                    if (leds[led].level < presets[preset][led]) {
                        leds[led].level++;
                    } else if (leds[led].level > presets[preset][led]) {
                        leds[led].level--;
                    } else {
                        leds[led].mismatch = false;  // This LED is on target
                        ledsMismatch--;              // One less to worry about
                    }
                    *leds[led].pwm = leds[led].level;
                }

                _delay_ms(CROSSFADE_SLOWNESS);  // Slow down crossfading
            }
            _delay_ms(PRESET_DISPLAY_TIME);     // Display current preset unchanged for some time
        }
    }
    
    return 0;
}