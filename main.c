// Name: charlieplexed tree
// Author: Steve Annessa
// Copyright: Steal it if you like but give me credit
// License: A license to ill

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define SCALE_FACTOR 60 //(2s/(1/(8MHz/1024))/255 = ~60

int counter;
int led_num;
int full_on;
int full_off;

int main(void)
{
  //this counter is used because at 8MHz the longest
  //time between interrupts prescaled at 1024 is
  //128us; so every 128us the interrupt will fire and
  //this counter will be incremented until it reaches
  //OCR0A * SCALE_FACTOR; in this case scale_factor will be 60
  //because the longest time I want to go without change is 2 seconds
  //that's (2s/(1/(8MHz/1024))/255 ----- 255 is the largest value for OCR0A
  //and (2s/(1/(8MHz/1024))/255 is ~60
  counter = 0;

  //use this value to iterate through the charlieplexed leds
  led_num = 0;

  //use these to handle the state when the lights are completely lit and completely off
  full_on = 0;
  full_off = 0;

  ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);  // prescaler divided by 128
  ADMUX |= (1 << REFS0);  //avcc with external capacitor at aref pin
  //ADCSRA |= (1 << ADFR); this doesn't seem to be in the 168
  ADMUX |= (1 << ADLAR);  //left adjust register to get 0-255
  ADCSRA |= (1 << ADEN);  //enable ad
  ADCSRA |= (1 << ADSC);  //start conversion

  //setup pins for output and input
  DDRD |= (1 << PORTD4);
  PORTD |= (1 << PORTD4);

  // setup timer
  TCCR0A = (1 << WGM01);               //clear timer on compare
  TCCR0B = (1 << CS02) | (1 << CS00);  //prescale by 1024; 1(8MHz/1024) = an action every 128us
  TIMSK0 = (1 << OCIE0A);              //enable the timera interrupt
  OCR0A = ADCH;  //get the value from the ADC

  //enable interrupts
  sei();

  while (1)
  {
    ADCSRA |= (1 << ADSC);  //start conversion
    if (ADCH < 5)
    {
      full_on = 1;
      full_off = 0;
      OCR0A = 10; //this is an arbitrary number, may need to change; I just want it fast
    }
    else if (ADCH > 250)
    {
      full_on = 0;
      full_off = 1;
      OCR0A = ADCH;
    }
    else
    {
      if (full_off == 1)
      {
        //turn the yellow led back on
        PORTD |= (1 << PORTD4);
      }
      full_on = 0;
      full_off = 0;
      OCR0A = ADCH;
    }
  }
  return 0;
}

ISR (TIMER0_COMPA_vect)
{
  counter++;

  int updated_scale_factor;
  if (full_on == 1)
  {
    updated_scale_factor = 0;
  }
  else
  {
    updated_scale_factor = SCALE_FACTOR;
  }

  if (full_off == 1)
  {
    PORTD = 0; //turn off the entire thing!
  }
  else if (counter >= updated_scale_factor)
  {
    counter = 0;

    //begin charlieplexing routine
    if (led_num == 0)
    {
      //led 1

      //inputs
      DDRD &= ~(1 << PORTD2) & ~(1 << PORTD3);

      //outputs
      DDRD |= (1 << PORTD0) | (1 << PORTD1);

      //low
      PORTD &= ~(1 << PORTD1);

      //high
      PORTD |= (1 << PORTD0);

      //move on to the next led
      led_num++;
    }
    else if (led_num == 1)
    {
      //led 2

      //inputs
      DDRD &= ~(1 << PORTD0) & ~(1 << PORTD3);

      //outputs
      DDRD |= (1 << PORTD1) | (1 << PORTD2);

      //low
      PORTD &= ~(1 << PORTD2);

      //high
      PORTD |= (1 << PORTD1);

      //move on to the next led
      led_num++;
    }
    else if (led_num == 2)
    {
      //led 3

      //inputs
      DDRD &= ~(1 << PORTD0) & ~(1 << PORTD1);

      //outputs
      DDRD |= (1 << PORTD2) | (1 << PORTD3);

      //low
      PORTD &= ~(1 << PORTD3);

      //high
      PORTD |= (1 << PORTD2);

      //move on to the next led
      led_num++;
    }
    else if (led_num == 3)
    {
      //led 4

      //inputs
      DDRD &= ~(1 << PORTD2) & ~(1 << PORTD3);

      //outputs
      DDRD |= (1 << PORTD0) | (1 << PORTD1);

      //low
      PORTD &= ~(1 << PORTD0);

      //high
      PORTD |= (1 << PORTD1);

      //move on to the next led
      led_num++;
    }
    else if (led_num == 4)
    {
      //led 5

      //inputs
      DDRD &= ~(1 << PORTD0) & ~(1 << PORTD3);

      //outputs
      DDRD |= (1 << PORTD1) | (1 << PORTD2);

      //low
      PORTD &= ~(1 << PORTD1);

      //high
      PORTD |= (1 << PORTD2);

      //move on to the next led
      led_num++;
    }
    else if (led_num == 5)
    {
      //led 6

      //inputs
      DDRD &= ~(1 << PORTD0) & ~(1 << PORTD1);

      //outputs
      DDRD |= (1 << PORTD2) | (1 << PORTD3);

      //low
      PORTD &= ~(1 << PORTD2);

      //high
      PORTD |= (1 << PORTD3);

      //move on to the next led
      led_num++;
    }
    else if (led_num == 6)
    {
      //led 7

      //inputs
      DDRD &= ~(1 << PORTD1) & ~(1 << PORTD3);

      //outputs
      DDRD |= (1 << PORTD0) | (1 << PORTD2);

      //low
      PORTD &= ~(1 << PORTD2);

      //high
      PORTD |= (1 << PORTD0);

      //move on to the next led
      led_num++;
    }
    else if (led_num == 7)
    {
      //led 8

      //inputs
      DDRD &= ~(1 << PORTD1) & ~(1 << PORTD3);

      //outputs
      DDRD |= (1 << PORTD0) | (1 << PORTD2);

      //low
      PORTD &= ~(1 << PORTD0);

      //high
      PORTD |= (1 << PORTD2);

      //move on to the next led
      led_num++;
    }
    else if (led_num == 8)
    {
      //led 9

      //inputs
      DDRD &= ~(1 << PORTD1) & ~(1 << PORTD2);

      //outputs
      DDRD |= (1 << PORTD0) | (1 << PORTD3);

      //low
      PORTD &= ~(1 << PORTD3);

      //high
      PORTD |= (1 << PORTD0);

      //move on to the next led
      led_num++;
    }
    else if (led_num == 9)
    {
      //led 10

      //inputs
      DDRD &= ~(1 << PORTD1) & ~(1 << PORTD2);

      //outputs
      DDRD |= (1 << PORTD0) | (1 << PORTD3);

      //low
      PORTD &= ~(1 << PORTD0);

      //high
      PORTD |= (1 << PORTD3);

      //move on to the next led
      led_num++;
    }
    else if (led_num == 10)
    {
      //led 11

      //inputs
      DDRD &= ~(1 << PORTD0) & ~(1 << PORTD2);

      //outputs
      DDRD |= (1 << PORTD1) | (1 << PORTD3);

      //low
      PORTD &= ~(1 << PORTD3);

      //high
      PORTD |= (1 << PORTD1);

      //move on to the next led
      led_num++;
    }
    else if (led_num == 11)
    {
      //led 12

      //inputs
      DDRD &= ~(1 << PORTD0) & ~(1 << PORTD2);

      //outputs
      DDRD |= (1 << PORTD1) | (1 << PORTD3);

      //low
      PORTD &= ~(1 << PORTD1);

      //high
      PORTD |= (1 << PORTD3);

      //reset to the first led
      led_num = 0;
    }
  }
}
