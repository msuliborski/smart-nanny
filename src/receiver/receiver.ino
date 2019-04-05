#include "LiquidCrystal.h"
#include "Time.h"
#include <EEPROM.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

int lcd_key     = 0;
int adc_key_in  = 0;
bool pressed = false;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

int alarms[15];
int alarmsNum = 0;
int alarmsCount = 0;
int menuStatus = 0;
int blink = 0;
int blinkTime = 0;
int h = 12;
int m = 0;

int read_LCD_buttons()
{
 adc_key_in = analogRead(0);
 if (adc_key_in > 1000) return btnNONE;
 if (adc_key_in < 50)   return btnRIGHT;  
 if (adc_key_in < 250)  return btnUP; 
 if (adc_key_in < 450)  return btnDOWN; 
 if (adc_key_in < 650)  return btnLEFT; 
 if (adc_key_in < 850)  return btnSELECT;  
 return btnNONE;
}

void setup()
{
//for (int i = 0 ; i < EEPROM.length() ; i++) {
//    EEPROM.write(i, 255);
//  }
 lcd.begin(16, 2);
 Serial.begin(9600);
 lcd.setCursor(0,0);
 lcd.print("Welcome to");
 lcd.setCursor(0,1);
 lcd.print("smart-nanny");
 delay(2000);
 lcd.clear();
 lcd.setCursor(0,1);
 menuStatus = 0;
 tone(7, 440, 300);
 while(EEPROM.read(alarmsNum) != 255)
  alarmsNum++;
}
 
void loop()
{
 lcd_key = read_LCD_buttons();
 
 
 switch (lcd_key)
 {
     case btnRIGHT:
     {
      if(!pressed)
      {
        if(menuStatus == 2)
        {
          if(blink == 1)
          {
            h++;
            if(h == 24)
              h = 0;
          }
          else if(blink == 2)
          {
            m++;
            if(m == 60)
              m = 0;
          }
        }
        else if(menuStatus == 3)
        {
          alarmsCount += 2;
          if(alarmsCount == alarmsNum)
            alarmsCount = 0;
        }
        pressed = true;
      }
      break;
     }
     case btnLEFT:
     {
      if(!pressed)
      {
        if(menuStatus == 2)
        {
          if(blink == 1)
          {
            h--;
            if(h == -1)
              h = 23;
          }
          else if(blink == 2)
          {
            m--;
            if(m == -1)
              m = 59;
          }
        }
        else if(menuStatus == 3)
        {
          alarmsCount -= 2;
          if(alarmsCount < 0)
            alarmsCount = alarmsNum - 2;
        }
        pressed = true;
      }
      break;
     }
     case btnUP:
     {
      if(!pressed)
      {
       pressed = true;
       menuStatus++;
       if(menuStatus == 5)
        menuStatus = 0;
      }
      break;
     }
     case btnDOWN:
     {
      if(!pressed)
      {
       pressed = true;
       menuStatus--;
       if(menuStatus == -1)
        menuStatus = 4;
      }
      
      break;
     }
     case btnSELECT:
     {
      if(!pressed)
      {
        pressed = true;
        if(menuStatus == 2)
        {
          blink++;
          if(blink == 3)
          {
            blink = 0;
            EEPROM.write(alarmsNum, h);
            EEPROM.write(alarmsNum + 1, m);
            //alarms[alarmsNum] = h*100 + m;
            alarmsNum += 2;
          }
            
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

  switch(menuStatus)
  {
    case 0:   //temperature
    {
      lcd.setCursor(0,0);
      lcd.print("Temperature:   ^");
      lcd.setCursor(0,1);
      int temp = 7;
      lcd.print(temp);
      lcd.print("\337C                ");
      lcd.setCursor(15,1);
      lcd.print("v");
      break;
    }
    case 1:   //humidity
    {
      lcd.setCursor(0,0);
      lcd.println("Humidity:      ^");
      lcd.setCursor(0,1);
      int hum = 35;
      lcd.print(hum);
      lcd.print("%                ");
      lcd.setCursor(15,1);
      lcd.print("v");
      break;
    }
    case 2:   //set alarm
    {
      
      blinkTime++;
      lcd.setCursor(0,0);
      lcd.println("Set alarm:     ^");
      lcd.setCursor(0,1);

      if(blink == 0)
      {
        if(h < 10)
          lcd.print("0");
        lcd.print(h);
        lcd.print(":");
        if(m < 10)
          lcd.print("0");
        lcd.print(m);
      }
      else if(blink == 1)
      {
        if(blinkTime<40)
        {
          if(h < 10)
            lcd.print("0");
          lcd.print(h);
        }
        else
          lcd.print("  ");
        if(blinkTime > 80)
          blinkTime = 0;
        lcd.print(":");
        if(m < 10)
          lcd.print("0");
        lcd.print(m);
      }
      else if(blink == 2)
      {
        if(h < 10)
          lcd.print("0");
        lcd.print(h);
        lcd.print(":");
        
        if(blinkTime<40)
        {
          if(m < 10)
            lcd.print("0");
          lcd.print(m);
        }
          
        else
          lcd.print("  ");
        if(blinkTime > 80)
          blinkTime = 0;
      }
      
      lcd.print("             ");
      lcd.setCursor(15,1);
      lcd.print("v");
      break;
    }
    case 3:   //show alarms
    {
      lcd.setCursor(0,0);
      lcd.println("Your alarms:   ^");
      lcd.setCursor(0,1);
      if(alarmsNum == 0)
        lcd.print("none");

      else
      {
        if(EEPROM.read(alarmsCount) < 10)
          lcd.print("0");
        lcd.print(EEPROM.read(alarmsCount));
        lcd.print(":");
        if(EEPROM.read(alarmsCount + 1) < 10)
          lcd.print("0");
        lcd.print(EEPROM.read(alarmsCount + 1));
      }
      lcd.print("                  ");
      lcd.setCursor(15,1);
      lcd.print("v");
      break;
    }
    case 4:   //current time and date
    {
      lcd.setCursor(0,0);
      lcd.println("Current time:  ^");
      lcd.setCursor(0,1);

      time_t t = now();
      
      if(day(t) < 10)
        lcd.print("0");
      lcd.print(day(t));
      lcd.print(".");

      if(month(t) < 10)
        lcd.print("0");
      lcd.print(month(t));
      lcd.print(".");

      if(year(t)%10 < 10)
        lcd.print("0");
      lcd.print(year(t)%10);
      lcd.print(" ");
      
      if(hour(t) < 10)
        lcd.print("0");
      lcd.print(hour(t));
      lcd.print(":");
      
      if(minute(t) < 10)
        lcd.print("0");
      lcd.print(minute(t));
      
      lcd.print("       ");
      lcd.setCursor(15,1);
      lcd.print("v");
      break;
    }
  }
 }
