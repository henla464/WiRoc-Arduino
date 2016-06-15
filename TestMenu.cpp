#include "TestMenu.h"
#include "LCDStates.h"

TestMenu::TestMenu() : AbstractState()
{
  cursorIndex = 0;
  firstRowIndex = 0;
}

char TestMenu::TestMenuItems[][15] {{"Send in slot"},{"Force send"},{"Receive"}};


void TestMenu::Init()
{
  AbstractState::Init();
  printMenu();
  printArrow();
}

void TestMenu::printMenu()
{
  ClearScreen();
  lcd.setCursor(2,0);
  lcd.print(TestMenu::TestMenuItems[firstRowIndex]);
  lcd.setCursor(2,1);
  lcd.print(TestMenu::TestMenuItems[firstRowIndex+1]);
}

void TestMenu::printArrow()
{
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


AbstractState* TestMenu::Tick()
{
   if (millis()-millisAtInit > 10000)
   {
      return (AbstractState*)&LCDStates::TheMainMenu;
   }
   return NULL;
}

AbstractState* TestMenu::GetSelected()
{
  if (cursorIndex == 0) 
  {
    return (AbstractState*)&LCDStates::TheTestSendInSlotMenu;
  } else if (cursorIndex == 1)
  {
    return (AbstractState*)&LCDStates::TheTestForceSendMenu;
  } else if (cursorIndex == 2)
  {
    return (AbstractState*)&LCDStates::TheTestReceiveMenu;
  }
  return NULL;
}

AbstractState* TestMenu::Select()
{
  resetTime();
  return GetSelected();
}


AbstractState* TestMenu::Down()
{
  resetTime();
  if (cursorIndex < 2)
  {
    if (cursorIndex > firstRowIndex)
    {
      firstRowIndex++;
      printMenu();
    }
    cursorIndex++;
    printArrow();
  }
  return NULL;
}

AbstractState* TestMenu::Up()
{
  resetTime();
  if (cursorIndex > 0)
  {
    cursorIndex--;
    if (cursorIndex < firstRowIndex)
    {
      firstRowIndex--;
      printMenu();
    }
    printArrow();
  }
  return NULL;
}


AbstractState* TestMenu::Left()
{
  resetTime();
  return (AbstractState*)&LCDStates::TheSettingsMenu;
}

AbstractState* TestMenu::Right()
{
  resetTime();
  return GetSelected();
}
