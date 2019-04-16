//Include Libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <dht.h>

#define DHT11_PIN 7

static double CORRECTING_COEFFICIENT = -5.0;
static int CRYING_THRESHOLD = 700;
static int SOUND_PIN = A5;
static int DELAY_TIME = 3000;
static int TIME_BETWEEN_SOUND_CHECK = 100;
static int SOUND_CHECKS_AMOUNT = 30;
const byte address[6] = "00001";
dht DHT;
RF24 radio(9, 8);  // CE, CSN
char message[5] = "    ";


void setup()
{
  while(!Serial);
    Serial.begin(9600);
  radio.begin();
  pinMode(SOUND_PIN, INPUT);
  radio.openWritingPipe(address);
  radio.stopListening();
}

void loop()
{
  clearMessage();
  getTemp().toCharArray(message, 5);
  if(isCrying()) message[4] = '1';
  else message[4] = '0';
  radio.write(&message, sizeof(message));
  Serial.println(message);
}

String getTemp() {
  String temp = "";
  int chk = DHT.read11(DHT11_PIN);
  temp = String((int)(DHT.temperature + CORRECTING_COEFFICIENT)) + String((int)DHT.humidity);
  return temp;
}

bool isCrying() {
  int state;
  bool isCrying = false;
  for (int i = 0; i < SOUND_CHECKS_AMOUNT; i++)
  {
    state = analogRead(SOUND_PIN);
    if (state > CRYING_THRESHOLD) isCrying = true;
    delay(TIME_BETWEEN_SOUND_CHECK);
  }
  return isCrying;
}

void clearMessage()
{
  for (int i = 0; i < sizeof(message); i++)
    message[i] = ' ';  
}
