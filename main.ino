#include <Adafruit_AHTX0.h>
#include <LiquidCrystal.h>
#include <SPI.h>
#include <SD.h>


Adafruit_AHTX0 aht;

int ThermistorPin = 0;   //pin thermister is on
int Vo; 
float R1 = 10000;       //resistance of thermister resistor
float logR2, R2, T, C;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;  //for thermister conversion to temp

float actual;
File dataFile;
int sum = 0;                    // sum of samples taken
unsigned char sample_count = 0; // current sample number
float voltage = 0.0;            // calculated voltage

int counter = 0;


LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {

  Serial.begin(9600);
  //lcd.print("Temp = ");
    lcd.begin(16,2);

    aht.begin();

    SD.begin(53);

}

void loop() {

    while (sample_count < 10) {     //code for alert system
        sum += analogRead(A2);
        sample_count++;
        delay(10);
    }

    voltage = ((float)sum / (float)10 * 5.015) / 1024.0;   //convert analog signal to voltage

    if (voltage < 5) {                 //if less then 5V, turn on LED
      digitalWrite(10, HIGH);
    }

  sensors_event_t humidity, temp;

  dataFile = SD.open("data.txt", FILE_WRITE);  //open file for SD card

  aht.getEvent(&humidity, &temp); //lcd.print(temp.temperature), lcd.print(humidity.relative_humidity)

  Vo = analogRead(ThermistorPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));    //code for thermister
  T = T - 273.15; 
  T = (T * 9.0)/ 5.0 + 32.0; 
  actual = (T*1.0984) - 1.8521;
  C = (T - 32) * 0.5556;

  actual = (temp.temperature + C) / 2;

  
  lcd.print("Temp = ");
  lcd.print(actual);
  lcd.print(" *C");

  lcd.setCursor(0, 1);              //print to second line
  lcd.print("Humidity: ");
  lcd.print(humidity.relative_humidity);

  dataFile.print("Temp: ");
  dataFile.println(actual);
  dataFile.print("Humidity: ");
  dataFile.println(humidity.relative_humidity);
  dataFile.println();

  dataFile.close();   //close SD card file
  
  delay(1000);              //delay 1s between measurements
  lcd.clear();
}
