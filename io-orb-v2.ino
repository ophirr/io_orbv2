

/**************************************************************
 * Blynk is a platform with iOS and Android apps to control
 * Arduino, Raspberry Pi and the likes over the Internet.
 * You can easily build graphic interfaces for all your
 * projects by simply dragging and dropping widgets.
 *
 *   Downloads, docs, tutorials: http://www.blynk.cc
 *   Blynk community:            http://community.blynk.cc
 *   Social networks:            http://www.fb.com/blynkapp
 *                               http://twitter.com/blynk_app
 *
 * Blynk library is licensed under MIT license
 * This example code is in public domain.
 *
 **************************************************************
 * Control a color gradient on NeoPixel strip using a slider!
 *
 * For this example you need NeoPixel library:
 *   https://github.com/adafruit/Adafruit_NeoPixel
 *
 * App dashboard setup:
 *   Slider widget (0...500) on V1
 *
 **************************************************************/

#define BLYNK_PRINT Serial
//#include <Adafruit_ESP8266.h>
#include <BlynkSimpleEsp8266.h>


#include <Adafruit_NeoPixel.h>

#define LEDPIN 5
#define CONNECTPIN 5

#define DEBUG 

#ifdef DEBUG
 #define DEBUG_PRINT(x)  Serial.print (x)
 #define DEBUG_PRINTLN(x)  Serial.println (x)
#else
 #define DEBUG_PRINT(x)
 #define DEBUG_PRINTLN(x) 
#endif

#define PIXEL_COUNT 24

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, LEDPIN, NEO_GRB + NEO_KHZ800);

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "4a272b1812cd4e38abd79edf460474b1";

// timer 
unsigned long currentMillis;
unsigned long previousMillis; // will store last time pixel was updated
unsigned long main_currentMillis; // store last main loop time
int seconds; //seconds count in the timer

int wait = 15;


// rainbow
int rainbow_wait = 5;
 int neoPixelToChange = 0; //track which neoPixel to change
int neoPixel_j = 0; //stores values for program cycles
int rainbow_color = 0; // stores rainbow color

uint32_t currentColor;// current Color in case we need it
uint16_t currentPixel = 0;// what pixel are we operating on

uint32_t magenta = strip.Color(255, 0, 255);
int showType = 0;



void setup()
{



  Serial.begin(9600);
  // Blynk.begin(auth, "ShangriTizo 2.4Ghz", "Tizoville");
  Blynk.begin(auth, "ShangriTizo Upstairs", "Tizoville");

  currentColor = strip.Color(255,0,0);
  currentPixel = 0;  
  strip.begin();
  strip.show();
  pinMode(CONNECTPIN, OUTPUT);
}


 // Keep this flag not to re-sync on every reconnection
 bool isFirstConnect = true;
 
BLYNK_CONNECTED() {
    if (isFirstConnect) {
      Blynk.syncAll();
      digitalWrite(CONNECTPIN, HIGH);
      isFirstConnect = false;
    }
    // Let's write your hardware uptime to Virtual Pin 2
    int value = millis() / 1000;
    Blynk.virtualWrite(V5, value);
}

BLYNK_WRITE(V0)
{
  int power = param.asInt();
  
  DEBUG_PRINT("Power: ");
  DEBUG_PRINTLN(power);
  
  if(power == 0) {
    for (int i = 0;i < strip.numPixels(); i++)
    {
      strip.setBrightness(0);
      DEBUG_PRINT("Pixel ");
      DEBUG_PRINT(i);
      DEBUG_PRINTLN(": Brightness set to 0 (Off)");
    }
  } else {
   
    for (int i = 0; i < strip.numPixels(); i++)
    {
      strip.setPixelColor(i, magenta);
      DEBUG_PRINT("Pixel ");
      DEBUG_PRINT(i);
      DEBUG_PRINTLN(": Brightness set to 1 (On)");
    }
    strip.setBrightness(150);
  }
  strip.show();
}

BLYNK_WRITE(V1)
{
  int shift = param.asInt();
  
  DEBUG_PRINT("Color: ");
  DEBUG_PRINTLN(shift);

  neoPixel_j = shift;
  
//  
//  for (int i = 0; i < strip.numPixels(); i++)
//  {
//    strip.setPixelColor(i, Wheel(shift & 255));
//  }
//  strip.show();
}

BLYNK_WRITE(V2)
{
  int brightness = param.asInt();

  strip.setBrightness(brightness);
  strip.show();

  DEBUG_PRINT("Brightness: ");
  DEBUG_PRINTLN(brightness);
  
}

BLYNK_WRITE(V3)
{
  int mode = param.asInt();

  DEBUG_PRINT("Mode: ");
  DEBUG_PRINTLN(mode);

  showType = mode;
}

BLYNK_WRITE(V4)
{
  int speed = param.asInt();

  DEBUG_PRINT("Speed: ");
  DEBUG_PRINTLN(speed);

 //  = speed;

}



void startShow(int i) {
  switch(i){
   // case 0: colorSet(strip.Color(0, 0, 0), wait);    // Black/off
   //         break;
    case 0: colorSet(neoPixel_j, wait);
            break;
    case 1: 
            rainbowCycle(rainbow_wait);  
            break;
    case 2: rainbow(rainbow_wait);
            break;
  }
}

void loop()
{
  Blynk.run();
  startShow(showType);
}



void rainbow(uint8_t wait) {
    
  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis > neoPixel_j) { // re-use pot value in time context
    
    // save the last time you changed a NeoPixel 
    previousMillis = currentMillis; 

    //change the colors of the pixels
    uint16_t i;

    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+rainbow_color) & 255));
    }
    strip.show();
    rainbow_color = (rainbow_color + 1) % 255; //increment j until all colors are used, then start over
  }
}

void colorSet(uint8_t color, uint8_t wait) {
    
  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis > wait) {
    
    // save the last time you changed a NeoPixel 
    previousMillis = currentMillis; 

    //change the colors of the pixels
    uint16_t i;
    //DEBUG_PRINT("In ColorSet: neoPixel_j = ");
    //DEBUG_PRINTLN(neoPixel_j);
    //DEBUG_PRINTLN();

    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(color));
    }
    strip.show();
   // neoPixel_j = (neoPixel_j + 1) % 255; //increment j until all colors are used, then start over
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  
  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis > wait) {
    
    // save the last time you changed a NeoPixel 
    previousMillis = currentMillis; 

    //change the colors of the pixels
    uint16_t i;

    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + neoPixel_j) & 255));
    }

    strip.show();

    neoPixel_j = (neoPixel_j + 1) % 1279; // 5 cycles of all colors on wheel, then start over
  }
}


// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
    for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }

}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if (WheelPos < 85) {
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
    WheelPos -= 170;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}





