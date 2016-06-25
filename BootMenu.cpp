#include "OutboundChannelMenu.h"
#include "LCDStates.h"
#include "ModuleDefs.h"

//boot_states_t BootMenu::BootStates;
BootMenu::BootMenu() : AbstractState()
{
  BootState = NOT_INITIALIZED;
}

void BootMenu::Init()
{
  AbstractState::Init();
  
  ClearScreen();
  printMenu();
}

AbstractState* BootMenu::Tick()
{
   //return (AbstractState*)&LCDStates::TheMainMenu;
   printMenu();
   return NULL;
}


void BootMenu::printMenu()
{
  if (BootState == NOT_INITIALIZED)
  {
    printRow1("  INITIALIZING  ");
    printRow2("  ------------  ");
  } else if (BootState == RADIO)
  {
    if (IsConfiguringInboundRadio)
    {
      printRow1("  Inb. Radio   ");
    } else {
      printRow1("  Outb. Radio  ");
    }
    printRow2("                ");
  } else if (BootState == RADIO_ALREADY_CONFIGURED)
  {
    if (IsConfiguringInboundRadio)
    {
      printRow1("  Inb. Radio   ");
    } else {
      printRow1("  Outb. Radio  ");
    }
    printRow2("  Configured    ");
  } else if (BootState == RADIO_RECONFIGURED)
  {
    if (IsConfiguringInboundRadio)
    {
      printRow1("  Inb. Radio   ");
    } else {
      printRow1("  Outb. Radio  ");
    }
    printRow2("Restart required");
  } else if (BootState == SD_CARD) {
    printRow1("    SD Card     ");
    printRow2("  Initialized   ");
  } else if (BootState == SD_CARD_FAILED) {
    printRow1("    SD Card     ");
    printRow2("    Failed!     ");
  } else if (BootState == SD_CARD_DB_OPENED) {
    printRow1("   DB Opened    ");
    printRow2("                ");
  } else if (BootState == SD_CARD_DB_CREATED) {
    printRow1("   DB Created   ");
    printRow2("                ");
  } else if (BootState == SD_CARD_LOG_OPENED) {
    printRow1("   Log Opened   ");
    lcd.setCursor(3,1);
    lcd.print(LogFileName);
  } else if (BootState == CONTINUE_AFTER_ERROR) {
    printRow1("   Continuing   ");
  } else {
    lcd.setCursor(0,0);
    lcd.print(BootState);
  }
}

void BootMenu::printRow1(String txt)
{
  lcd.setCursor(0,0);
  lcd.print(txt);
}

void BootMenu::printRow2(String txt)
{
  lcd.setCursor(0,1);
  lcd.print(txt);
}


AbstractState* BootMenu::Select()
{
  if (BootState == RADIO_RECONFIGURED)
  {
    BootState = CONTINUE_AFTER_ERROR;
  } else if (BootState != SD_CARD_LOG_OPENED) {
    return (AbstractState*)&LCDStates::TheMainMenu;
  } 
  return NULL;
}
/*
AbstractState* BootMenu::Up()
{
  return NULL;
}

AbstractState* BootMenu::Down()
{
  return NULL;
}

AbstractState* BootMenu::Left()
{
  return NULL;
}*/
