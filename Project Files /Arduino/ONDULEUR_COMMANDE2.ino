
#include "math.h"


#define VPV 80
#define GRIDAMPLITUDE 50*sqrt(2)

#define RATIO   28.28427125         //transformation ration + amplifier gain (>1)
#define ACSG    0.185            // current sensor gain V/A


#define DEGTORAD  M_PI/180

#define GAINV   (5*RATIO)/(1024)  // found by dividing transformer ratio and gain by 1024 
#define GAINC   (5/(1024*ACSG))    // ACS712

#define OFFSETV 2.5*RATIO  //non inverting summer offset (normally around 2.5V)
#define OFFSETC 2.5/ACSG // 0.5*VCC according to datasheet

#define HYST 0.01
#define STEP 5
#define CONT 2   // 1- Digital Hysterisis controller implementation, 2- PWM hysterisis controller, 0- No control

/*
Vcurrent= ACSG* I + offset (offset =2.5V) 
Vcurent = ADCC* 5/1024 
I=(Vcurrent-offset)/ACSG
I=ADCC* 5/(1024 *ACSG) -offset/ACSG
I=ADCC* GAINC         -  OFFSETC

Vpin = ADCV * 5/1024
Vgrid =(Vpin - offset)*RATIO
Vgrid = ADCV*(5*RATIO)/1024 -offset*RATIO
Vgrid = ADCV* GAINV         -OFFSETV 


*/

uint16_t ADCV= 0, ADCC=0;
    float buf=0;

/* ------------------------------------------------------------------------ Config Functions -----------------------------------------------------------*/


void configADC() {
  // Disable digital input buffers on ADC0 and ADC1 to reduce power
    DIDR0 |= (1 << ADC0D) | (1 << ADC1D);

    // Select AVcc as reference voltage, start with ADC0
    ADMUX = (1 << REFS0); // REFS0 = 1 => AVcc, MUX = 0000 => ADC0

    // Set ADC clock prescaler to 64: 16MHz / 64 = 250 kHz 
    ADCSRA = (1 << ADEN)  | // Enable ADC
             (1 << ADIE)  | // Enable ADC interrupt
             (1 << ADPS2) | (1 << ADPS1); // Prescaler = 64 => 250kHz => TADC= 52µS => Tconv= 208µS => Tshift = 104µS 

    // Single conversion mode, interrupt driven
    ADCSRB = 0; // Free-running OFF

    // Start first conversion manually
    ADCSRA |= (1 << ADSC);
}







void configPWM() {
  // Set pin 12 (OC1B) as output
  pinMode(12, OUTPUT);

  // Stop Timer1
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  // Set Fast PWM, TOP = ICR1
  TCCR1A = (1 << COM1B1) | (1 << WGM11);
  TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS10); // No Prescaler
  ICR1 = 1599;  // TOP value for 10kHz: 16MHz / (1 * (1599 + 1)) = 10kHz
  OCR1B = 800;  // Rapport cyclique (~50%)
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* ------------------------------------------------------------------------ regular Functions -----------------------------------------------------------*/





float currentreference(float gridv ,float amplitude ) {
//float currentreference(float gridv ,float amplitude, float phase ) {
  /*float gridref= asin(gridv/GRIDAMPLITUDE); // V(t)= Vmsin(wt+phi)
                                            //(Wt+phi)=arcsin((V(t)/Vm))
                                            //I= Im*sin( (wt+phi)v+phii)*/

  float Iref  = (amplitude/GRIDAMPLITUDE)*gridv;
                                      
  
  return Iref;
  
}






void digitalhysterisis(float diff){
    if (diff > HYST) {
    // Turn on (set PB6)
      PORTB |= (1 << PORTB6);   // Set pin HIGH
      }
    else if (diff < HYST) {
    // Turn off (clear PB6)
      PORTB &= ~(1 << PORTB6);  // Set pin LOW 
      }
}





float PWMhysterisis(float diff,float out){
//  static float out = 0;  
    if (diff > HYST) {
      out = out + STEP;
      }
    else if (diff < HYST) {
      out = out - STEP;
      }
    return out;
}





 



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* ------------------------------------------------------------------------ ISR -----------------------------------------------------------*/

ISR(ADC_vect) {
    static uint8_t discard = 0;
    static uint8_t channel = 0;

//Conversion time : 52µS (TOTAL CONVERSION TIME : 208µS) 


    if (discard) {
        discard = 0;
        ADCSRA |= (1 << ADSC);                // Start next conversion
        return; // Discard this reading (to empty S&H capacitor)
    }

    if (channel == 0) {
          
        ADCV = ADC;                        // Read ADC0
        ADMUX = (ADMUX & 0xF0) | 0x01;     // Switch to ADC1
        channel = 1;
    } else {

        ADCC = ADC;                        // Read ADC1
        ADMUX = (ADMUX & 0xF0) | 0x00;     // Switch to ADC0
        channel = 0;
    }

    discard = 1;                           // Discard next conversion
    ADCSRA |= (1 << ADSC);                // Start next conversion
}








void setup() {
  
cli();// disable global interrupts

configADC();

sei(); // Enable global interrupts
 //Serial.begin(2000000);

#if CONT == 1
  DDRB |= (1 << DDB6);  // Set PB6 (digital pin 12) as output
 
 
#elif CONT ==2
  configPWM();
#elif CONT == 0
  Serial.begin(2000000);
  
#endif

 

}

void loop() {

float gvoltage = ADCV*GAINV - OFFSETV;
float I        = ADCC*GAINC - OFFSETC;
float Iref     = currentreference(gvoltage ,1);
   
#if CONT == 1
    digitalhysterisis(Iref-I);
/*   Serial.print(I);
Serial.print("\t");
Serial.println(0.5);*/
#elif CONT == 2
    gvoltage=0;
    buf=PWMhysterisis(Iref-I,buf);
    float m=(buf+gvoltage)/((float)(VPV/2));
    float alpha=0.5*(m+1);
    if ( alpha >= 0.95 ){
      alpha =0.95;
    }
    else if(alpha <=0.05){
      alpha =0.05;
    }
    OCR1B = (uint16_t)(alpha*(float)(ICR1));  

/*  Serial.print(I);
Serial.print("\t");
Serial.println(Iref);*/
 
#elif CONT ==0



Serial.print(gvoltage);
Serial.print("\t");
Serial.println(Iref);


#endif


  
}
