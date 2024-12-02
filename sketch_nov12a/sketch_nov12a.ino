//Flash SPI QIO pins are: GPIO 6, 7, 8, 9, 10 and 11
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>

//setup servo
Servo servoMotor;
bool doorOpenned = false;

//setup lcd
int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x3F, lcdColumns, lcdRows);
//usa pinos GPIO21(SDA) E GPIO22(SLC)

//definição de pinos
#define pinLm35 34
#define pinLedA3 15
#define pinFanA3 2
#define pinServoA1 13
#define pinBuzzerA1 18

//char custom usado no display
byte Heart[] = {
  B00000,
  B01010,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000,
  B00000
};

byte customChar[] = {
  B00110,
  B01001,
  B01001,
  B00110,
  B00000,
  B00000,
  B00000,
  B00000,
};

void setup() {
  Serial.begin(115200);

  //inicialização servo
  servoMotor.attach(pinServoA1);

  //inicialização lcd
  lcd.init();
  lcd.backlight();
  pinMode(pinLedA3, OUTPUT);
  pinMode(pinFanA3, OUTPUT);
  lcd.createChar(0, customChar);
  lcd.createChar(1, Heart);

  //pinos digitais de saida
  pinMode(pinLedA3,OUTPUT);
  pinMode(pinFanA3,OUTPUT);
  pinMode(pinBuzzerA1,OUTPUT);

}

void loop() {
  toggleLedA3();
  toggleFanA3();
  delay(1500);
  toggleLedA3();
  toggleFanA3();
  lm35A2read();
  delay(1500);
  buzzerA1();
  openDoor();
  delay(1500);
  closeDoor();
  delay(1000);
  // coopMultitask();//what if...

}

//liga o buzzer por 750ms
void buzzerA1(){
  Serial.print("\nbuzzerA1");
  digitalWrite(pinBuzzerA1, HIGH);
  delay(750);
  digitalWrite(pinBuzzerA1, LOW);
}

//apresenta string no display 2º andar, a string vai só na primeira linha
void displayA2String(char str[]) {
  Serial.print("\nDisplayA2String");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(str);
  lcd.setCursor(0, 1);
  lcd.write(byte(1));
  lcd.write(byte(1));
  lcd.write(byte(1));
}

//apresenta temp no display 2º andar
void displayA2temp(float tempC) {
  Serial.print("\nDisplayA2Temp");
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Temperatura");
  lcd.setCursor(0, 0);
  lcd.print(tempC);
  lcd.write(byte(0));
  lcd.print("C");
}

//recebe leitura do termometro 2º andar
void lm35A2read() {
  Serial.print("\nleitura termometro: ");
  int adcVal = analogRead(pinLm35);
  float milliVolt = adcVal * (5 / 1023.0);
  float tempC = milliVolt * 10;
  Serial.print(tempC);
  displayA2temp(tempC);
}

//liga/desliga fan 3º andar
void toggleFanA3() {
  digitalWrite(pinFanA3, !digitalRead(pinFanA3));
  Serial.println("\ntoggleFanA3");
}

//liga/desliga led 3º andar
void toggleLedA3() {
  digitalWrite(pinLedA3, !digitalRead(pinLedA3));
  Serial.println("\ntoggleLedA3");
}

//Abre porta 1º andar
void openDoor(){
  if(!doorOpenned){
    for(int posDegrees = 0; posDegrees <= 90; posDegrees++) {
      servoMotor.write(posDegrees);
      Serial.println(posDegrees);
      delay(25);
    }
  }
  doorOpenned = true;
}

//fecha porta 1º andar
void closeDoor(){
  if(doorOpenned){
    for(int posDegrees = 90; posDegrees >= 0; posDegrees--) {
      servoMotor.write(posDegrees);
      Serial.println(posDegrees);
      delay(25);
    }
  }
  doorOpenned = false;
}

//a maior piada de todos os tempos
void coopMultitask() {
  int varCoop = 0;
  while(1){
    toggleLedA3();
    switch(varCoop){//funções dentro do switch são excutadas a cada valor do delay * numero de tarefas dentro do switch
      case 0:
        displayA2String("CoopMultitask");
        varCoop = 1;
        break;
      case 1:
        toggleFanA3();
        varCoop = 2;
        break;
      case 2:
        buzzerA1();
        varCoop = 3;
      case 3:
        lm35A2read();
        varCoop = 0;
    }
    delay(250);
  }
}
