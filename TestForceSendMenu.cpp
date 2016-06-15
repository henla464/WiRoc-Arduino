#include "TestForceSendMenu.h"
#include "LCDStates.h"
#include "ModuleDefs.h"
#include "LCDMachine.h"

TestForceSendMenu::TestForceSendMenu() : AbstractState()
{
  interval = 10;
  lastTimeSent = millis();
}

void TestForceSendMenu::Init()
{
  AbstractState::Init();
  
  ClearScreen();
  lcd.setCursor(0,0);
  lcd.print("Interval: ");
  
  printMenu();
}

AbstractState* TestForceSendMenu::Tick()
{
  if (isSending)
  {
    if (millis()-lastTimeSent > 1000 * interval)
    {
      lastTimeSent = millis();
      cardNumber++;
      SIPunchMessage siMessage;
      LCDMachine::Station->GetTestSIPunchMessage(&siMessage, Settings::GetNodeNumber() + 100, cardNumber);
      LCDMachine::OutboundRadio->SendSIPunchDataTest(&siMessage);
    }
  }
  return NULL;
}

void TestForceSendMenu::printMenu()
{
  lcd.setCursor(10,0);
  lcd.print(interval);
  lcd.print("s  ");
  lcd.setCursor(0,1); 
  if (isSending)
  {
    lcd.print("Snd");
  } else {
    lcd.print("   ");
  }
}

AbstractState* TestForceSendMenu::Select()
{
  isSending = !isSending;
  printMenu();
  return NULL;
}

AbstractState* TestForceSendMenu::Up()
{
  interval++;
  printMenu();
  return NULL;
}

AbstractState* TestForceSendMenu::Down()
{
  interval--;
  printMenu();
  return NULL;
}

AbstractState* TestForceSendMenu::Left()
{
  return (AbstractState*)&LCDStates::TheTestMenu;
}
