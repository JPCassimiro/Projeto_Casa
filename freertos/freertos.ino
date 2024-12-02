//Flash SPI QIO pins are: GPIO 6, 7, 8, 9, 10 and 11
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>

//setup servo
Servo servoMotor;
bool doorOpenned = false;

//setup lcd
int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x3F, lcdColumns, lcdRows);  //usa pinos GPIO21(SDA) E GPIO22(SLC)
// char str[];
float tempC = 0;

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

TaskHandle_t TaskLedA3 = NULL;
TaskHandle_t TaskFanA3 = NULL;
TaskHandle_t TaskLM35 = NULL;
TaskHandle_t TaskDisplayTemp = NULL;
// TaskHandle_t TaskDisplayString = NULL;
TaskHandle_t TaskServo = NULL;
TaskHandle_t TaskBuzzer = NULL;


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
  pinMode(pinLedA3, OUTPUT);
  pinMode(pinFanA3, OUTPUT);
  pinMode(pinBuzzerA1, OUTPUT);

  //setuo tasks
  xTaskCreate(toggleLedA3, "taskLedA3", 1000, NULL, 1, &TaskLedA3);
  xTaskCreate(toggleFanA3, "taskFanA3", 1000, NULL, 1, &TaskFanA3);
  xTaskCreate(lm35A2read, "TaskLM35", 1000, NULL, 1, &TaskLM35);
  xTaskCreate(displayA2temp, "TaskDisplayTemp", 2000, NULL, 1, &TaskDisplayTemp);
  // xTaskCreate(displayA2String, "TaskDisplayString", 2000, NULL, 1, &TaskDisplayString);
  xTaskCreate(servoA1, "TaskServo", 1000, NULL, 1, &TaskServo);
  xTaskCreate(buzzerA1, "TaskBuzzer", 1000, NULL, 1, &TaskBuzzer);

  //str
  // str = "DEFAULT_STR";
}

void loop() {

}

//aciona porta 1° andar
void servoA1(void *pvParameters) {
  (void) pvParameters;
  while (1) {
    if (!doorOpenned) {
      for (int posDegrees = 0; posDegrees <= 90; posDegrees++) {
        servoMotor.write(posDegrees);
        Serial.println(posDegrees);
        vTaskDelay(25 / portTICK_PERIOD_MS);
      }
      doorOpenned = true;
    } else if (doorOpenned) {
      for (int posDegrees = 90; posDegrees >= 0; posDegrees--) {
        servoMotor.write(posDegrees);
        Serial.println(posDegrees);
        vTaskDelay(25 / portTICK_PERIOD_MS);
      }
      doorOpenned = false;
    }
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}


//liga o buzzer por 750ms
void buzzerA1(void *pvParameters) {
  (void) pvParameters;
  while (1) {
    Serial.print("\nbuzzerA1");
    digitalWrite(pinBuzzerA1, HIGH);
    vTaskDelay(750 / portTICK_PERIOD_MS);
    digitalWrite(pinBuzzerA1, LOW);
  }
}

//apresenta string no display 2º andar, a string vai só na primeira linha
// void displayA2String(void *pvParameters) {
//   (void) pvParameters;
//   while (1) {
//     Serial.print("\nDisplayA2String");
//     lcd.clear();
//     lcd.setCursor(0, 0);
//     lcd.print(str);
//     lcd.setCursor(0, 1);
//     lcd.write(byte(1));
//     lcd.write(byte(1));
//     lcd.write(byte(1));
//     vTaskDelay(1000 / portTICK_PERIOD_MS);
//   }
// }

//apresenta temp no display 2º andar
void displayA2temp(void *pvParameters) {
  (void) pvParameters;
  while (1) {
    Serial.print("\nDisplayA2Temp");
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Temperatura");
    lcd.setCursor(0, 0);
    lcd.print(tempC);
    lcd.write(byte(0));
    lcd.print("C");
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

//recebe leitura do termometro 2º andar
void lm35A2read(void *pvParameters) {
  (void) pvParameters;
  while (1) {
    Serial.print("\nleitura termometro: ");
    int adcVal = analogRead(pinLm35);
    float milliVolt = adcVal * (5 / 1023.0);
    tempC = milliVolt * 10;
    Serial.print(tempC);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

//liga/desliga fan 3º andar
void toggleFanA3(void *pvParameters) {
  (void) pvParameters;
  while (1) {
    digitalWrite(pinFanA3, !digitalRead(pinFanA3));
    Serial.println("\ntoggleFanA3");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

//liga/desliga led 3º andar
void toggleLedA3(void *pvParameters) {
  (void) pvParameters;
  while (1) {
    digitalWrite(pinLedA3, !digitalRead(pinLedA3));
    Serial.println("\ntoggleLedA3");
    vTaskDelay(250 / portTICK_PERIOD_MS);
  }
}
