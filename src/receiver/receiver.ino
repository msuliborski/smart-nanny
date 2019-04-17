#include "LiquidCrystal.h"

#include "Time.h"

#include <EEPROM.h>

#include <RTClib.h>

#include <Wire.h>

#include <SPI.h>

#include <nRF24L01.h>

#include <RF24.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
RF24 radio(3, 2); // CE, CSN
RTC_DS1307 rtc;

const byte address[6] = "00001";
int lcd_key = 0;
int adc_key_in = 0;
bool pressed = false;
bool canDeleteAlarm = false;
char frame[5] = {0};
#define btnRIGHT 0
#define btnUP 1
#define btnDOWN 2
#define btnLEFT 3
#define btnSELECT 4
#define btnNONE 5

int alarms[15];
int alarmsNum = 0;
int alarmsCount = 0;
int menuStatus = 0;
int blink = 0;
int blinkTime = 0;
int h = 0;
int m = 0;
int temp = 0;
int hum = 0;
bool lowTempAlert = false;
bool highTempAlert = false;
bool alarmAlert = false;
bool deviceNotConnected = false;
float milisecondsFromLastPacket = 100;

int read_LCD_buttons() {
    adc_key_in = analogRead(0);
    if (adc_key_in > 1000) return btnNONE;
    if (adc_key_in < 50) return btnRIGHT;
    if (adc_key_in < 250) return btnUP;
    if (adc_key_in < 450) return btnDOWN;
    if (adc_key_in < 650) return btnLEFT;
    if (adc_key_in < 850) return btnSELECT;
    return btnNONE;
}

void setup() {
    //TODO
    //wylaczanie, kasowanie alarmow
    
    if (!rtc.begin()) {
        while (1);
    }
    radio.begin();
    radio.openReadingPipe(0, address);
    radio.startListening();
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

    lcd.begin(16, 2);
    lcd.setCursor(0, 0);
    lcd.print("Welcome to");
    lcd.setCursor(0, 1);
    lcd.print("smart-nanny");
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 1);
    menuStatus = 0;

    while (EEPROM.read(alarmsNum) != 255)
        alarmsNum++;

}

void loop() {

    lcd_key = read_LCD_buttons();
    if (radio.available()) {
        radio.read( & frame, sizeof(frame));
        temp = (frame[0] - 48) * 10 + (frame[1] - 48);
        hum = (frame[2] - 48) * 10 + (frame[3] - 48);
        milisecondsFromLastPacket = 0;
    }
    milisecondsFromLastPacket += 0.01;
    if (temp >= 32)
        highTempAlert = true;
    else if (temp <= 15)
        lowTempAlert = true;
    else {
        highTempAlert = false; 
        lowTempAlert = false;
    }

    DateTime t = rtc.now();

    for (int i = 0; i < alarmsNum; i += 2) {
        if (t.hour() == EEPROM.read(i) && t.minute() == EEPROM.read(i + 1) && t.second() == 0) {
            alarmAlert = true;
        }
    }

    switch (lcd_key) {
    case btnRIGHT:
        {
            if (!pressed) {
                if (menuStatus == 2) {
                    if (blink == 1) {
                        h++;
                        if (h == 24)
                            h = 0;
                    } else if (blink == 2) {
                        m++;
                        if (m == 60)
                            m = 0;
                    }
                } else if (menuStatus == 3) {
                    alarmsCount += 2;
                    if (alarmsCount == alarmsNum)
                        alarmsCount = 0;
                }
                pressed = true;
            }
            break;
        }
    case btnLEFT:
        {
            if (!pressed) {
                if (menuStatus == 2) {
                    if (blink == 1) {
                        h--;
                        if (h == -1)
                            h = 23;
                    } else if (blink == 2) {
                        m--;
                        if (m == -1)
                            m = 59;
                    }
                } else if (menuStatus == 3) {
                    alarmsCount -= 2;
                    if (alarmsCount < 0)
                        alarmsCount = alarmsNum - 2;
                }
                pressed = true;
            }
            break;
        }
    case btnUP:
        {
            if (!pressed) {
                pressed = true;
                menuStatus++;
                if (menuStatus == 5)
                    menuStatus = 0;
            }
            break;
        }
    case btnDOWN:
        {
            if (!pressed) {
                pressed = true;
                menuStatus--;
                if (menuStatus == -1)
                    menuStatus = 4;
            }

            break;
        }
    case btnSELECT:
        {
            if (!pressed) {
                pressed = true;

                if (lowTempAlert)
                    lowTempAlert = false;
                else if (highTempAlert)
                    highTempAlert = false;
                else if (alarmAlert)
                    alarmAlert = false;

                if (menuStatus == 2) {
                    blink++;
                    if (blink == 3) {
                        blink = 0;
                        EEPROM.write(alarmsNum, h);
                        EEPROM.write(alarmsNum + 1, m);
                        //alarms[alarmsNum] = h*100 + m;
                        alarmsNum += 2;
                    }
                } else if (menuStatus == 3) {
                    canDeleteAlarm = true;
                }
            }

            break;
        }
    case btnNONE:
        {
            pressed = false;
            break;
        }
    }

    
    if(milisecondsFromLastPacket >= 5.0)    //device not connected
    {
      lcd.setCursor(0,0);
      lcd.print("     ALERT!                ");
      lcd.setCursor(0,1);
      lcd.print("CONNECTION ERROR     ");
    } else if (frame[4] - 48 == 1) //crying handle
    {
        lcd.setCursor(0, 0);
        lcd.print("     ALERT!                ");
        lcd.setCursor(0, 1);
        lcd.print("CHILD IS CRYING     ");
        tone(1, 440, 100);
        delay(100);
    } else if (lowTempAlert || highTempAlert) //temp alert handle
    {
        lcd.setCursor(0, 0);
        lcd.print("     ALERT!                ");
        lcd.setCursor(0, 1);
        if (lowTempAlert)
            lcd.print("  TEMP TOO LOW     ");
        else if (highTempAlert)
            lcd.print(" TEMP TOO HIGH     ");
        tone(1, 440, 100);
        delay(100);
    } else if (alarmAlert) //alarm alert handle
    {
        lcd.setCursor(0, 0);
        lcd.print("     ALERT!                ");
        lcd.setCursor(0, 1);
        lcd.print("   It's ");
        if (t.hour() < 10)
            lcd.print("0");
        lcd.print(t.hour());
        lcd.print(":");
        if (t.minute() < 10)
            lcd.print("0");
        lcd.print(t.minute());
        lcd.print("                 ");
        tone(1, 440, 100);
        delay(100);

    } else {
        switch (menuStatus) {
        case 0: //temperature
            {
                lcd.setCursor(0, 0);
                lcd.print("Temperature:   ^");
                lcd.setCursor(0, 1);
                if (temp < 10)
                    lcd.print(" ");
                lcd.print(temp);
                lcd.print("\337C                ");
                lcd.setCursor(15, 1);
                lcd.print("v");
                break;
            }
        case 1: //humidity
            {
                lcd.setCursor(0, 0);
                lcd.println("Humidity:      ^");
                lcd.setCursor(0, 1);
                if (hum < 10)
                    lcd.print(" ");
                lcd.print(hum);
                lcd.print("%                ");
                lcd.setCursor(15, 1);
                lcd.print("v");
                break;
            }
        case 2: //set alarm
            {

                blinkTime++;
                lcd.setCursor(0, 0);
                lcd.println("Set alarm:     ^");
                lcd.setCursor(0, 1);

                if (blink == 0) {
                    if (h < 10)
                        lcd.print("0");
                    lcd.print(h);
                    lcd.print(":");
                    if (m < 10)
                        lcd.print("0");
                    lcd.print(m);
                } else if (blink == 1) {
                    if (blinkTime < 40) {
                        if (h < 10)
                            lcd.print("0");
                        lcd.print(h);
                    } else
                        lcd.print("  ");
                    if (blinkTime > 80)
                        blinkTime = 0;
                    lcd.print(":");
                    if (m < 10)
                        lcd.print("0");
                    lcd.print(m);
                } else if (blink == 2) {
                    if (h < 10)
                        lcd.print("0");
                    lcd.print(h);
                    lcd.print(":");

                    if (blinkTime < 40) {
                        if (m < 10)
                            lcd.print("0");
                        lcd.print(m);
                    } else
                        lcd.print("  ");
                    if (blinkTime > 80)
                        blinkTime = 0;
                }

                lcd.print("             ");
                lcd.setCursor(15, 1);
                lcd.print("v");
                break;
            }
        case 3: //show alarms
            {
                lcd.setCursor(0, 0);
                lcd.println("Your alarms:   ^");
                lcd.setCursor(0, 1);
                if (alarmsNum == 0)
                    lcd.print("none");

                else {
                    if (EEPROM.read(alarmsCount) < 10)
                        lcd.print("0");
                    lcd.print(EEPROM.read(alarmsCount));
                    lcd.print(":");
                    if (EEPROM.read(alarmsCount + 1) < 10)
                        lcd.print("0");
                    lcd.print(EEPROM.read(alarmsCount + 1));
                }
                lcd.print("                  ");
                lcd.setCursor(15, 1);
                lcd.print("v");

                if (canDeleteAlarm) {
                    int tempH = EEPROM.read(alarmsNum - 2);
                    int tempM = EEPROM.read(alarmsNum - 1);
                    EEPROM.write(alarmsNum - 2, 255);
                    EEPROM.write(alarmsNum - 1, 255);

                    if (alarmsNum > 2)
                    {
                      EEPROM.write(alarmsCount, tempH);
                      EEPROM.write(alarmsCount + 1, tempM);
                    }

                    alarmsNum -= 2;
                    canDeleteAlarm = false;
                }
                break;
            }
        case 4: //current time and date
            {

                lcd.setCursor(0, 0);
                lcd.println("Current time:  ^");
                lcd.setCursor(0, 1);

                if (t.day() < 10)
                    lcd.print("0");
                lcd.print(t.day());

                lcd.print(".");

                if (t.month() < 10)
                    lcd.print("0");
                lcd.print(t.month());
                lcd.print(" ");

                if (t.hour() < 10)
                    lcd.print("0");
                lcd.print(t.hour());
                lcd.print(":");

                if (t.minute() < 10)
                    lcd.print("0");
                lcd.print(t.minute());
                lcd.print(":");

                if (t.second() < 10)
                    lcd.print("0");
                lcd.print(t.second());

                lcd.print("       ");
                lcd.setCursor(15, 1);
                lcd.print("v");
                break;
            }
        }
    }
}
