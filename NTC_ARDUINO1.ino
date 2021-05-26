// Thermistor Example #3 from the Adafruit Learning System guide on Thermistors
// https://learn.adafruit.com/thermistor/overview by Limor Fried, Adafruit Industries
// MIT License - please keep attribution and consider buying parts from Adafruit

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

byte N[8] = {
B00111,
B00101,
B00111,
B00000,
B00000,
B00000,
B00000,
B00000,
};

// which analog pin to connect
#define THERMISTORPIN A0
// resistance at 25 degrees C
#define THERMISTORNOMINAL 10000
// temp. for nominal resistance (almost always 25 C)
#define TEMPERATURENOMINAL 25
// how many samples to take and average, more takes longer
// but is more 'smooth'
#define NUMSAMPLES 5
// The beta coefficient of the thermistor (usually 3000-4000)
#define BCOEFFICIENT 3950
// the value of the 'other' resistor
#define SERIESRESISTOR 110000

int samples[NUMSAMPLES];

void setup(void)
{
  pinMode(13, OUTPUT);

  Serial.begin(9600);
  //analogReference(EXTERNAL);

  // Inicializar el LCD
  lcd.init();

  //Encender la luz de fondo.
  lcd.backlight();

  lcd.createChar (0,N);
  
  // Escribimos el Mensaje en el LCD.
  lcd.setCursor(0, 0);
  lcd.print("Registro de");
  lcd.setCursor(0, 1);
  lcd.print("temperatura");
  lcd.setCursor(0, 2);
  lcd.print("Estado de la");
  lcd.setCursor(0, 3);
  lcd.print("incubadora");
}

void loop(void)
{
  uint8_t i;
  float average;

  // take N samples in a row, with a slight delay
  for (i = 0; i < NUMSAMPLES; i++)
  {
    samples[i] = analogRead(THERMISTORPIN);
    delay(10);
  }

  // average all the samples out
  average = 0;
  for (i = 0; i < NUMSAMPLES; i++)
  {
    average += samples[i];
  }
  average /= NUMSAMPLES;

  Serial.print("Average analog reading ");
  Serial.println(average);

  // convert the value to resistance
  average = 1023 / average - 1;
  average = SERIESRESISTOR / average;
  Serial.print("Thermistor resistance ");
  Serial.println(average);

  float steinhart;
  steinhart = average / THERMISTORNOMINAL;          // (R/Ro)
  steinhart = log(steinhart);                       // ln(R/Ro)
  steinhart /= BCOEFFICIENT;                        // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;                      // Invert
  steinhart -= 273.15;                              // convert absolute temp to C

  Serial.print("Temperature ");
  Serial.print(steinhart);
  Serial.println(" *C");

  lcd.setCursor(12, 1);
  // Escribimos el la temperatura
  lcd.print(steinhart);
  lcd.setCursor(17, 1);
  lcd.write (byte (0));
  lcd.print("C");

  if (steinhart > 30 && steinhart < 35)
  {
    digitalWrite(2, HIGH);
    lcd.setCursor(15, 3);
    // Escribimos el la temperatura
    lcd.print("ON ");
  }
  else
  {
    digitalWrite(2, LOW);
    lcd.setCursor(15, 3);
    // Escribimos el la temperatura
    lcd.print("OFF");
  }

  delay(5000);
}
