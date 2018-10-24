#define FASTLED_INTERRUPT_RETRY_COUNT 0
#define FASTLED_ALLOW_INTERRUPTS 0
#define FASTLED_ESP8266_NODEMCU_PIN_ORDER
#define INTERRUPT_THRESHOLD 1
#define FASTLED_INTERNAL
#include "FastLED.h"
#include "font5x7.h"
#include <ESP8266WiFi.h>

FASTLED_USING_NAMESPACE













int lower(char c) {
    int a = (int) c;
    if ((a >= 65) && (a <= 90))
        a = a + 32;
    return a;
}

short * stringToArray(char c) {
    int rep = lower(c);

    switch (rep) {
      case 33: return exclamation;
      case 97:  return a_font5x7;
      case 98:  return b_font5x7;
      case 99:  return c_font5x7;
      case 100: return d_font5x7;
      case 101: return e_font5x7;
      case 102: return f_font5x7;
      case 103: return g_font5x7;
      case 104: return h_font5x7;
      case 105: return i_font5x7;
      case 106: return j_font5x7;
      case 107: return k_font5x7;
      case 108: return l_font5x7;
      case 109: return m_font5x7;
      case 110: return n_font5x7;
      case 111: return o_font5x7;
      case 112: return p_font5x7;
      case 113: return q_font5x7;
      case 114: return r_font5x7;
      case 115: return s_font5x7;
      case 116: return t_font5x7;
      case 117: return u_font5x7;
      case 118: return v_font5x7;
      case 119: return w_font5x7;
      case 120: return x_font5x7;
      case 121: return y_font5x7;
      case 122: return z_font5x7;
      default:  return blank_font5x7;
    }
}
















#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    3
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define BRIGHTNESS  95
#define GRID_WIDTH  32
#define GRID_HEIGHT 16
#define NUM_LEDS    512

CRGB leds[NUM_LEDS];
int FRAMES_PER_SECOND = 10;

// wifi credentials
 const char* ssid = "BronzeMaple(2.4GHz)\0";
 const char* password = "these are not the droids you're looking for\0";
//const char* ssid = "AndroidAP-jp\0";
//const char* password = "id384478\0";

// Set web server port number to 80
WiFiServer server(8080);
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

// Variable to store the HTTP request
String header;
char currentPattern = 0;
int displayIndex = 0;
int displaySubIndex = 0;

const char* display = "hey erik and karren  \0";

void setup() {
  delay(3000); // 3 second delay for recovery

  // setup wifi connection
  Serial.begin(9600);
  Serial.println();
  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // default design
  setJustin2018();
  FastLED.setBrightness(10);
}

void drawGrid(int x, int y, short *character, CRGB color) {

  // Serial.println();
  for(int i = 0; i < 7; i++ ){
    for(int j = 0; j < 5; j++ ) {
      // Serial.print( *( character + (i * 5) + j));
      if ((*( character + (i * 5) + j)) == 1)
        leds[coordToInt(j + x, y - i)] = color;
    }
    // Serial.println();
  }
  // Serial.println();
}
short drawGridPixel(short *character, int sub, int y) {
  return (*( character + (y * 5) + sub));
}
// void drawGrid(int x, int y, short character[7][5], CRGB color) {
//   for(int i = 0; i < 7; i++ ){
//     for(int j = 0; j < 5; j++ ) {
//       if (character[i][j] == 1)
//         leds[coordToInt(j + x, y - i)] = color;
//     }
//   }
// }
int intToCoordY(int i) {
  return i/GRID_WIDTH;
}
int intToCoorX(int i, int y) {

  if (y%2 == 0) {
    return  i%GRID_WIDTH;
  } else {
    return  GRID_WIDTH - (i%GRID_WIDTH);
  }
}
int intToCoorX(int i) {
  int y = i/GRID_WIDTH;

  if (y%2 == 0) {
    return  i%GRID_WIDTH;
  } else {
    return  GRID_WIDTH - (i%GRID_WIDTH);
  }
}
int coordToInt(int x, int y) {
  if (y%2==0) {
     return y*GRID_WIDTH + x;
  } else {
    return y*GRID_WIDTH + (GRID_WIDTH - x) - 1;
  }
}
void slideRow(int y) {
  CRGB temp = leds[coordToInt(0, y)];
  for (int i = 0; i < GRID_WIDTH - 1; i++) {
    leds[coordToInt(i, y)] = leds[coordToInt(i + 1, y)];
  }
  leds[coordToInt(GRID_WIDTH - 1, y)] = temp;
}
void modifiedSlideRow(int y, int ciel) {
  for (int i = 0; i < GRID_WIDTH - 1; i++) {
    leds[coordToInt(i, y)] = leds[coordToInt(i + 1, y)];
  }
  if(displaySubIndex < 5 && drawGridPixel(stringToArray(display[displayIndex]), displaySubIndex, ciel - y)) {
    leds[coordToInt(GRID_WIDTH - 1, y)] = CRGB::Red;
  } else {
    leds[coordToInt(GRID_WIDTH - 1, y)] = CRGB::Black;
  }
}

void loop()
{
  WiFiClient client = server.available();

  if (client && client.connected()) {
    // Read the first line of the request
    String req = client.readStringUntil('\r');
    Serial.println(req);
    client.flush();

    if (req.indexOf("/theta ") != -1) {
      currentPattern = 1;
      thetachi2018();
    } else if (req.indexOf("/thetaNoScroll ") != -1) {
      currentPattern = 2;
      thetachi2018();
    } else if (req.indexOf("/justin ") != -1) {
      currentPattern = 0;
      setJustin2018();
    } else if (req.indexOf("/justinNoScroll ") != -1) {
      currentPattern = 3;
      setJustin2018();
    } else if (req.indexOf("/rainbow ") != -1) {
      currentPattern = 11;
    } else if (req.indexOf("/rainbowWithGlitter ") != -1) {
      currentPattern = 12;
    } else if (req.indexOf("/confetti ") != -1) {
      currentPattern = 13;
    } else if (req.indexOf("/sinelon ") != -1) {
      currentPattern = 14;
    } else if (req.indexOf("/juggle ") != -1) {
      currentPattern = 15;
    } else if (req.indexOf("/bpm ") != -1) {
      currentPattern = 16;
    } else if (req.indexOf("/utep2018NoScroll ") != -1) {
      currentPattern = 4;
      utep2018();
    } else if (req.indexOf("/utep2018 ") != -1) {
      currentPattern = 5;
      utep2018();
    } else if (req.indexOf("/10fps ") != -1) {
      FRAMES_PER_SECOND = 10;
    } else if (req.indexOf("/30fps ") != -1) {
      FRAMES_PER_SECOND = 30;
    } else if (req.indexOf("/60fps ") != -1) {
      FRAMES_PER_SECOND = 60;
    } else if (req.indexOf("/120fps ") != -1) {
      FRAMES_PER_SECOND = 120;
    } else if (req.indexOf("/fullwhite ") != -1) {
      currentPattern = 6;
      fullWhite();
    } else if (req.indexOf("/10bright ") != -1) {
      FastLED.setBrightness(10);
    } else if (req.indexOf("/30bright ") != -1) {
      FastLED.setBrightness(30);
    } else if (req.indexOf("/50bright ") != -1) {
      FastLED.setBrightness(40);
    } else if (req.indexOf("/70bright ") != -1) {
      FastLED.setBrightness(70);
    } else if (req.indexOf("/90bright ") != -1) {
      FastLED.setBrightness(95);
    } else if (req.indexOf("/fullblack ") != -1) {
      currentPattern = 7;
      fullblack();
    }

    client.print("HTTP/1.1 200 OK\r\nContent-type:text/html\r\n\r\n\r\n<!DOCTYPE html><html><head> <link rel=\"icon\" href=\"data:;base64,iVBORw0KGgo=\"> <title>Hat Software!</title></head><body> Use \"Justin 2018\" design: <a href=\"/justin\"> <button>USE DESIGN</button> </a> </br> </br> Use \"Justin 2018\" No Scroll design: <a href=\"/justinNoScroll\"> <button>USE DESIGN</button> </a> </br> </br> Use \"Theta Chi 2018\" design: <a href=\"/theta\"> <button>USE DESIGN</button> </a> </br> </br> Use \"Theta Chi 2018\" No Scroll design: <a href=\"/thetaNoScroll\"> <button>USE DESIGN</button> </a> </br> </br> Use \"UTEP 2018\" No Scroll design: <a href=\"/utep2018NoScroll\"> <button>USE DESIGN</button> </a> </br> </br> Use \"UTEP 2018\" design: <a href=\"/utep2018\"> <button>USE DESIGN</button> </a> </br> </br> Use \"Full White\" design: <a href=\"/fullwhite\"> <button>USE DESIGN</button> </a> </br> </br> Use \"Full Black\" design: <a href=\"/fullblack\"> <button>USE DESIGN</button> </a> </br> </br> Use \"rainbow\" design: <a href=\"/rainbow\"> <button>USE DESIGN</button> </a> </br> </br> Use \"rainbowWithGlitter\" design: <a href=\"/rainbowWithGlitter\"> <button>USE DESIGN</button> </a> </br> </br> Use \"confetti\" design: <a href=\"/confetti\"> <button>USE DESIGN</button> </a> </br> </br> Use \"sinelon\" design: <a href=\"/sinelon\"> <button>USE DESIGN</button> </a> </br> </br> Use \"juggle\" design: <a href=\"/juggle\"> <button>USE DESIGN</button> </a> </br> </br> Use \"bpm\" design: <a href=\"/bpm\"> <button>USE DESIGN</button> </a> </br> </br> </br> </br> </br> </br> </br> </br> </br> </br> </br> </br> Use \"10fps\": <a href=\"/10fps\"> <button>USE DESIGN</button> </a> Use \"30fps\": <a href=\"/30fps\"> <button>USE DESIGN</button> </a> Use \"60fps\": <a href=\"/60fps\"> <button>USE DESIGN</button> </a> Use \"120fps\": <a href=\"/120fps\"> <button>USE DESIGN</button> </a> </br> </br> </br> </br> </br> </br> </br> </br> </br> </br> </br> </br> Use \"10bright\": <a href=\"/10bright\"> <button>USE DESIGN</button> </a> Use \"30bright\": <a href=\"/30bright\"> <button>USE DESIGN</button> </a> Use \"50bright\": <a href=\"/50bright\"> <button>USE DESIGN</button> </a> Use \"70bright\": <a href=\"/70bright\"> <button>USE DESIGN</button> </a> Use \"90bright\": <a href=\"/90bright\"> <button>USE DESIGN</button> </a></body></html>");
    client.println();
  }
//  Keep your eyes on the fries

  if (currentPattern == 3) {
    setJustin2018();
  } else if (currentPattern == 2) {
    thetachi2018();
  } else if (currentPattern == 11) {
    rainbow();
  } else if (currentPattern == 12) {
    rainbowWithGlitter();
  } else if (currentPattern == 13) {
    confetti();
  } else if (currentPattern == 14) {
    sinelon();
  } else if (currentPattern == 15) {
    juggle();
  } else if (currentPattern == 16) {
    bpm();
  } else if (currentPattern == 4) {
    utep2018();
  } else if (currentPattern == 6) {
    fullWhite();
  } else if (currentPattern == 7) {
    fullblack();
  }
  FastLED.show();
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND);

  if (currentPattern == 0 || currentPattern == 1 || currentPattern == 5) {
    slideRow(1);
    slideRow(2);
    slideRow(3);
    slideRow(4);
    slideRow(5);
    slideRow(6);
    slideRow(7);
    modifiedSlideRow(8, 14);
    modifiedSlideRow(9, 14);
    modifiedSlideRow(10, 14);
    modifiedSlideRow(11, 14);
    modifiedSlideRow(12, 14);
    modifiedSlideRow(13, 14);
    modifiedSlideRow(14, 14);

    // Serial.println(displayIndex);
    // Serial.println(displaySubIndex);
    // Serial.println();

    displaySubIndex += 1;

    if (displaySubIndex >= 6) {
      displayIndex++;
      displaySubIndex = 0;
    }

    if (displayIndex >= 21) {
      displayIndex = 0;
    }

  }


  // do some periodic updates
  if (currentPattern > 10) {
    EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbowy
  }
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

//void nextPattern()
//{
//  // add one to the current pattern number, and wrap around at the end
//  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
//}
void setJustin2018() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  drawGrid(0, 14, stringToArray('j'), CRGB::Blue);
  drawGrid(5, 14, u_font5x7, CRGB::Red);
  drawGrid(10, 14, s_font5x7, CRGB::Yellow);
  drawGrid(15, 14, t_font5x7, CRGB::Blue);
  drawGrid(20, 14, i_font5x7, CRGB::Green);
  drawGrid(25, 14, n_font5x7, CRGB::Red);

  drawGrid(0, 7, d_2, CRGB::Red);
  drawGrid(7, 7, d_0, CRGB::Red);
  drawGrid(14, 7, d_1, CRGB::Red);
  drawGrid(21, 7, d_8, CRGB::Red);
}
void thetachi2018() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  drawGrid(0, 14, g_o, CRGB::Red);
  drawGrid(6, 14, x_font5x7, CRGB::Red);
  drawGrid(21, 14, g_o, CRGB::Red);
  drawGrid(27, 14,x_font5x7, CRGB::Red);

  drawGrid(4, 7, d_2, CRGB::Blue);
  drawGrid(10, 7, d_0, CRGB::Blue);
  drawGrid(16, 7, d_1, CRGB::Blue);
  drawGrid(22, 7, d_8, CRGB::Blue);
}
void utep2018() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  drawGrid(4, 14, u_font5x7, CRGB::Orange);
  drawGrid(10, 14, t_font5x7, CRGB::Orange);
  drawGrid(16, 14, e_font5x7, CRGB::Orange);
  drawGrid(22, 14, p_font5x7, CRGB::Orange);

  drawGrid(4, 7, d_2,  CRGB::Orange);
  drawGrid(10, 7, d_0, CRGB::Orange);
  drawGrid(16, 7, d_1, CRGB::Orange);
  drawGrid(22, 7, d_8, CRGB::Orange);
}
void fullWhite() {
  fill_solid(leds, NUM_LEDS, CRGB::White);
}
void fullblack() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
}
void rainbow()
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter()
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter)
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti()
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}
