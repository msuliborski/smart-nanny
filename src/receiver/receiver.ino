#include "LiquidCrystal.h"
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
  lcd.setCursor(0,0);
  lcd.print(menuStatus);
 }
