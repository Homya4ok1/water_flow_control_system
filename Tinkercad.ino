//https://www.tinkercad.com/things/jtahI18VaBy-antiflood-protection
// моделювання проекту в тінкеркад, з двома екранами
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Адреси дисплеїв
#define LCD1_ADDR 0x27 // Адреса першого дисплея
#define LCD2_ADDR 0x26 // Адреса другого дисплея

// Кол-во символів та рядків дисплея
#define LCD_COLS 16
#define LCD_ROWS 2

// Ініціалізуємо об'єкти LiquidCrystal_I2C для кожного дисплея
LiquidCrystal_I2C lcd1(LCD1_ADDR, LCD_COLS, LCD_ROWS);
LiquidCrystal_I2C lcd2(LCD2_ADDR, LCD_COLS, LCD_ROWS);

#define NOTE_C4 262   // До четвертої октави
#define NOTE_E4 330   // Мі мажор
#define NOTE_G4 392   // Соль мажор
#define NOTE_B7 3951  // Нота Си (B) в верхній октаві
#define NOTE_F7 2794  // Нота Фа (F) в верхній октаві

int alarmMelody[] = { NOTE_B7, NOTE_F7 };      // Мелодія тривоги
int melody[] = { NOTE_C4, NOTE_E4, NOTE_G4 };  // Мелодія
int noteDuration = 500;                        // Тривалість кожної ноти в мілісекундах

const int led = 13;  //індикація стану аварії
const int led1 = 8;  //синій світлодіод//пульс зчитування сенсорів

//індикація аварії на сенсорі
const int led2 = 7;
const int led3 = 6;
const int led4 = 5;
const int led5 = 4;

const int buzzer = 11;   //пін бузера
const int SENSOR1 = A0;  //сенсор
const int SENSOR2 = A1;
const int SENSOR3 = A2;
const int SENSOR4 = A3;
const int DigPin1 = 2;     //пін керування драйверу
const int DigPin2 = 3;     //пін керування драйверу реверс
const int buttonPin = 12;  //пін кнопки

int sensorValue1 = 0;
int sensorValue2 = 0;
int sensorValue3 = 0;
int sensorValue4 = 0;

//запис пустого значення в строчку
String sensorValue1v = " ";
String sensorValue2v = " ";
String sensorValue3v = " ";
String sensorValue4v = " ";

bool Ledtr = 0;

bool buttontr = 1;
const int humidity = 500;  //мінімальний рівень вологості

bool flagButton = 0;
bool flagAlarm = 0;   //флаг увімкнення аварії
bool flagAlarm2 = 0;  //флаг
int currentSensor = 0;

void alarm();       //об'явлення функції тривоги
void drivertest();  //об'явлення функції випробовування драйверу
int time();         //об'явлення функції часу



void setup() {
  //Serial.begin(9600);
  lcd1.init();
  lcd2.init();  // initialize the lcd
  pinMode(SENSOR1, INPUT);
  pinMode(SENSOR2, INPUT);
  pinMode(SENSOR3, INPUT);
  pinMode(SENSOR4, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(DigPin1, OUTPUT);
  pinMode(DigPin2, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(led, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(led5, OUTPUT);

  // Print a message to the LCD.
  lcd1.backlight();
  lcd2.backlight();
  lcd1.setCursor(0, 0);
  lcd1.print("    AntiFlood");
  lcd1.setCursor(0, 1);
  lcd1.print("   Protection");

  lcd2.setCursor(0, 0);
  lcd2.print("    Power by");
  lcd2.setCursor(0, 1);
  lcd2.print("  Dikhtiaruk V.");

  for (int i = 0; i < 3; i++) {
    tone(buzzer, melody[i], noteDuration / 1.25);
    delay(noteDuration);
  }
  time(4);
  lcd1.clear();
  lcd2.clear();
  drivertest();
}

void loop() {
  //
 

  // функція почергового зчитування данних з сенсорів
  switch (currentSensor) {
    case 0:
      sensorValue1 = analogRead(SENSOR1);
      break;
    case 1:
      sensorValue2 = analogRead(SENSOR2);
      break;
    case 2:
      sensorValue3 = analogRead(SENSOR3);
      break;
    case 3:
      sensorValue4 = analogRead(SENSOR4);
      break;
    case 4:
      buttontr = digitalRead(buttonPin);
     
      //тригер синього світлодіода
      Ledtr = !Ledtr;
      digitalWrite(led1, Ledtr);
      break;
  }
  currentSensor++;

  //вибірка послідовності зчитування сенсорів
  if (currentSensor >= 5) {
    lcd1.clear();
    currentSensor = 0;
  }

  // функція визову аварійного режиму
  if (sensorValue1 >= humidity) {
    sensorValue1v = " ALARM";
    alarm();
    digitalWrite(led2, 1);
  } else if (sensorValue2 >= humidity) {
    sensorValue2v = " ALARM";
    alarm();
    digitalWrite(led3, 1);
  } else if (sensorValue3 >= humidity) {
    sensorValue3v = " ALARM";
    alarm();
    digitalWrite(led4, 1);
  } else if (sensorValue4 >= humidity) {
    sensorValue4v = " ALARM";
    alarm();
    digitalWrite(led5, 1);
  } else {
    flagAlarm = 0;

    if (flagButton == 1) {
      drivertest();
    }
  }

  lcd1.setCursor(0, 0);
  lcd1.print("Sensor1   ");
  lcd1.print(sensorValue1);
  lcd1.print(sensorValue1v);

  lcd1.setCursor(0, 1);
  lcd1.print("Sensor2   ");
  lcd1.print(sensorValue2);
  lcd1.print(sensorValue2v);

  lcd2.setCursor(0, 0);
  lcd2.print("Sensor3   ");
  lcd2.print(sensorValue3);
  lcd2.print(sensorValue3v);

  lcd2.setCursor(0, 1);
  lcd2.print("Sensor4   ");
  lcd2.print(sensorValue4);
  lcd2.print(sensorValue4v);
  digitalWrite(led, flagAlarm);  //індикація стану аварії
  //digitalWrite(led1, flagAlarm);
  
  Serial.println(flagButton);

  //активація режиму перевірки  //відключення звукового сигналу
  if (buttontr == 0) {
    flagButton = 1;
  }

  if ((flagAlarm == 1) && (flagButton == 0)) {
    for (int i = 0; i < 2; i++) {
      tone(buzzer, alarmMelody[i], noteDuration);
      delay(noteDuration);
    }
    delay(200);
  }
}

void alarm()  //функція аварії
{
  flagAlarm = 1;
  if ((flagAlarm == 1) && (flagAlarm2 == 0)) {
    // Змінюємо стани пінів
    // закриваємо задвижку
    lcd1.clear();
    lcd1.setCursor(0, 0);
    lcd1.print("     ALARM");
    lcd1.setCursor(0, 1);
    lcd1.print("  shut of water");
    digitalWrite(DigPin1, HIGH);
    digitalWrite(DigPin2, LOW);
    time(8);
    flagAlarm2 = 1;
    lcd1.clear();
    lcd2.clear();
  } else {
    digitalWrite(DigPin1, LOW);
  }
}


void drivertest()  // функція тестування приводу (закриття, відкриття)
{

  tone(buzzer, 2800);
  time(1);
  noTone(buzzer);

  // Змінюємо стани пінів
  // закриваємо задвижку
  lcd1.clear();
  lcd1.setCursor(0, 0);
  lcd1.print("   drivertest");
  lcd1.setCursor(0, 1);
  lcd1.print("  shut of water");

  digitalWrite(DigPin1, HIGH);
  digitalWrite(DigPin2, LOW);
  time(8);

  // Змінюємо стани пінів
  // відкриваємо задвижку
  lcd1.clear();
  lcd1.setCursor(0, 0);
  lcd1.print("   drivertest");
  lcd1.setCursor(0, 1);
  lcd1.print("opening of water");

  digitalWrite(DigPin1, LOW);
  digitalWrite(DigPin2, HIGH);
  time(8);
  lcd1.clear();
  lcd2.clear();
  //встановлення драйверу в режим очікування
  digitalWrite(DigPin2, LOW);
  tone(buzzer, 1000);
  time(1);
  noTone(buzzer);

  buttontr = 1;
  flagAlarm = 0;
  flagAlarm2 = 0;
  flagButton = 0;
  sensorValue1v = " ";
  sensorValue2v = " ";
  sensorValue3v = " ";
  sensorValue4v = " ";
  digitalWrite(led5, 0);
  digitalWrite(led2, 0);
  digitalWrite(led3, 0);
  digitalWrite(led4, 0);
}
//лічільник часу затримки
int time(int x) {
  for (int i = 0; i <= x; i++) {
    // Serial.println( i );
    delay(1000 - 1);
  }
  return 0;
}
