#include <dht.h>
dht DHT;
#define DHT11_PIN 2
static double CORRECTING_COEFFICIENT = -5.0;
void setup() {
Serial.begin(9600);
}

void loop() {
  int chk = DHT.read11(DHT11_PIN);
  Serial.print("Temperature =");
  Serial.println(DHT.temperature + CORRECTING_COEFFICIENT);
  Serial.print("Humidity =");
  Serial.println(DHT.humidity);
  delay(5000); 
}
