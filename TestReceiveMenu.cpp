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
  lcd.print("Cnt:     ");
  lcd.setCursor(4,0);
  lcd.print(MessageDatabase::ReceiveCount);
  lcd.setCursor(9,0);
  lcd.print("Msg:     ");
  lcd.setCursor(13,0);
  lcd.print(MessageDatabase::LastMessageNumber);
  lcd.setCursor(0,1);
  lcd.print("Crd:    ");
  lcd.setCursor(4,1);
  lcd.print(MessageDatabase::LastSICardNumber);
  lcd.setCursor(11,1);
  lcd.print("Fr:    ");
  lcd.setCursor(14,1);
  lcd.print(MessageDatabase::LastFromNode);

}

AbstractState* TestReceiveMenu::Left()
{
  return (AbstractState*)&LCDStates::TheTestMenu;
}
