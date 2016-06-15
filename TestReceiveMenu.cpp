#include "TestReceiveMenu.h"
#include "LCDStates.h"
#include "ModuleDefs.h"

TestReceiveMenu::TestReceiveMenu() : AbstractState()
{
}

void TestReceiveMenu::Init()
{
  AbstractState::Init();
  
  ClearScreen();
  printMenu();
}

AbstractState* TestReceiveMenu::Tick()
{
   if (millis()-millisAtInit > 500)
   {
      printMenu();
   }
   return NULL;
}

void TestReceiveMenu::printMenu()
{
  lcd.setCursor(0,0);
  lcd.print("Cnt:");
  lcd.print(MessageDatabase::ReceiveCount);
  lcd.setCursor(9,0);
  lcd.print("Msg:");
  lcd.print(MessageDatabase::LastMessageNumber);
  lcd.setCursor(0,1);
  lcd.print("C no:");
  lcd.print(MessageDatabase::LastSICardNumber);
  lcd.setCursor(9,1);
  lcd.print("From:");
  lcd.print(MessageDatabase::LastFromNode);

}

AbstractState* TestReceiveMenu::Left()
{
  return (AbstractState*)&LCDStates::TheTestMenu;
}
