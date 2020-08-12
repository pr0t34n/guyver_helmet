#include <Adafruit_NeoPixel.h>

// definitions
// pins
#define LED_PIN    6
#define BUTTON_PIN 5
// how many leds?
#define LED_COUNT 4

// declare rgb strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// test vars
bool go = true;                   // for shift cycling
bool needRandom = true;           // for random delay times
unsigned long delay_1 = 0;        // for storing random delay time
int breath = 1;                   // for scanner
int colorScheme = 0;              // which color scheme to use?
int colorSchemeMax = 3;           // how many total color schemes to use? (inclusive: colorSchemeMax=3 holds schemes in 0,1,2,3

// timer stuff (for 'multitasking')
unsigned long currentMillis = 0;  // holds current time at start of loop()

// define length button needs to be held down to do stuff
int longPressTime = 2000;
int shortPressTime = 50;

// button controls
int buttonRead;                  // to hold temporary read value
int previousButtonRead = LOW;    // holds semi-permanent button val
int buttonCounter = 0;           // how many short presses?
bool buttonLongPress = false;    // is the button press a long press?
bool buttonShortPress = false;   // is the button press a short press?
bool buttonActive = false;       // is the button being held down?
unsigned long buttonTimer = 0;   // when was the button pressed?
int buttonCounterMax = 6;        // how many patterns to store? (inclusive. i.e, buttonCounterMax=3 actually stores 4 patterns: 0,1,2,3)

// define colors for control medal ring ------------------------------------
// pointer blanks for scheming
int *colorOne;
int *colorTwo;

// primary colors
int red[] = {255, 0, 0};
int green[] = {0, 255, 0};
int blue[] = {0, 0, 255};

int dark_red[] = {128, 0, 0};
int dark_green[] = {0, 128, 0};
int dark_blue[] = {0, 0, 128};

// standard mixes
int aqua[] = {0, 255, 255};
int violet[] = {255, 0, 255};
int yellow[] = {255, 255, 0};

// white and half and blank (full and half and off)
int white[] = {255, 255, 255};
int half_white[] = {127, 127, 127};
int blank[] = {0, 0, 0};

// custom mixes
int warm[] = {200, 70, 0}; // a nice control medal warm fuzzy glow
int cool[] = {50, 160, 240}; // blueish-grey, very soft and pleasing
int pink[] = {255, 45, 100}; // a pastely pink
int cool_green[] = {204, 170, 46}; // soft creamy green
int lawn_green[] = {125, 252, 0}; // bright and cheery
int orchid[] = {153, 51, 204}; // a pale purpose
int fleshy[] = {247, 117, 48}; // weird skin color
int orange[] = {237, 64, 0}; // a solid starting point for orange
int gummy_red[] = {227, 23, 13}; // like a knock-off swedish fish
int blood_orange[] = {204, 18, 0}; // soft reddish orange
int light_teal[] = {0, 255, 204}; // really more like marine blue

// testing custom colors ---------------------------------------------------
// for testing pcts. this is a great table of what you should approx. expect: http://www.downtownuplighting.com/rgb-color-chart
float r_pct = 0.8;
float g_pct = 0.07;
float b_pct = 0;

int test_color[] = {r_pct * 255, g_pct * 255, b_pct * 255}; //test pcts here
//int test_color[] = {204,18,0}; //test specific color combos here
// -------------------------------------------------------------------------

void setup() {
  // logging
  Serial.begin(9600);

  // set button pin as input
  pinMode(BUTTON_PIN, INPUT);

  // initialize RGB strip
  strip.begin();           // INITIALIZE rgb strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(255); // Set BRIGHTNESS (max = 255)
}

void loop() {
  currentMillis = millis(); // timestamp at beginning of loop
  checkButton();
  
  // most patterns take two colors. color1 is a base color, where color2 is a highlight
  // some patterns take three colors. in that case, blank is usually used as 'color' 0
  if (colorScheme==0){
    colorOne = dark_blue;
    colorTwo = aqua;
  }
  else if (colorScheme==1){
    colorOne = dark_red;
    colorTwo = warm;
  }
  else if (colorScheme==2){
    colorOne = dark_green;
    colorTwo = lawn_green;
  }
  else if (colorScheme==3){
    colorOne = blood_orange;
    colorTwo = violet;
  }

  // for testing a color, will just show the color
  //showColor(test_color);

  // // basic rainbow, each pixel shifts through all colors individually
  // argument: delay (in ms)
  //rainbow(10);

  // circle
  // spins around like a cdrom reading
  // arguments: color, delay (ms)
  //circle(aqua,40);

  // glitch
  // randomly blips colors and/or fades to black
  // arguments: color1, color2, delay min time, delay max time
  // color2 should be the highlight color and color1 is the dark color
  //glitch(dark_red,orange,10,300);
  //glitch(aqua,dark_blue,10,500);

  // slow/med/fastPulse
  // fades from color1 to color2, holds on color2 longer
  // arguments: color1 (highlight), color2 (base)
  //slowPulse(warm,blank);
  //medPulse(white,lawn_green);
  //fastPulse(cool,red);

  // beacon
  // two fade-in blips from color2 to color1, then a slight delay
  // arguments: color1 (highlight), color2 (base), wait time between beacons
  //beacon(warm,blank,1500);

  // comms
  // randomly fades in and holds base color (color2)
  // arguments: color1 (base), color2 (highlight)
  //controlMedalComms(dark_blue,aqua);

  // crazyrainbow
  // loops through all colors with focus on 'difficult' colors, like orange. all pixels change at the same time
  // arguments: none
  //crazyRainbow();

  // sparkle
  // chooses random pixels, lights them to a random brightness with input colors as a guide
  // arguments: color1, color2, deadPixel (true/false)
  // if deadPixel is true, one random pixel will be off each loop, the result is a more 'stormy' look
  //sparkle(dark_blue,white,true,50);

  // scanner
  // moves a highlight (color3) through a transition color (color2) and fades to base (color1)
  // arguments: color1 (base), color2 (middle), color3 (highlight), delay (in ms), direction (v (down to up), V (up-to-down), h (right-to-left), H (left-to-right)
  //scanner(blank, dark_blue, white, 3, 'v');
  //scanner(blank, dark_blue, white, 3, 'V');
  //scanner(blank, dark_blue, white, 3, 'h');
  //scanner(blank, dark_blue, white, 3, 'H');
  
  if (buttonCounter == 0)
    medPulse(colorOne, colorTwo);
  else if (buttonCounter == 1)
    beacon(colorOne, colorTwo, 1500);
  else if (buttonCounter == 2)
    glitch(colorOne, colorTwo, 10, 300);
  else if (buttonCounter == 3)
    scanner(blank, colorOne, colorTwo, 3, 'H');
  else if (buttonCounter == 4)
    controlMedalComms(colorOne, colorTwo);
  else if (buttonCounter == 5)
    sparkle(colorOne, colorTwo, true, 50);
  else
    showColor(blank);
}

void scanner(int color_1[], int color_2[], int color_3[], int delaytime, char dir) {
  checkButton();
  int i_max = 100;
  int rc[] = {0, 0, 0, 0};

  if (dir == 'v') {
    rc[0] = 0;
    rc[1] = 3;
    rc[2] = 1;
    rc[3] = 2;
  } else if (dir == 'h') {
    rc[0] = 3;
    rc[1] = 2;
    rc[2] = 0;
    rc[3] = 1;
  } else if (dir == 'V') {
    rc[0] = 1;
    rc[1] = 2;
    rc[2] = 3;
    rc[3] = 0;
  } else if (dir == 'H') {
    rc[0] = 1;
    rc[1] = 0;
    rc[2] = 3;
    rc[3] = 2;
  }
  
  // breath 1; upper row goes from blank to full
  // lower row goes from full to max
  if (breath == 1){
    for (int i = 0; i < i_max; i++) {
      checkButton();
      int r_1 = map(i, 0, i_max, color_1[0], color_2[0]);
      int g_1 = map(i, 0, i_max, color_1[1], color_2[1]);
      int b_1 = map(i, 0, i_max, color_1[2], color_2[2]);
  
      int r_2 = map(i, 0, i_max, color_2[0], color_3[0]);
      int g_2 = map(i, 0, i_max, color_2[1], color_3[1]);
      int b_2 = map(i, 0, i_max, color_2[2], color_3[2]);
  
      displayRGB(r_1, g_1, b_1, rc[0], false);
      displayRGB(r_1, g_1, b_1, rc[1], false);
      displayRGB(r_2, g_2, b_2, rc[2], false);
      displayRGB(r_2, g_2, b_2, rc[3], false);
      strip.show();
      
      delay(delaytime);
    }
    breath++;
  }

  // breath 2: upper row goes from full to max
  // lower row goes from max to full
  if (breath == 2){
    for (int i = 0; i < i_max; i++) {
      checkButton();
      int r_1 = map(i, 0, i_max, color_2[0], color_3[0]);
      int g_1 = map(i, 0, i_max, color_2[1], color_3[1]);
      int b_1 = map(i, 0, i_max, color_2[2], color_3[2]);
  
      int r_2 = map(i, 0, i_max, color_3[0], color_2[0]);
      int g_2 = map(i, 0, i_max, color_3[1], color_2[1]);
      int b_2 = map(i, 0, i_max, color_3[2], color_2[2]);
  
      displayRGB(r_1, g_1, b_1, rc[0], false);
      displayRGB(r_1, g_1, b_1, rc[1], false);
      displayRGB(r_2, g_2, b_2, rc[2], false);
      displayRGB(r_2, g_2, b_2, rc[3], false);
      strip.show();
      
      delay(delaytime);
    }
    breath++;
  }

  // breath 3: upper row goes from max to full
  // lower row goes from full to blank
  if (breath == 3){
    for (int i = 0; i < i_max; i++) {
      checkButton();
      int r_1 = map(i, 0, i_max, color_3[0], color_2[0]);
      int g_1 = map(i, 0, i_max, color_3[1], color_2[1]);
      int b_1 = map(i, 0, i_max, color_3[2], color_2[2]);
  
      int r_2 = map(i, 0, i_max, color_2[0], color_1[0]);
      int g_2 = map(i, 0, i_max, color_2[1], color_1[1]);
      int b_2 = map(i, 0, i_max, color_2[2], color_1[2]);
  
      displayRGB(r_1, g_1, b_1, rc[0], false);
      displayRGB(r_1, g_1, b_1, rc[1], false);
      displayRGB(r_2, g_2, b_2, rc[2], false);
      displayRGB(r_2, g_2, b_2, rc[3], false);
      strip.show();
      
      delay(delaytime);
    }
    breath++;
  }

  // breath 4: upper row goes from full to blank
  // lower row stays blank
  if (breath == 4){
    for (int i = 0; i < i_max; i++) {
      checkButton();
      int r_1 = map(i, 0, i_max, color_2[0], color_1[0]);
      int g_1 = map(i, 0, i_max, color_2[1], color_1[1]);
      int b_1 = map(i, 0, i_max, color_2[2], color_1[2]);
  
      int r_2 = map(i, 0, i_max, color_1[0], color_2[0]);
      int g_2 = map(i, 0, i_max, color_1[1], color_2[1]);
      int b_2 = map(i, 0, i_max, color_1[2], color_2[2]);
  
      displayRGB(r_1, g_1, b_1, rc[0], false);
      displayRGB(r_1, g_1, b_1, rc[1], false);
      //displayRGB(r_2,g_2,b_2,rc[2],false);
      //displayRGB(r_2,g_2,b_2,rc[3],false);
      strip.show();
      
      delay(delaytime);
    }
    breath++;
  }
  
  // breath 5: show color1 for full cycle, to avoid using a true delay
  if (breath == 5){
    for (int i = 0; i < i_max; i++) {
      checkButton();
      showColor(color_1);
      strip.show();
      delay(delaytime);
    }
    breath++;
  }
  
  // breath 6: upper row stays blank
  // lower row goes from blank to full
  if (breath == 6){
    for (int i = 0; i < i_max; i++) {
      checkButton();
      int r_1 = map(i, 0, i_max, color_2[0], color_1[0]);
      int g_1 = map(i, 0, i_max, color_2[1], color_1[1]);
      int b_1 = map(i, 0, i_max, color_2[2], color_1[2]);
  
      int r_2 = map(i, 0, i_max, color_1[0], color_2[0]);
      int g_2 = map(i, 0, i_max, color_1[1], color_2[1]);
      int b_2 = map(i, 0, i_max, color_1[2], color_2[2]);
  
      //displayRGB(r_1,g_1,b_1,rc[0],false);
      //displayRGB(r_1,g_1,b_1,rc[1],false);
      displayRGB(r_2, g_2, b_2, rc[2], false);
      displayRGB(r_2, g_2, b_2, rc[3], false);
      strip.show();
      
      delay(delaytime);
    }
    breath = 1;
  }
}

void sparkle(int color_1[], int color_2[], bool deadOne, int waitTime) {
  checkButton();
  if (millis() >= delay_1){
    // pick a random pixel to work with
    int i = random(0, 4);
    
    int c1_r, c2_r = 0;
    int c1_g, c2_g = 0;
    int c1_b, c2_b = 0;

    // generate random pct 1
    int randPct = random(1,100);
    c1_r = color_1[0] * randPct/100;
    c1_g = color_1[1] * randPct/100;
    c1_b = color_1[2] * randPct/100;

    // generate random pct 2
    randPct = random(1,100);
    c2_r = color_2[0] * randPct/100;
    c2_g = color_2[1] * randPct/100;
    c2_b = color_2[2] * randPct/100;
  
    // flip a coin to decide whether to use color1 or color2
    byte coin = random(0, 2);
    if (coin)
      strip.setPixelColor(i, strip.Color(c1_r, c1_g, c1_b));
    else
      strip.setPixelColor(i, strip.Color(c2_r, c2_g, c2_b));
  
    // if deadPixel is set, assign it
    if (deadOne) {
      int deadPixel = random(0, 4);
      strip.setPixelColor(deadPixel, strip.Color(0, 0, 0));
    }
    strip.show();
    delay_1 = millis() + waitTime;
  }
}

void crazyRainbow() {
  int i_max = 100;
  int cycleTime = 10;
  int waitTime = 200;

  // rainbow with 'harder' colors specified explicitly
  shift(red,orange,cycleTime,i_max,true,waitTime);
  shift(orange,yellow,cycleTime,i_max,true,waitTime);
  shift(yellow,green,cycleTime,i_max,true,waitTime);
  shift(green,aqua,cycleTime,i_max,true,waitTime);
  shift(aqua,blue,cycleTime,i_max,true,waitTime);
  shift(blue,violet,cycleTime,i_max,true,waitTime);
  shift(violet,red,cycleTime,i_max,true,waitTime);
}

void controlMedalComms(int color_1[], int color_2) {
  checkButton();
  if (millis() >= delay_1){
    int cycleTime = random(3,7);
    int i_max = random(50,150);
    shift(color_1, color_2,cycleTime,i_max,false,0);
    shift(color_2, color_1,cycleTime,i_max,false,0);
    generateRandomDelay(100,1000);
  }
}

void beacon(int color1[], int color2[], int waitTime){
  checkButton();
  if (millis() >= delay_1){
    shift(blank,color1,5,60,false,0);
    shift(color1,color2,5,60,false,0);
    shift(color2,color1,5,60,false,0);
    shift(color1,blank,5,60,true,200);
    shift(blank,color1,5,60,false,0);
    shift(color1,color2,5,60,false,0);
    shift(color2,color1,5,60,false,0);
    shift(color1,blank,5,60,true,waitTime);
  }
}

void slowPulse(int color1[], int color2[]){
  checkButton();
  if (millis() >= delay_1){
    shift(blank,color1,10,100,false,0);
    shift(color1,color2,10,100,false,0);
    shift(color2,color1,10,100,false,0);
    shift(color1,blank,10,100,true,2000);
  }
}

void medPulse(int color1[], int color2[]){
  checkButton();
  if (millis() >= delay_1){
    shift(blank,color1,7,80,false,0);
    shift(color1,color2,7,80,false,0);
    shift(color2,color1,7,80,false,0);
    shift(color1,blank,7,80,true,1200);
  }
}

void fastPulse(int color1[], int color2[]){
  checkButton();
  if (millis() >= delay_1){
    shift(blank,color1,5,60,false,0);
    shift(color1,color2,5,60,false,0);
    shift(color2,color1,5,60,false,0);
    shift(color1,blank,5,60,true,800);
  }
}

void shift(int color1[], int color2[], int cycleTime, int i_max, bool wait, int waitTime){
  for (int i = 0; i<i_max; i++){
    checkButton();
    int r = map(i, 0, i_max, color1[0], color2[0]);
    int g = map(i, 0, i_max, color1[1], color2[1]);
    int b = map(i, 0, i_max, color1[2], color2[2]);

    int color[] = {r,g,b};
    
    showColor(color);
    strip.show();
    delay(cycleTime);
  }
  if (wait)
    delay_1 = millis() + waitTime;
}

void generateRandomDelay(int dmin, int dmax){
  if (needRandom == true){
    delay_1 = currentMillis + random(dmin,dmax);
    needRandom = false;
  }
  
  if (delay_1 < currentMillis)
    needRandom = true;
}

void glitch(int color_1[], int color_2[], int delayMin, int delayMax){
  
  checkButton();
  if (currentMillis > delay_1){
    
    generateRandomDelay(delayMin,delayMax);
    
    int die = random(0,10); // randoms are [0,x)
    
    if (die == 0)
      showColor(color_1);
    else if (die == 1)
      showColor(color_2);
    else if (die == 2){
      int i_max = random(50,101);
      int cycleTime = random(0,10);
      shift(blank,color_1,cycleTime,i_max,false,0);
      shift(color_1,color_2,cycleTime,i_max,false,0);
      shift(color_2,blank,cycleTime,i_max,false,0);
    }
    else if (die == 3)
      showColor(color_1);
    else if (die == 4)
      showColor(color_2);
    else if (die == 5){
      int i_max = random(50,101);
      int cycleTime = random(0,10);
      shift(color_1,color_2,cycleTime,i_max,false,0);
    }
    else if (die == 6){
      int i_max = random(50,101);
      int cycleTime = random(0,10);
      shift(color_2,color_1,cycleTime,i_max,false,0);
    }
    else if (die == 7){
      int randPct = random(0,100);
      int r = color_1[0] * randPct/100;
      int g = color_1[1] * randPct/100;
      int b = color_1[2] * randPct/100;
      int out_color[] = {r,g,b};
      showColor(out_color);
    }
    else if (die == 8){
      int randPct = random(0,100);
      int r = color_2[0] * randPct/100;
      int g = color_2[1] * randPct/100;
      int b = color_2[2] * randPct/100;
      int out_color[] = {r,g,b};
      showColor(out_color);
    }
  }
}

void circle(int input_color[], int wait) {
  uint32_t color = strip.Color(input_color[0], input_color[1], input_color[2]);
  for (int a = 0; a < 10; a++) { // Repeat 10 times...
    for (int b = 0; b < 3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for (int c = b; c < strip.numPixels(); c += 3) {
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
        checkButton();
      }
      strip.show(); // Update strip with new contents
      delay(wait);  // Pause for a moment
    }
  }
}

void rainbow(int wait) {
  int num_loop = 1;
  // Hue of first pixel runs <num_loop> complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to <num_loop>*65536. Adding 256 to firstPixelHue each time
  // means we'll make <num_loop>*65536/256 passes through this outer loop:
  for (long firstPixelHue = 0; firstPixelHue < num_loop * 65536; firstPixelHue += 256) {
    for (int i = 0; i < strip.numPixels(); i++) { // For each pixel in strip...
      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
      checkButton();
    }
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

void showColor(int input_color[]) {
  uint32_t color = strip.Color(input_color[0], input_color[1], input_color[2]);

  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, color);
  }
  strip.show();
}

void displayRGB(int r, int g, int b, int pixel, bool doShow) {
  uint32_t color = strip.Color(r, g, b);
  strip.setPixelColor(pixel, color);
  if (doShow)
    strip.show();
}

void checkButton(){
  buttonRead = digitalRead(BUTTON_PIN);
  // button is pressed (HIGH)
  if (buttonRead == HIGH){
    if (buttonActive == false){
      buttonActive = true;
      buttonTimer = millis();
    }
    if (millis() - buttonTimer >= shortPressTime)
      buttonShortPress = true; // is a short press
    if (millis() - buttonTimer >= longPressTime){
      buttonLongPress = true; // is a long press
      buttonShortPress = false; // ignore the short press
    }
  // button is released (LOW)
  } else {
    buttonActive = false;

    // do stuff if long press
    if (buttonLongPress == true){
      buttonLongPress = false;
      if (colorScheme < colorSchemeMax)
        colorScheme++;
      else
        colorScheme=0;
    }
    // do stuff if short press
    if (buttonShortPress == true){
      buttonShortPress = false;
      if (buttonCounter > buttonCounterMax)
        buttonCounter = 0;
      else
        buttonCounter++;
    }
  }
}
