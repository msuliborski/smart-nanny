#include "LiquidCrystal.h"
#include "Time.h"
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

int menuStatus = 0;

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
  static unsigned char up_arrow[8] = {0x04, 0x0E, 0x15, 0x04, 0x04, 0x04, 0x04, 0x00};
static unsigned char down_arrow[8] = {0x00, 0x04, 0x04, 0x04, 0x04, 0x15, 0x0E, 0x04};


 lcd.begin(16, 2);
 lcd.setCursor(0,0);
 lcd.print("Welcome to");
 lcd.setCursor(0,1);
 lcd.print("smart-nanny");
 delay(2000);
 lcd.clear();
 lcd.setCursor(0,1);
 menuStatus = 0;
}
 
void loop()
{
 lcd_key = read_LCD_buttons();
 
 
 switch (lcd_key)
 {
     case btnRIGHT:
     {
     pressed = true;
     break;
     }
     case btnLEFT:
     {
     pressed = true;
     break;
     }
     case btnUP:
     {
      if(pressed == false)
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
      if(pressed == false)
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
      int hour = 12;
      int minutes = 0;
      lcd.setCursor(0,0);
      lcd.println("Set alarm:     ^");
      lcd.setCursor(0,1);
      
      lcd.print(hour);
      lcd.print(":");
      if(minutes < 10)
        lcd.print("0");
      lcd.print(minutes);
      lcd.print("             ");
      lcd.setCursor(15,1);
      lcd.print("v");
      break;
    }
    case 3:   //show alarms
    {
      lcd.setCursor(0,0);
      lcd.println("Show alarms:   ^");
      lcd.setCursor(0,1);
      
      lcd.print("%                ");
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
      if(hour(t) < 10)
        lcd.print("0");
      lcd.print(hour(t));
      lcd.print(":");
      if(minute(t) < 10)
        lcd.print("0");
      lcd.print(minute(t));
      lcd.print(":");
      if(second(t) < 10)
        lcd.print("0");
      lcd.print(second(t));
      lcd.print("       ");
      lcd.setCursor(15,1);
      lcd.print("v");
      break;
    }
  }
 }
