#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include <LowPower.h>
#include <ThreeWire.h>  
#include <RtcDS1302.h>

#define hdc1080_adress 0b1000000

/// PINS
//SDA = A4, SCL = A5,MISO2 = 12, MOSI2 = 11, SCK = 13, CS = 10, CLK = 3, DAT = 2, RST = 4;
/// PINS

File myFile;
ThreeWire myWire(4,5,2); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

void setup() {
	pinMode(LED_BUILTIN, OUTPUT);
	Rtc.Begin();
	SD.begin(10)
	Wire.beginTransmission(hdc1080_adress);
	Wire.write(0x02);
	Wire.write(0b000100000000000); // sets up configuration 
	Wire.endTransmission(true);
}

void loop() {
	// Iespējams kods var būt nedaudz nepareizs jo man nav iespēja pārbaudīt to

	// read time and date from clock
	RtcDateTime time = Rtc.GetDateTime();
	// read temperature
	Wire.beginTransmission(hdc1080_adress);
	Wire.write(0x00);  //send the address of the data we want to the hdc so it knows what to send
	Wire.endTransmission(false);
	Wire.requestFrom(hdc1080_adress, 2, true);

	sleep(10); // Wait a bit so that the sensor has time to make the mesurement

	double temperature;
	temperature = (Wire.read() << 8 | Wire.read());  //get the raw data from registers
  
	// acces file on sd card and add a new line of data
	myFile = SD.open("data", FILE_WRITE); // automaticaly creates the file if its not available
	if (myFile) {
		string data_s = time.date() + "_" + time.time() + "_" + temperature;
		for (int i = 0; i < data_s.length(); i++)
		{
			data_s[i] = (~data_s[i]); // encryption
		}
		myFile.println(data_s);
		myFile.close();
	}
	// sleep for 10 minutes
	
	digitalWrite(LED_BUILTIN, LOW);
	for (int i = 0; i < 75; i++)
	{
		LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
		/// For better optimization possibly look into setting the standby mode without library
	}
}
