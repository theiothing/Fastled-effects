#include "FastLED.h"                                          // FastLED library.

#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif


/************ FASTLED ******************/

// Fixed definitions cannot change on the fly.
#define LED_DT D5                                             // Serial data pin
#define LED_CK D6                                             // Serial clock pin for APA102 or WS2801
#define COLOR_ORDER BGR                                       // It's GRB for WS2812B
#define LED_TYPE APA102                                       // What kind of strip are you using (APA102, WS2801 or WS2812B
#define NUM_LEDS 24                                           // Number of LED's
#define MAX_BRIGHT 254                                        // Overall brightness definition. It can be changed on the fly.
#define LOOP_DURATION 15

struct CRGB leds[NUM_LEDS];                                   // Initialize our LED array.

CRGBPalette16 currentPalette = RainbowColors_p;
TBlendType    currentBlending = LINEARBLEND;
byte          colorIndex = 0;
int updatePerSeconds = 30;
unsigned long previousMillis = 0;

DEFINE_GRADIENT_PALETTE( heatmap_gp ) {
  0,     0,  0,  0,   //black
  128,   255,  0,  0,   //red
  224,   255, 255,  0,  //bright yellow
  255,   255, 255, 255
}; //heatmap

// Define variables used by the sequences.
uint8_t  thisfade = 8;                                        // How quickly does it fade? Lower = slower fade rate.
int       thishue = 50;                                       // Starting hue.
uint8_t   thisinc = 1;                                        // Incremental value for rotating hues
uint8_t   thissat = 100;                                      // The saturation, where 255 = brilliant colours.
uint8_t   thisbri = 255;                                      // Brightness of a sequence. Remember, max_bright is the overall limiter.
int       huediff = 256;                                      // Range of random #'s to use for hue
uint8_t thisdelay = 20;                                        // We don't need much delay (if any)

uint8_t gHue = 0;

void setup() {
  delay(200);
  //LEDS.addLeds<LED_TYPE, LED_DT, COLOR_ORDER>(leds, NUM_LEDS);       // Use this for WS2812B
  LEDS.addLeds<LED_TYPE, LED_DT, LED_CK, COLOR_ORDER>(leds, NUM_LEDS); // Use this for APA102 or WS2801

  FastLED.setBrightness(MAX_BRIGHT);
  set_max_power_in_volts_and_milliamps(5, 1000);
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
  for (byte i=0; i<NUM_LEDS; i++){
    leds[i] = CHSV(gHue, 255, 255);
    gHue += 254/NUM_LEDS;
    FastLED.show();
    delay(50);
  }

  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();

}//setup

void loop() {

  //ChangeMe();
  
  EVERY_N_MILLISECONDS(thisdelay) {                           // FastLED based non-blocking delay to update/display the sequence.
    sinelon();
    gHue++;
  }
  FastLED.show();
  
} // loop()


void sinelon()
{
  // a colored dot sweeping 
  // back and forth, with 
  // fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16(13,0,NUM_LEDS);
  static int prevpos = 0;
  if( pos < prevpos ) { 
    fill_solid( leds+pos, (prevpos-pos)+1, CHSV(gHue,220,255));
  } else { 
    fill_solid( leds+prevpos, (pos-prevpos)+1, CHSV( gHue,220,255));
  }
  prevpos = pos;
}


void confetti() {                                             // random colored speckles that blink in and fade smoothly
  fadeToBlackBy(leds, NUM_LEDS, thisfade);                    // Low values = slower fade.
  int pos = random16(NUM_LEDS);                               // Pick an LED at random.
  leds[pos] += CHSV((thishue + random16(huediff)) / 4 , thissat, thisbri); // I use 12 bits for hue so that the hue increment isn't too quick.
  thishue = thishue + thisinc;                                // It increments here.
} // confetti()

void ChangeMe() {                                             // A time (rather than loop) based demo sequencer. This gives us full control over the length of each sequence.
  uint8_t secondHand = (millis() / 1000) % LOOP_DURATION;                // IMPORTANT!!! Change '15' to a different value to change duration of the loop.
  static uint8_t lastSecond = 99;                             // Static variable, means it's only defined once. This is our 'debounce' variable.
  if (lastSecond != secondHand) {                             // Debounce to make sure we're not repeating an assignment.
    lastSecond = secondHand;
    switch (secondHand) {
      case  0: thisinc = 1; thishue = 192; thissat = 255; thisfade = 2; huediff = 256; break; // You can change values here, one at a time , or altogether.
      case  5: thisinc = 2; thishue = 128; thisfade = 8; huediff = 64; break;
      case 10: thisinc = 1; thishue = random16(255); thisfade = 1; huediff = 16; break; // Only gets called once, and not continuously for the next several seconds. Therefore, no rainbows.
      case LOOP_DURATION: break;                                                                // Here's the matching 15 for the other one.
    }
  }
} // ChangeMe()
/****** END OF FILE ******/
