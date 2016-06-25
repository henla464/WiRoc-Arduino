#include "InboundRadioNodeMenu.h"
#include "LCDStates.h"
#include "ModuleDefs.h"

InboundRadioNodeMenu::InboundRadioNodeMenu() : AbstractState()
{
  cursorIndex = 0;
  firstRowIndex = 0;
  numberOfNodes = 0;
  initialized = false; 
}

void InboundRadioNodeMenu::Init()
{
  AbstractState::Init();
  if (!initialized) {
    numberOfNodes = Settings::GetInboundRadioNodes(nodes);
    initialized = true;
  }
  printArrow();
  printMenu();
}

void InboundRadioNodeMenu::printArrow()
{
  ClearScreen();
  if (firstRowIndex == cursorIndex)
  {
    lcd.setCursor(0,0);
    lcd.print("> ");
    lcd.setCursor(0,1);
    lcd.print("  ");
  } 
  else 
  {
    lcd.setCursor(0,0);
    lcd.print("  ");
    lcd.setCursor(0,1);
    lcd.print("> ");  
  }
}

void InboundRadioNodeMenu::printMenu()
{
  if (firstRowIndex < numberOfNodes) 
  {
    lcd.setCursor(2,0);
    lcd.print(nodes[firstRowIndex]);
  }
  if (firstRowIndex+1 < numberOfNodes) 
  {
    lcd.setCursor(2,1);
    lcd.print(nodes[firstRowIndex+1]);
  }
  if (firstRowIndex == numberOfNodes)
  {
    lcd.setCursor(2,0);
    lcd.print("Add new node");
    lcd.setCursor(2,1);
    lcd.print("Save");
  } else if (firstRowIndex+1 == numberOfNodes)
  {
    lcd.setCursor(2,1);
    lcd.print("Add new node");
  }
}



AbstractState* InboundRadioNodeMenu::Tick()
{
   if (millis()-millisAtInit > 10000)
   {
      return (AbstractState*)&LCDStates::TheMainMenu;
   }
   return NULL;
}

AbstractState* InboundRadioNodeMenu::Select()
{
  resetTime();

  if (cursorIndex < numberOfNodes)
  {
    for(uint8_t i = cursorIndex; i+1 < numberOfNodes;i++)
    {
      nodes[i] = nodes[i+1];
    }
    numberOfNodes--;
    LCDStates::TheSplashMenu.Init("Removed", &LCDStates::TheInboundRadioNodeMenu);
    return (AbstractState*)&LCDStates::TheSplashMenu;
  } else if (cursorIndex == numberOfNodes)
  {
    // add new
    if (numberOfNodes < 5)
    {
      return (AbstractState*)&LCDStates::TheAddNodeMenu;
    } 
    else 
    {
      LCDStates::TheSplashMenu.Init("List full", &LCDStates::TheInboundRadioNodeMenu);
      return (AbstractState*)&LCDStates::TheSplashMenu;
    }
    
  } else if (cursorIndex == numberOfNodes+1)
  {
    Settings::SetInboundRadioNodes(nodes, numberOfNodes);
    LCDStates::TheSplashMenu.Init("Saved", &LCDStates::TheInboundRadioNodeMenu);
    return (AbstractState*)&LCDStates::TheSplashMenu;
  }
  return NULL;
}


AbstractState* InboundRadioNodeMenu::Up()
{
  resetTime();
  if (cursorIndex > 0)
  {
    cursorIndex--;
    if (cursorIndex < firstRowIndex)
    {
      firstRowIndex--;
    }
    printArrow();
    printMenu();
  }
  return NULL;
}


AbstractState* InboundRadioNodeMenu::Down()
{
  resetTime();
  if (cursorIndex < numberOfNodes+1)
  {
    if (cursorIndex > firstRowIndex)
    {
      firstRowIndex++;
    }
    cursorIndex++;
    printArrow();
    printMenu();
  }
  return NULL;
}


AbstractState* InboundRadioNodeMenu::Left()
{
  resetTime();
  return (AbstractState*)&LCDStates::TheSettingsMenu;
}


bool InboundRadioNodeMenu::AddNode(uint8_t nodeNumber)
{
  resetTime();
  if (numberOfNodes < 5)
  {
    bool nodeExistsInList = false;
    for(uint8_t i = 0; i < 5; i++)
    {
      if (nodes[i] == nodeNumber) 
      {
        nodeExistsInList = true;
      }
    }
    if (!nodeExistsInList)
    {
      nodes[numberOfNodes] = nodeNumber;
      numberOfNodes++;
      return true;
    }
  }
  return false;
}


