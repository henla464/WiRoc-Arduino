#include "StationNumberMenu.h"
#include "LCDStates.h"
#include "ModuleDefs.h"

StationNumberMenu::StationNumberMenu() : AbstractState()
{
  nodeNumber = 1;
}

void StationNumberMenu::Init()
{
  AbstractState::Init();
  ClearScreen();
  lcd.setCursor(0,0);
  lcd.print("Node number: ");
   
  nodeNumber = Settings::GetNodeNumber();
  if (nodeNumber > 31)
  {
    nodeNumber = 0;
    Settings::SetNodeNumber(nodeNumber);
  }
  printNodeNumber();
}

void StationNumberMenu::printNodeNumber()
{
   lcd.setCursor(13,0);
   lcd.print("  ");
   lcd.setCursor(13,0);
   lcd.print(nodeNumber);
}

AbstractState* StationNumberMenu::Tick()
{
   if (millis()-millisAtInit > 10000)
   {
      return (AbstractState*)&LCDStates::TheMainMenu;
   }
   return NULL;
}

AbstractState* StationNumberMenu::Select()
{
  resetTime();
  Settings::SetNodeNumber(nodeNumber);
  LCDStates::TheSplashMenu.Init("Saved", false);
  return (AbstractState*)&LCDStates::TheSplashMenu;
}

AbstractState* StationNumberMenu::Up()
{
  resetTime();
  if (nodeNumber < 31)
  {
    nodeNumber++;
    printNodeNumber();
  }
  return NULL;
}

AbstractState* StationNumberMenu::Down()
{
  resetTime();
  if (nodeNumber > 1)
  {
    nodeNumber--;
    printNodeNumber();
  }
  return NULL;
}

AbstractState* StationNumberMenu::Left()
{
  return (AbstractState*)&LCDStates::TheSettingsMenu;
}
