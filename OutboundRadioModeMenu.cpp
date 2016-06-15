#include "OutboundRadioModeMenu.h"
#include "InboundRadioModeMenu.h"
#include "LCDStates.h"
#include "ModuleDefs.h"

OutboundRadioModeMenu::OutboundRadioModeMenu() : AbstractState()
{
  radioModeIndex = 0;
}

void OutboundRadioModeMenu::Init()
{
  AbstractState::Init();
    
  ClearScreen();
  initRadioModeIndex();
 
  printMenu();
}

AbstractState* OutboundRadioModeMenu::Tick()
{
   if (millis()-millisAtInit > 10000)
   {
      return (AbstractState*)&LCDStates::TheMainMenu;
   }
   return NULL;
}

void OutboundRadioModeMenu::initRadioModeIndex()
{
  radioModeIndex = Settings::GetOutboundRadioMode();
}

void OutboundRadioModeMenu::printMenu()
{
  lcd.setCursor(0,0);
  lcd.print("Outb. rdio mode:");
  lcd.setCursor(0,1);
  lcd.print("                ");
  lcd.setCursor(0,1);
  lcd.print(InboundRadioModeMenu::Modes[radioModeIndex]);
}

AbstractState* OutboundRadioModeMenu::Select()
{
  resetTime();
  Settings::SetOutboundRadioMode(radioModeIndex);
  LCDStates::TheSplashMenu.Init("Saved", false);
  return (AbstractState*)&LCDStates::TheSplashMenu;
}

AbstractState* OutboundRadioModeMenu::Up()
{
  resetTime();
  if (radioModeIndex > 0)
  {
    radioModeIndex--;
  }
  printMenu();
  return NULL;
}

AbstractState* OutboundRadioModeMenu::Down()
{
  resetTime();
  if (radioModeIndex < 2)
  {
    radioModeIndex++;
  }
  printMenu();
  return NULL;
}

AbstractState* OutboundRadioModeMenu::Left()
{
  resetTime();
  return (AbstractState*)&LCDStates::TheSettingsMenu;
}
