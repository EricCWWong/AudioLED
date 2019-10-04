/*
Purpose:
  This program allows a teensy 3.6 module and teensy audio baord to perform FFT on the audion input from
  a micro USB source. Then, we output the music to a speaker through the audio baord and assign the lighting
  patterns of the LEDs with the FFT data. It can be thought of as disco lights.

Module: Teensy 3.6 + Teensy audio board
Audio input: Micro-USB source
Audio output: 3.5mm audio jack

LEDs type: WS2812
Number of LEDs strips: 5
Number of LEDs per strip: 25
Power supply: 5V

Written by Eric Wong and Thomas Dance.
*/


#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Adafruit_NeoPixel.h>
#include <math.h>

#define NUM_LEDS 25 //Number of LEDs per strip.
#define PIN1 25 //Pin number for the 1st LED strip.
#define PIN2 26 //Pin number for the 2nd LED strip.
#define PIN3 27 //Pin number for the 3rd LED strip.
#define PIN4 28 //Pin number for the 4th LED strip.
#define PIN5 29 //Pin number for the 5th LED strip.
#define VOLUME 0.60 //Default volume.


//Global variables:

//Global variables for colour selection:
int stop_col = 0; //The conditioning variable to run the colour change. //stop_col = 1: Stop running the colour change function.
int colour = 1; //This variable defines the colour of the LEDs: 1=r, 2=b, 3=g.

//The conditioning variable to run the volume change.
//stop_vol = 1: Stop taking inputs from volume buttons.
int stop_vol = 0;

//The volume of the music. Default setting is 0.40.
//Can be changed with volume button.
float vol = VOLUME;


// GUItool: begin automatically generated code
AudioInputUSB            usb1;           //xy=368,328
AudioAnalyzeFFT256       fft256_2;       //xy=674,413
AudioAnalyzeFFT256       fft256_1;       //xy=684,218
AudioOutputI2S           i2s1;           //xy=686,324
AudioConnection          patchCord1(usb1, 0, fft256_1, 0);
AudioConnection          patchCord2(usb1, 0, i2s1, 0);
AudioConnection          patchCord3(usb1, 1, i2s1, 1);
AudioConnection          patchCord4(usb1, 1, fft256_2, 0);
AudioControlSGTL5000     sgtl5000_1;
// GUItool: end automatically generated code


//Setting up LEDs strips:
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN1);//, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(NUM_LEDS, PIN2);//, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip3 = Adafruit_NeoPixel(NUM_LEDS, PIN3);//, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip4 = Adafruit_NeoPixel(NUM_LEDS, PIN4);//, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip5 = Adafruit_NeoPixel(NUM_LEDS, PIN5);//, NEO_GRB + NEO_KHZ800);


//The default setup: (this only run once)
void setup() {
    // Assigning audio memory:
    AudioMemory(10);

    // set up Serial:
    Serial.begin(9600);

    // set up the LED strip:
    strip.begin();
    strip.clear();
    strip.setBrightness(255);
    //strip.show(); // Initialize all pixels to 'off'

    // set up the LED strip2:
    strip2.begin();
    strip2.clear();
    strip2.setBrightness(255);
    //strip2.show(); // Initialize all pixels to 'off'

    // set up the LED strip3:
    strip3.begin();
    strip3.clear();
    strip3.setBrightness(255);
    //strip3.show(); // Initialize all pixels to 'off'

    // set up the LED strip4:
    strip4.begin();
    strip4.clear();
    strip4.setBrightness(255);
    //strip4.show(); // Initialize all pixels to 'off'

    // set up the LED strip5:
    strip5.begin();
    strip5.clear();
    strip5.setBrightness(255);
    //strip5.show(); // Initialize all pixels to 'off'

    // Set up the audio adaptor:
    sgtl5000_1.enable();
    sgtl5000_1.volume(1);

    //Set up pin for volume key:
    pinMode(36, OUTPUT);
    pinMode(39, INPUT);
    pinMode(38, INPUT);
    digitalWrite(36, HIGH);

}

//The loop: (runs repeatedly)
void loop() {
//Here, we introduce the volume changing mechanism. The change in volume is done discretely.

    //Detected that the buttons are not pressed and set stop_vol = 0 to allow input from buttons:
    if (digitalRead(39) == LOW and digitalRead(38) == LOW) {
      stop_vol = 0;
    }
    if (stop_vol == 0) {
      //The function to increase volume:
      if (digitalRead(39) == HIGH){
        vol = vol + 0.01;
        stop_vol = 1; //Set stop_vol = 1 to refuse any further input until buton is released.
      }
      //The function to decrease volume:
      if (digitalRead(38) == HIGH){
        vol = vol - 0.01;
        stop_vol = 1; //Set stop_vol = 1 to refuse any further input until buton is released.
      }
    }
    //Assign volume to audio adaptor:
    sgtl5000_1.volume(vol);

//Now we extract data from the music and assign the data to the LEDs.
    //Array of the averaged FFT and bins:
    uint16_t average[128];
    uint16_t bins[32];

    //Variable checking if all FFT is 0, it is used to decide the timing of changing colours:
    int sum;

    //The loop to extract the FFT data and assigned it to the LEDs:
    for(int i = 0; i < 128; i++)  {
        average[i] = (fft256_1.output[i] + fft256_2.output[i])/2; //Averaging L-R Channel.

        //Changing the data to log scale:
        // float f = log10(average[i]+1);  // f is logarithmetic of the average of L-R Channel.
        // if(f>avg_max[i]) {
        //   avg_max[i] = f;
        // }

        // Test code:
        //Serial.print(avg_max[i]-f);
        //Serial.print(",");

        //Assigning the data of high revolution sound spectrum to LEDs:
        if (i > 28 && i < 54){
          //Colour selection of the LEDs: (1=r,2=b,3=g)
          if (colour == 1)      {
            strip3.setPixelColor(i-29, average[i-29]>>4, 0, 0);
            strip4.setPixelColor(i-29, average[i-29]>>4, 0, 0);
          }
          else if (colour == 2) {
            strip3.setPixelColor(i-29,0, average[i-29]>>4, 0);
            strip4.setPixelColor(i-29,0, average[i-29]>>4, 0);
          }
          else if (colour ==3)  {
            strip3.setPixelColor(i-29, 0,0, average[i-29]>>4);
            strip4.setPixelColor(i-29, 0,0, average[i-29]>>4);
          }

        }

        //Assigning low resolution sound data to LEDs:
        // Decreasing resolution of the sound data, by combining 4 data into 1, 128 data becomes 32 data.
        if (i%4 == 0)   bins[i/4] = 0;
        bins[i/4] += min(average[i]>>4, 255-bins[i/4]);

        // Setting sum=0:
        if (i == 0) sum = 0;

        //Sending data to LEDs:
        if (i%4 == 3){
            //Colour selection of the LEDs: (1=r,2=b,3=g)
            if (colour == 1)      {
              strip.setPixelColor(i/4, bins[i/4], 0, 0);
              strip2.setPixelColor(i/4, bins[i/4], 0, 0);
              strip5.setPixelColor(i/4, bins[i/4], 0, 0);
            }
            else if (colour == 2) {
              strip.setPixelColor(i/4, 0, bins[i/4], 0);
              strip2.setPixelColor(i/4, 0, bins[i/4], 0);
              strip5.setPixelColor(i/4, 0, bins[i/4], 0);
            }
            else if (colour == 3) {
              strip.setPixelColor(i/4, 0, 0, bins[i/4]);
              strip2.setPixelColor(i/4, 0, 0, bins[i/4]);
              strip5.setPixelColor(i/4, 0, 0, bins[i/4]);
            }

            //Adding every FFT:
            sum = sum + bins[i/4];

        }


    }
    //Test code:
    //Serial.println("");

//Finally, we change colours when the music stop.
    //Random colour algorithm:
    int oldcol; //It denotes the previous colours of the LEDs: 1 = r ; 2 = b ; 3 = g.

    //We check if the music is stopped by checking if sum = 0. Also check if we already change colour by checking stop_col = 0.
    if (sum == 0 && stop_col == 0){
      stop_col = 1; //To prevent the colour kept changing when the music is stop for longer period.
      oldcol = colour;
      colour = rand() % 3 + 1; //Random new colour.
        if(oldcol == colour) colour = (colour + 1)%3; //Assigning new colour if randomed the same old colour.
    }
    else if (sum > 0) stop_col=0; //Reset stop_col = 0 to allow change in colour when the music next stopped.

    //Sending data to the LEDs strip.
    strip.show();
    strip2.show();
    strip3.show();
    strip4.show();
    strip5.show();

}
