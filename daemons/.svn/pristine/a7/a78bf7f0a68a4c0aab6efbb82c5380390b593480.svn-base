#include "FastLED.h"

#define POWER_DEBUG_PRINT 1

//#define NUM_LEDS 1
#define rows 8
#define columns 32
#define NUM_LEDS (rows*columns)
#define BRIGHTNESS 32

#define DATA_PIN 30

//#define CLOCK_PIN 13

// Define the array of leds
CRGB leds[NUM_LEDS];

String readString;
int ledState = LOW;             // ledState used to set the onboard warning LED

void setup() { 
    pinMode(13, OUTPUT); //Signal LED on the MEGA for debugging
    Serial.begin(115200);
     // FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
     // FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, NUM_LEDS);
     FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  	 // FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
     FastLED.setBrightness( BRIGHTNESS );
}

uint16_t XY( int p ){
  uint8_t x;
  uint8_t y;
  uint16_t i;

  x = p%columns;
  y = p/columns;
  
  //Even column
  if(x%2 == 0){
      // Even rows run forwards
      i = x*rows + y;
  }
  //odd column => move upwards
  else{
      i = x*rows + (rows-1-y);
  } 

  return i;
}

void loop() { 
  //readBuffer is 1 larger than data for start of packet header 0xAA
  unsigned char readBuf[769] = {0};

//  Serial.setTimeout(1000);
  Serial.readBytes(readBuf, 769);
  int loopCount = 0;
  int packetStartIndex = -1;
  unsigned char ledBuf[768] = {0};

  //Find start of message
  for(int i=0;i<769;i++){
    if((int)readBuf[i] == 170/*0xAA*/){
      packetStartIndex = i+1;
      break;
    }
  }
  if(packetStartIndex == -1){
    //No begin flag => Error, invalid message; blink onboard LED to signal
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
    digitalWrite(13, ledState);
    return;
  }
  
  //copy the image data into the led array
  //data begins at previously found start-of-message flag, count around from there
  for(int i=0;i<768;i++){
    if(packetStartIndex+i > 768){
      packetStartIndex = -i;
    }
    ledBuf[i] = readBuf[packetStartIndex+i];
  }

  
  //Set leds to the transmitted values.
  for(int i=0;i<NUM_LEDS;i++){
    leds[XY(i)].r = ledBuf[i*3];
    leds[XY(i)].g = ledBuf[i*3+1];
    leds[XY(i)].b = ledBuf[i*3+2];
  }

  //write the image matrix back as confirmation
  Serial.write(ledBuf, 768);

  FastLED.show();
  delay(50);
  // Now turn the LED off, then pause
  //leds[0] = CRGB::Black;
  //FastLED.show();
  //delay(500);
}
