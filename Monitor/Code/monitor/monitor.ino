#include "LPD8806.h"
#include "SPI.h"
#include <RFM69.h>
#include "m3RFM.h"
#define NODEID     42

int brightness = 127; // choose depending on environment

const int seats_start  =  10;
const int seats_end    =  41;
const int thermo_start =   0;
const int thermo_end   =   9;
const int air_start    =  42;
const int air_end      =  53;
const int sound_start  =  54;
const int sound_end    = 123;

const uint16_t nLEDs       = 123;
const uint8_t dataPin      =   4;
const uint8_t clockPin     =   3;
const uint8_t thermoPotPin =  A3;
const uint8_t seatsPotPin  =  A4;
const uint8_t airPotPin    =  A5;
const uint8_t button0      =   0;

const uint8_t spalten = 11;
const int s = sound_start;
const uint16_t sound [12][7] = {{s+0, s+15, s+16, s+39, s+40, -1, -1},    {s+1, s+14, s+17, s+38, s+41, s+61, s+62}, {-1, -1, s+18, s+37, s+42, s+60, s+63},
                                {-1, s+13, s+19, s+36, s+43, s+59, s+64}, {s+2, s+12, s+20, s+35, s+44, s+58, s+65}, {s+3, s+11, s+21, s+34, s+45, s+57, s+66},
                                {-1, s+10, s+22, s+33, s+46, s+56, s+67},   {-1, s+9, s+23, s+32, s+47, -1, -1},     {s+4, s+8, s+24, s+31, s+48, s+55, -1},
                                {s+5, s+7, s+25, s+30, s+49, s+54, -1},   {-1, s+6, s+26, s+29, s+50, s+53, s+68},   {-1, -1, s+27, s+28, s+51, s+52, s+69}};

float thermoVal    = 0;
float seatsVal     = 0;
float airVal       = 0;
float soundVal     = 0;
float seatsMapVal  = 0;
float thermoMapVal = 0;
float airMapVal    = 0;
float soundMapVal  = 0;
int i              = 0;
int randomNumber;
float s_temp, s_sound, s_seat = 0;

LPD8806 strip = LPD8806(nLEDs, dataPin, clockPin);

bool test = 0;
bool manual;

void setup(){
  strip.begin();
  radio.initialize(FREQUENCY, NODEID, NETWORKID);
  radio.encrypt(ENCRYPTKEY);
  pinMode(button0, INPUT_PULLUP);
  pinMode(8, OUTPUT);
  Serial.begin(19230);
  //create sequency of random numbers
  randomSeed(analogRead(A5));
  for(i = 0; i <= nLEDs; i++){
    strip.setPixelColor(i, 0,0,0);
  }
    strip.show();
}

//main loop
void loop(){
  receiveRFM();
  Serial.print("TEMP: ");
  Serial.println(getFloatWert());
  //check weather the button is on manual (true) or sensors (false)
  Serial.println(digitalRead(button0));
  //loop for fake data
  if(manual){
    //get recent values from the potentiometer
    thermoVal = analogRead(thermoPotPin);
    seatsVal  = analogRead(seatsPotPin);
    airVal    = analogRead(airPotPin);
    
    
    //map the values to the range needed
    //the used potentiometer have a range from 0 to 900 with the used power supply of 1.5 A and 2.0 A for the sound
    //the reason I map from 800 to 0 is that i accidentally transposed ground and vcc on the board with the potis
    seatsMapVal  = map(seatsVal, 800, 0, 0, 12);
    thermoMapVal = map(thermoVal, 800, 0, 0, 19);
    airMapVal    = map(airVal, 800, 0, 0, 12);
   
    //function calls (see below)
    temperature();
    seats();
    air();
    soundbars();
    //finally show the strip with its new values
    strip.show();
    delay(400);
    manual = digitalRead(button0);
  }else if (!manual){
    if(getKlasse() == ACK_TEMP || getKlasse() == TEMP){
      
      thermoVal = int(getFloatWert());
      s_temperature();
    }
    if(getKlasse() == ACK_OTHER || getKlasse() == OTHER){
      seatsVal = getWert();
      s_seats();
    }
    
    //for Schleife einbauen??
    air();
    
    if(getKlasse() == ACK_SOUND || getKlasse() == SOUND){
      soundVal = getWert();
      s_soundbars();
    }
    
    strip.show();
    delay(400);
    manual = digitalRead(button0);
  }
  digitalWrite(8, manual);
}

void myColorFunction(int start, int end, int red, int green, int blue){
	for (int start = 0; start <= end; start++)
	{
		strip.setPixelColor(i, red, green, blue);
	}
}

void s_temperature(){
	/*uint16_t j, red, green, blue;

	//calculation of the rgb values based on the temperature potentiometer (can be modified individually)
	red = max(0, -176 + ((thermoVal - 1) * 16));
	green = max(0, 127 - abs(16 * (thermoVal - 9)));
	blue = max(0, min(127, 127 - (thermoVal * 16)));

	//calculates which leds have to be on
	for (j = thermo_start; j <= thermo_end; j++){
		//turning off not needed leds for current potentiometer value
		if (j > thermoMapVal / 2){
			strip.setPixelColor(j, 0, 0, 0);
		}
		else{
			//turning on the leds with colors calculated above
			strip.setPixelColor(j, red, green, blue);
		}
	}*/

	switch ((int)thermoVal)
	{
	case 16: myColorFunction(thermo_start, thermo_start + 0,  0, 0, 127);
	case 17: myColorFunction(thermo_start, thermo_start + 0,  0, 0, 127);
	case 18: myColorFunction(thermo_start, thermo_start + 1,  0, 0, 127);
	case 19: myColorFunction(thermo_start, thermo_start + 1,  0, 0, 127);
	case 20: myColorFunction(thermo_start, thermo_start + 2,  0, 0, 127);
	case 21: myColorFunction(thermo_start, thermo_start + 2,  0, 0, 127);
	case 22: myColorFunction(thermo_start, thermo_start + 3,  0, 0, 127);
	case 23: myColorFunction(thermo_start, thermo_start + 3,  0, 0, 127);
	case 24: myColorFunction(thermo_start, thermo_start + 4,  0, 0, 127);
	case 25: myColorFunction(thermo_start, thermo_start + 4,  0, 0, 127);
	case 26: myColorFunction(thermo_start, thermo_start + 5, 0, 0, 127);
	case 27: myColorFunction(thermo_start, thermo_start + 5, 0, 0, 127);
	default:
		break;
	}
}


void s_seats(){}
void s_soundbars(){}

//calculation for the temperature LEDs
void temperature(){
  uint16_t j, red, green, blue;
      
  //calculation of the rgb values based on the temperature potentiometer (can be modified individually)
  red   = max(0, -176 + ((thermoMapVal-1) * 16));
  green = max(0, 127 - abs(16 * (thermoMapVal - 9)));
  blue  = max(0, min(127, 127 - (thermoMapVal * 16)));
  
  //calculates which leds have to be on
  for(j = thermo_start; j <= thermo_end; j++){
    //turning off not needed leds for current potentiometer value
    if(j > thermoMapVal/2){
      strip.setPixelColor(j, 0,0,0);
    }else{
    //turning on the leds with colors calculated above
      strip.setPixelColor(j, red, green, blue);
    }
  }
}

void seats(){
  uint16_t j;
  //calculation weather a seat is free or taken
  for(j = seats_start; j <= seats_end; j+=4){
    if(j/4 <= seatsMapVal){
      //red
      strip.setPixelColor(j    , brightness, 0, 0);
      strip.setPixelColor(j+1  , brightness, 0, 0);
      strip.setPixelColor(j+2  , brightness, 0, 0);
      strip.setPixelColor(j+3  , brightness, 0, 0);
    }else{
      //green
      strip.setPixelColor(j    , 0, brightness, 0);
      strip.setPixelColor(j+1  , 0, brightness, 0);
      strip.setPixelColor(j+2  , 0, brightness, 0);
      strip.setPixelColor(j+3  , 0, brightness, 0);
    }
  }
}

void air(){
  uint16_t j, red, green, blue;
  
  //calculation of the rgb values based on the air potentiometer (can be modified individually)
  red = min(127, max(0, ((airMapVal-1) * 16)));
  green = max(0, min(127, 127 - (airMapVal * 16)));
  blue = max(0, min(127, 255 - (airMapVal * 16)));
  
  //calculates which leds have to be on
  for(j = air_end; j >= air_start; j--){
    //turning off not needed leds for current potentiometer value
    if(j < air_end - airMapVal){
      strip.setPixelColor(j, 0,0,0);
    }else{
    //turning on the leds with colors calculated above
      strip.setPixelColor(j, red, green, blue);
    }
  }
}

void soundbars(){
  //create random number of range 0 to 6 (7 discrete values are needed for this configuratios
  randomNumber = random(6);
  //map that random number to the led brightness range
  soundMapVal = map(randomNumber, 0, 5, 0, 127);
  
  //calculate which leds in each column have to shine with the intensity depending on the random value
  for(i = 0; i <= spalten; i++){        
    strip.setPixelColor(sound[i][0] , max(0, soundMapVal - 120), max(0, soundMapVal - 120), max(0, soundMapVal - 120));
    strip.setPixelColor(sound[i][1] ,   max(0, soundMapVal - 90), max(0, soundMapVal - 90), max(0, soundMapVal - 90));
    strip.setPixelColor(sound[i][2] , max(0, soundMapVal - 60), max(0, soundMapVal - 60), max(0, soundMapVal - 60));
    strip.setPixelColor(sound[i][3] , soundMapVal, soundMapVal, soundMapVal);
    strip.setPixelColor(sound[i][4] , max(0, soundMapVal - 60), max(0, soundMapVal - 60), max(0, soundMapVal - 60));
    strip.setPixelColor(sound[i][5] , max(0, soundMapVal - 90), max(0, soundMapVal - 90), max(0, soundMapVal - 90));
    strip.setPixelColor(sound[i][6] , max(0, soundMapVal - 120), max(0, soundMapVal - 120), max(0, soundMapVal - 120));
  }
}

