//Flash SPI QIO pins are: GPIO 6, 7, 8, 9, 10 and 11
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>

String comando = "";             //Variável que guarda o comando
bool comandoFinalizado = false;  //Variável que confirma se recebeu o comando completo ou não

//setup servo
Servo servoMotor;
bool doorOpenned = false;

//setup lcd
int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x3F, lcdColumns, lcdRows);  //usa pinos GPIO21(SDA) E GPIO22(SLC)
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

//setup taskHandle
TaskHandle_t TaskLedA3 = NULL;
TaskHandle_t TaskFanA3 = NULL;
TaskHandle_t TaskLM35 = NULL;
TaskHandle_t TaskDisplayTemp = NULL;
TaskHandle_t TaskDisplayString = NULL;
TaskHandle_t TaskServo = NULL;
TaskHandle_t TaskBuzzer = NULL;
TaskHandle_t TaskSerial = NULL;


void setup() {
  Serial.begin(115200);

  //inicialização servo
  servoMotor.attach(pinServoA1);

  //inicialização lcd
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, customChar);
  lcd.createChar(1, Heart);

  //pinos digitais de saida
  pinMode(pinLedA3, OUTPUT);
  pinMode(pinFanA3, OUTPUT);
  pinMode(pinBuzzerA1, OUTPUT);

  //setup tasks
  xTaskCreate(toggleLedA3, "TaskLedA3", 1000, NULL, 1, &TaskLedA3);
  xTaskCreate(toggleFanA3, "TaskFanA3", 1000, NULL, 1, &TaskFanA3);
  xTaskCreate(lm35A2read, "TaskLM35", 1000, NULL, 1, &TaskLM35);
  xTaskCreate(displayA2temp, "TaskDisplayTemp", 2000, NULL, 1, &TaskDisplayTemp);
  xTaskCreate(displayA2String, "TaskDisplayString", 2000, NULL, 1, &TaskDisplayString);
  xTaskCreate(servoA1, "TaskServo", 1000, NULL, 1, &TaskServo);
  xTaskCreate(buzzerA1, "TaskBuzzer", 1000, NULL, 1, &TaskBuzzer);
  xTaskCreate(serialReceive, "TaskSerial", 1000, NULL, 1, &TaskSerial);
}

void loop() {
}

//aciona porta 1° andar
void servoA1(void *pvParameters) {
  (void)pvParameters;
  while (1) {
    if ((comando.charAt(0) == '4') && (comandoFinalizado == true)) {//verifica o comando recebido
      if (!doorOpenned) {//variavel de controle que determina se a porta ta aberta ou fechada, a porta tem que começar fechada pra funcionar
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
      comando = "";
      comandoFinalizado = false;
      vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    vTaskDelay(60 / portTICK_PERIOD_MS);
  }
}

//liga o buzzer por 750ms
void buzzerA1(void *pvParameters) {
  (void)pvParameters;
  while (1) {
    if ((comando.charAt(0) == '5') && (comandoFinalizado == true)) {//verifica o comando recebido
      Serial.print("\nbuzzerA1");
      digitalWrite(pinBuzzerA1, HIGH);
      comando = "";
      comandoFinalizado = false;
      vTaskDelay(750 / portTICK_PERIOD_MS);
      digitalWrite(pinBuzzerA1, LOW);
    }
    vTaskDelay(60 / portTICK_PERIOD_MS);
  }
}

// apresenta string no display 2º andar
void displayA2String(void *pvParameters) {
  (void)pvParameters;
  while (1) {
    if ((comando.charAt(0) == '3') && (comandoFinalizado == true)) {//verifica o comando recebido
      unsigned long startTime = millis();
      Serial.print("\nDisplayA2String");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.write(byte(1));
      lcd.print("");
      lcd.write(byte(1));
      lcd.print("");
      lcd.write(byte(1));
      lcd.print("");
      lcd.write(byte(1));
      lcd.print("");
      lcd.write(byte(1));
      lcd.print("");
      lcd.write(byte(1));
      lcd.print("");
      lcd.setCursor(0, 1);
      lcd.write(byte(1));
      lcd.print("");
      lcd.write(byte(1));
      lcd.print("");
      lcd.write(byte(1));
      lcd.print("");
      lcd.write(byte(1));
      lcd.print("");
      lcd.write(byte(1));
      lcd.print("");
      lcd.write(byte(1));
      lcd.print("");
      comando = "";
      comandoFinalizado = false;
      unsigned long endTime = millis();
      Serial.print((endTime-startTime));
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    vTaskDelay(60 / portTICK_PERIOD_MS);
  }
}

//apresenta temp no display 2º andar
void displayA2temp(void *pvParameters) {
  (void)pvParameters;
  while (1) {
    if ((comando.charAt(0) == '2') && (comandoFinalizado == true)) {//verifica o comando recebido
      Serial.print("\nDisplayA2Temp");
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("Temperatura");
      lcd.setCursor(0, 0);
      lcd.print(tempC);
      lcd.write(byte(0));
      lcd.print("C");
      comando = "";
      comandoFinalizado = false;
      vTaskDelay(2500 / portTICK_PERIOD_MS);
    }
    vTaskDelay(60 / portTICK_PERIOD_MS);
  }
}

//recebe leitura do termometro 2º andar
void lm35A2read(void *pvParameters) {
  (void)pvParameters;
  while (1) {
    Serial.print("\nleitura termometro: ");
    int adcVal = analogRead(pinLm35);
    float milliVolt = adcVal * (5 / 1023.0);
    tempC = milliVolt * 10;
    Serial.print(tempC);
    vTaskDelay(2500 / portTICK_PERIOD_MS);
  }
}

//liga/desliga fan 3º andar
void toggleFanA3(void *pvParameters) {
  (void)pvParameters;
  while (1) {
    if ((comando.charAt(0) == '1') && (comandoFinalizado == true)) {//verifica o comando recebido
      digitalWrite(pinFanA3, !digitalRead(pinFanA3));
      Serial.println("\ntoggleFanA3");
      comando = "";
      comandoFinalizado = false;
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    vTaskDelay(60 / portTICK_PERIOD_MS);
  }
}

//liga/desliga led 3º andar
void toggleLedA3(void *pvParameters) {
  (void)pvParameters;
  while (1) {
    if ((comando.charAt(0) == '0') && (comandoFinalizado == true)) {//verifica o comando recebido
      digitalWrite(pinLedA3, !digitalRead(pinLedA3));
      Serial.println("\ntoggleLedA3");
      comando = "";
      comandoFinalizado = false;
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    vTaskDelay(60 / portTICK_PERIOD_MS);
  }
}

//aparenta functionar
// void serialEvent() {
//   while (Serial.available()) {
//     char inChar = (char)Serial.read();
//     comando += inChar;
//     if (inChar == '\n') {
//       comandoFinalizado = true;
//     }
//   }
// }

//já que serialEvent não é por interrupt, talvez essa sejá uma solução melhor
void serialReceive(void *pvParameters) {
  (void)pvParameters;
  while (1) {
    if (Serial.available()){
      char inChar = (char)Serial.read();
      comando += inChar;
      if (inChar == '\n') {
        comandoFinalizado = true;
      }
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
