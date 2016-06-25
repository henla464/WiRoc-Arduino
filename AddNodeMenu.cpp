#include "AddNodeMenu.h"
#include "LCDStates.h"
#include "ModuleDefs.h"

AddNodeMenu::AddNodeMenu() : AbstractState()
{
  nodeNumber = 1;
}

void AddNodeMenu::Init()
{
  AbstractState::Init();
  ClearScreen();
  lcd.setCursor(0,0);
  lcd.print("Node number: ");
   
  nodeNumber = 1;
  printNodeNumber();
}

void AddNodeMenu::printNodeNumber()
{
   lcd.setCursor(13,0);
   lcd.print("  ");
   lcd.setCursor(13,0);
   lcd.print(nodeNumber);
}

AbstractState* AddNodeMenu::Tick()
{
   if (millis()-millisAtInit > 10000)
   {
      return (AbstractState*)&LCDStates::TheMainMenu;
   }
   return NULL;
}

AbstractState* AddNodeMenu::Select()
{
  resetTime();
  if (LCDStates::TheInboundRadioNodeMenu.AddNode(nodeNumber))
  {
    LCDStates::TheSplashMenu.Init("Added", &LCDStates::TheInboundRadioNodeMenu);  
  }
  else
  {
    LCDStates::TheSplashMenu.Init("Node exists", &LCDStates::TheInboundRadioNodeMenu);  
  }
  return (AbstractState*)&LCDStates::TheSplashMenu;    
}

AbstractState* AddNodeMenu::Up()
{
  resetTime();
  if (nodeNumber < 31)
  {
    nodeNumber++;
    printNodeNumber();
  }
  return NULL;
}

AbstractState* AddNodeMenu::Down()
{
  resetTime();
  if (nodeNumber > 1)
  {
    nodeNumber--;
    printNodeNumber();
  }
  return NULL;
}

AbstractState* AddNodeMenu::Left()
{
  return (AbstractState*)&LCDStates::TheInboundRadioNodeMenu;
}
