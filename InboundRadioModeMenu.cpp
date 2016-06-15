#include "InboundRadioModeMenu.h"
#include "LCDStates.h"
#include "ModuleDefs.h"

char InboundRadioModeMenu::Modes[][17] {{"P2P, retry"},{"P2P, fire/forget"},{"P2M, retry"}};

InboundRadioModeMenu::InboundRadioModeMenu() : AbstractState()
{
  radioModeIndex = 0;
}

void InboundRadioModeMenu::Init()
{
  AbstractState::Init();
    
  ClearScreen();
  initRadioModeIndex();
 
  printMenu();
}

AbstractState* InboundRadioModeMenu::Tick()
{
   if (millis()-millisAtInit > 10000)
   {
      return (AbstractState*)&LCDStates::TheMainMenu;
   }
   return NULL;
}

void InboundRadioModeMenu::initRadioModeIndex()
{
  radioModeIndex = Settings::GetInboundRadioMode();
}

void InboundRadioModeMenu::printMenu()
{
  lcd.setCursor(0,0);
  lcd.print("Inb. radio mode:");
  lcd.setCursor(0,1);
  lcd.print("                ");
  lcd.setCursor(0,1);
  lcd.print(Modes[radioModeIndex]);
}

AbstractState* InboundRadioModeMenu::Select()
{
  resetTime();
  Settings::SetInboundRadioMode(radioModeIndex);
  LCDStates::TheSplashMenu.Init("Saved", false);
  return (AbstractState*)&LCDStates::TheSplashMenu;
}

AbstractState* InboundRadioModeMenu::Up()
{
  resetTime();
  if (radioModeIndex > 0)
  {
    radioModeIndex--;
  }
  printMenu();
  return NULL;
}

AbstractState* InboundRadioModeMenu::Down()
{
  resetTime();
  if (radioModeIndex < 2)
  {
    radioModeIndex++;
  }
  printMenu();
  return NULL;
}

AbstractState* InboundRadioModeMenu::Left()
{
  resetTime();
  return (AbstractState*)&LCDStates::TheSettingsMenu;
}
