#include "MainMenu.h"
#include "LCDStates.h"
#include "ModuleDefs.h"

MainMenu::MainMenu() : AbstractState()
{
  
}

void MainMenu::Init()
{
   ClearScreen();
   lcd.setCursor(0,0);
   #ifdef IS_STATION_MODULE
   lcd.print("Control");
   lcd.setCursor(7,0);
   if (usbIsConnected) {
     lcd.print(" -> ");
   } else {
     lcd.print(" !! ");
   }
   #else 
     char* inboundChannel = Settings::GetInboundChannel();
     lcd.print("CH:");
     lcd.print(inboundChannel);
     lcd.setCursor(7,0);
     lcd.print(" -> ");
   #endif
   
   char* outboundChannel = Settings::GetOutboundChannel();
   lcd.setCursor(11,0);
   lcd.print("CH:");
   lcd.print(outboundChannel);

   lcd.setCursor(0,1);
   lcd.print("Node:");
   lcd.print(Settings::GetNodeNumber());
   lcd.setCursor(8,1);
   lcd.print("Bat:");
   printNumbers();
}

void MainMenu::SetUSBConnected(bool usbConnected)
{
  usbIsConnected = usbConnected;
}

void MainMenu::printNumbers()
{
  lcd.setCursor(12,1);
  uint8_t p = Battery::GetBatteryPercentage();
  if (p < 100)
  {
    lcd.print(" ");
    if (p < 10)
    {
      lcd.print(" ");
    }
  }
  lcd.print(p);
  lcd.print("%");
}

AbstractState* MainMenu::Tick()
{
   if (millis()-lastUpdateMillis > 60000)
   {
      lastUpdateMillis = millis();
      printNumbers();
   }
   return NULL;  
}

AbstractState* MainMenu::Select()
{
    return (AbstractState*)&LCDStates::TheSettingsMenu;
}
