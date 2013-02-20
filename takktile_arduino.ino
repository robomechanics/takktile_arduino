/**************************************************************************/
/*!
@file takktile_arduino.ino
@author Eric Bakan & Yaroslav Tenzer
@license BSD

Driver for the TakkTile Strip sensor

This is a library for the TakkTile Strip sensor
----> http://www.takktile.com/product:takkstrip

@section HISTORY

v1.0 - First release by Eric Bakan
v1.1 - Updated for automatic sensor detection

@section NOTES

// Some logic copied from https://github.com/adafruit/Adafruit_MPL115A2/

*/
/**************************************************************************/

// Arduino addressing is using 7bit
// All the addresses were calculated in 8bit
// And were right shifted when actually used

#include <Wire.h>

#define NUM_SENSORS 10*5 // reserve addresses for 10 strips with 5 sensors on each
#define PRECISION 2

#define FREESCALE_ADDRESS 0xC0
#define SENSOR_ALL_ON 0x0C
#define SENSOR_ALL_OFF 0x0D


float a0[NUM_SENSORS];
float b1[NUM_SENSORS];
float b2[NUM_SENSORS];
float c12[NUM_SENSORS];

byte addressArray[NUM_SENSORS]; 
byte addressLength;

boolean flagShowAddress=false;
boolean flagShowPressure=true;
boolean flagShowTemperature=false;

void initialize() {
    // s 0C
  Wire.beginTransmission(SENSOR_ALL_ON>>1);
  Wire.endTransmission();
  
  // s C0 12 01
  Wire.beginTransmission(0xC0>>1);
  Wire.write(0x12);
  Wire.write(0x01);
  Wire.endTransmission();
  
  // s 0D
  Wire.requestFrom(SENSOR_ALL_ON>>1, 1);
  
  delay(5);
}

void readCoeffs(byte addressSensor, byte num) {
  
  // Select sensor
  Wire.beginTransmission(addressSensor>>1);
  Wire.endTransmission();
  
  // Request coefficients
  Wire.beginTransmission(FREESCALE_ADDRESS>>1);
  Wire.write(0x04);
  Wire.endTransmission();
  Wire.requestFrom(FREESCALE_ADDRESS>>1, 8);
  int16_t a0coeff = (( (uint16_t) Wire.read() << 8) | Wire.read());
  int16_t b1coeff = (( (uint16_t) Wire.read() << 8) | Wire.read());
  int16_t b2coeff = (( (uint16_t) Wire.read() << 8) | Wire.read());
  int16_t c12coeff = (( (uint16_t) (Wire.read() << 8) | Wire.read())) >> 2;
  // Turn sensor off
  Wire.requestFrom(addressSensor>>1, 1);
  
  a0[num] = (float)a0coeff / 8;
  b1[num] = (float)b1coeff / 8192;
  b2[num] = (float)b2coeff / 16384;
  c12[num] = (float)c12coeff;
  c12[num] /= 4194304.0;
}

void setup () {
  Wire.begin();
  Serial.begin(115200);
  
  checkAddresses(); // check how many sensors are connected
  
  // for each found sensor, read the coefficients ..
  for(int i=0;i<addressLength;i++) {
    readCoeffs(addressArray[i],i);
  }
}
void readNum(byte addressSensor, float* oTemp, float* oPressure)
{
  // Select sensor
  Wire.beginTransmission(addressSensor>>1);
  Wire.endTransmission();

  // Request P/T data
  Wire.beginTransmission(FREESCALE_ADDRESS>>1);
  Wire.write((byte)0x00);
  Wire.endTransmission();

  Wire.requestFrom(FREESCALE_ADDRESS>>1, 4);
  uint16_t pressure = (( (uint16_t) Wire.read() << 8) | Wire.read()) >> 6;
  uint16_t temp = (( (uint16_t) Wire.read() << 8) | Wire.read()) >> 6;
  
  // Turn sensor off
  Wire.requestFrom(addressSensor>>1, 1);

  float pressureComp = a0[addressSensor] + (b1[addressSensor] + c12[addressSensor]  * temp)  * pressure + b2[addressSensor] * temp;

  // Calculate temp & pressure
  *oPressure = ((65.0F / 1023.0F) * pressureComp) + 50.05F; // kPa
  *oTemp = ((float) temp - 498.0F) / -5.35F + 25.0F; // C
  
  //TODO check the pressure calculations ... something is wrong with them
  *oPressure = pressure;
  //*oTemp = temp;
}

void checkAddresses()
{
  addressLength=0;
  int temp_add=0;
  // check every strip
  for (int strip_n=0;strip_n<10;strip_n++){
    // check every sensor
    for (int sensor_n=0;sensor_n<5;sensor_n++){
      temp_add=(strip_n<<4)+sensor_n*2;       // calculate the address 

      // check if the Attiny responds with its address
      Wire.beginTransmission(temp_add>>1); // take into account that its 7bit !
      if (Wire.endTransmission()==0)  
      {
        // check if there is a sensor on this line        
        Wire.beginTransmission(FREESCALE_ADDRESS>>1);
        if (Wire.endTransmission()==0)  
          addressArray[addressLength]=temp_add;
          addressLength++;
      }
    }
  }
}

void loop() {

  float oTemp=0;
  float oPressure=0;
  
  initialize();
 
  Serial.print('[');
  for(int i=0;i<addressLength;i++)
  {
    if (i>0){
          Serial.print(',');
    }
    readNum(addressArray[i], &oTemp, &oPressure);
    Serial.print('[');    
    if (flagShowAddress){
      Serial.print(addressArray[i],HEX);
    }
    if (flagShowPressure){
      if (flagShowAddress){
        Serial.print(',');
      }
    Serial.print(oPressure,PRECISION);
    }
    if (flagShowTemperature){
      if (flagShowPressure){
        Serial.print(',');
      }
    Serial.print(oTemp,PRECISION);
    }
    Serial.print(']');
  }
  Serial.println(']');
  
  if (Serial.available()){
    byte inByte = (byte)
    Serial.read();
    if (inByte=='n') { flagShowAddress = !flagShowAddress; }
    if (inByte=='p') { flagShowPressure = !flagShowPressure; }
    if (inByte=='t') { flagShowTemperature = !flagShowTemperature; }
    
  }
}
