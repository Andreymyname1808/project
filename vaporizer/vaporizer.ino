#include <DHT.h>

const int RELE_FAN = 10;
const int RELE_VAPORIZER = 3;
const int DHT11_1 = 4;
const int DHT11_2 = 5;

DHT dht_1(DHT11_1, DHT11);
DHT dht_2(DHT11_2, DHT11);

// датчик на объекте
float temp_1 = 0;
float humidity_1 = 0;

// датчик в багажнике
float temp_2 = 0;
float humidity_2 = 0;

bool enable_cooling = 0;

void setup() {
  Serial.begin(9600);
  pinMode(RELE_FAN,OUTPUT);
  pinMode(RELE_VAPORIZER,OUTPUT);
  dht_1.begin();
  dht_2.begin();
}

void loop() {
  update_sensors();
  bluetooth();
  if (enable_cooling==1) {
    cooling();
  }
  else {
    cooling_off();  
  }
}

void update_sensors() {
  humidity_1 = dht_1.readHumidity(); //Измеряем влажность
  temp_1 = dht_1.readTemperature(); //Измеряем температуру
  if (isnan(humidity_1) || isnan(temp_1)) {  // Проверка. Если не удается считать показания, выводится «Ошибка считывания»
    Serial.println("error: sensor_1");
  }
  humidity_2 = dht_2.readHumidity(); //Измеряем влажность
  temp_2 = dht_2.readTemperature(); //Измеряем температуру
  if (isnan(humidity_2) || isnan(temp_2)) {  // Проверка. Если не удается считать показания, выводится «Ошибка считывания»
    Serial.println("error: sensor_2");
  }
}

float f(float t, float rh) {
  return 17.27*t/(237.7+t)+log(rh/100);
}

float dew_point(float t, float rh) {
  return 237.7*f(t,rh)/(17,27-f(t,rh));
}

void cooling() {
  float dew_point_1 = dew_point(temp_1,humidity_1);
  if (temp_1<dew_point_1) {
    digitalWrite(RELE_FAN,1);
    digitalWrite(RELE_VAPORIZER,0);
  }
  else {
    digitalWrite(RELE_FAN,0);
    digitalWrite(RELE_VAPORIZER,1);
  }
}

void cooling_off() {
  digitalWrite(RELE_FAN,0);
  digitalWrite(RELE_VAPORIZER,0);
}

void bluetooth() {
  if (Serial.available()>0) {
    char f = Serial.read();
    if (f=='g') {
      Serial.print("temp_1:     ");
      Serial.println(temp_1);
      Serial.print("humidity_1: ");
      Serial.println(humidity_1);
      Serial.print("temp_2:     ");
      Serial.println(temp_2);
      Serial.print("humidity_2: ");
      Serial.println(humidity_2);
      Serial.println();
    }
    if (f=='1') {
      enable_cooling = 1;
    }
    if (f=='0') {
      enable_cooling = 0;
    }
  }  
}
