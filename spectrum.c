//i/p f > 19.2khz? fffff

#define LIN_OUT8 1 //set FHT to produce linear output
#define LOG_OUT 0 //turn off FHT logarithmic output
#define FHT_N 256 //FHT sample number ; output bin is 150hz wide and 256 bins
//#define SCALE (1-255) not sure about the value
#include <FHT.h>
byte low, high;

void setup()
{
  Serial.begin(115200);//set sampling rate (not required in the final project)
  ADCSRA = 0b11100101;// set ADC to free running mode and set pre-scalar to 32 (0xe5)
  ADMUX = 0b00000000; // use pin A0 and external voltage reference (o/p has a dc bias of vcc/2 connect it to 3.3v and vpp of 1v)
  DIDR0  = 0x01; // turn off the digital input for adc0
  TIMSK0 = 0;    // turn off timer0 (lower jitter)
}

void loop()
{
  sampleanalogop();
  sampleuse();
}

void sampleanalogop()
{


  // reduces jitter cli();
  cli();// UDRE interrupt slows this way down on arduino1.0
  for (int i = 0 ; i < FHT_N ; i++)
  {
    // save 256 samples
    while (!(ADCSRA & 0x10)); // wait for adc to be ready
    ADCSRA = 0xf5; // restart adc byte
    low = ADCL; // fetch adc data byte
    high = ADCH;
    int k = (high << 8) | low; // form into an int
    k -= 0x0200; // form into a signed int
    k <<= 6; // form into a 16b signed int
    fht_input[i] = k; // put real data into bins

  }
  sei();// set interrupts
  fht_window(); // window the data for better frequency response
  fht_reorder(); // reorder the data before doing the fht
  fht_run(); // process the data in the fht
  fht_mag_lin8(); // take the output of the fht

  /* Output is in fht_lin_out8[]
      fht_lin_out8[0] = first bin magnitude (0hz -> 150hz)
      fht_lin_out8[1] = second bin magnitude (150hz -> 300hz)
      has 128 8 bit values


  */
}

void sampleuse()
{
  float count = FHT_N/2;
  float temp;
  for(int i=0;i<count;i++)
  {
    temp = fht_lin_out8[i]; // max value is 22400 apparently
  }
  
}
