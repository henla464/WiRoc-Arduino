#include "TestSendInSlotMenu.h"
#include "LCDStates.h"
#include "ModuleDefs.h"
#include "LCDMachine.h"

TestSendInSlotMenu::TestSendInSlotMenu() : AbstractState()
{
  interval = 10;
  lastTimeSent = millis();
}

void TestSendInSlotMenu::Init()
{
  AbstractState::Init();
  
  ClearScreen();
  lcd.setCursor(0,0);
  lcd.print("Interval: ");
  
  printMenu();
}

AbstractState* TestSendInSlotMenu::Tick()
{
  if (isSending)
  {
    if (millis()-lastTimeSent > 1000 * interval)
    {
      lastTimeSent = millis();
      // create message and save to db
      cardNumber++;
      SIPunchMessage siMessage;
      //Serial.print("get test message "); Serial.print(cardNumber);
      LCDMachine::Station->GetTestSIPunchMessage(&siMessage, Settings::GetNodeNumber() + 100, cardNumber);
      //Serial.print("Got message: ");
      //for (int i= 0; i < sizeof(SIPunchMessage); i++) {
      //      Serial.print(((uint8_t *)&siMessage)[i], HEX); Serial.print(" ");
      //}
      //Serial.println("");  
      MessageDatabase::SaveSIPunchMessage(&siMessage);
      //Serial.println("saved message");
    }
  }
  return NULL;
}

void TestSendInSlotMenu::printMenu()
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

AbstractState* TestSendInSlotMenu::Select()
{
  isSending = !isSending;
  printMenu();
  return NULL;
}

AbstractState* TestSendInSlotMenu::Up()
{
  interval++;
  printMenu();
  return NULL;
}

AbstractState* TestSendInSlotMenu::Down()
{
  interval--;
  printMenu();
  return NULL;
}

AbstractState* TestSendInSlotMenu::Left()
{
  return (AbstractState*)&LCDStates::TheTestMenu;
}
