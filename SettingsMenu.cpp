#include "SettingsMenu.h"
#include "LCDStates.h"

SettingsMenu::SettingsMenu() : AbstractState()
{
  cursorIndex = 0;
  firstRowIndex = 0;
  
}

char SettingsMenu::SettingMenuItems[][15] {{"Outb. Channel"},
                                            {"Outb. mode"},
                                            {"Node number"},
                                            {"Battery info"},
                                            {"System info"},
                                            {"Test"},
                                            {"Erase database"},
                                            {"Inb. Channel"},
                                            {"Inb. mode"},
                                            {"Inb.rdio nodes"}};


void SettingsMenu::Init()
{
  AbstractState::Init();
  printMenu();
  printArrow();
}

void SettingsMenu::printMenu()
{
  ClearScreen();
  lcd.setCursor(2,0);
  lcd.print(SettingsMenu::SettingMenuItems[firstRowIndex]);
  lcd.setCursor(2,1);
  lcd.print(SettingsMenu::SettingMenuItems[firstRowIndex+1]);
}

void SettingsMenu::printArrow()
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


AbstractState* SettingsMenu::Tick()
{
   if (millis()-millisAtInit > 10000)
   {
      return (AbstractState*)&LCDStates::TheMainMenu;
   }
   return NULL;
}

AbstractState* SettingsMenu::GetSelected()
{
  if (cursorIndex == 0) 
  {
    return (AbstractState*)&LCDStates::TheOutboundChannelMenu;
  } else if (cursorIndex == 1)
  {
    return (AbstractState*)&LCDStates::TheOutboundRadioModeMenu;
  } else if (cursorIndex == 2)
  {
    return (AbstractState*)&LCDStates::TheStationNumberMenu;
  } else if (cursorIndex == 3)
  {
    return (AbstractState*)&LCDStates::TheBatteryMenu;
  } else if (cursorIndex == 4)
  {
    return (AbstractState*)&LCDStates::TheSystemInfoMenu;
  } else if (cursorIndex == 5)
  {
    return (AbstractState*)&LCDStates::TheTestMenu;
  } else if (cursorIndex == 6)
  {
    return (AbstractState*)&LCDStates::TheEraseDBMenu;
  } else if (cursorIndex == 7)
  {
    return (AbstractState*)&LCDStates::TheInboundChannelMenu;
  } else if (cursorIndex == 8)
  {
    return (AbstractState*)&LCDStates::TheInboundRadioModeMenu;
  } else if (cursorIndex == 9)
  {
    return (AbstractState*)&LCDStates::TheInboundRadioNodeMenu;
  }
  return NULL;
}

AbstractState* SettingsMenu::Select()
{
  resetTime();
  return GetSelected();
}


AbstractState* SettingsMenu::Down()
{
  resetTime();
  #ifdef IS_RELAY_MODULE
    #define NUMBER_OF_MENUITEMS 10
  #endif
  #ifdef IS_STATION_MODULE
   #define NUMBER_OF_MENUITEMS 7
  #endif
  
  if (cursorIndex < NUMBER_OF_MENUITEMS-1)
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

AbstractState* SettingsMenu::Up()
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


AbstractState* SettingsMenu::Left()
{
  resetTime();
  return (AbstractState*)&LCDStates::TheMainMenu;
}

AbstractState* SettingsMenu::Right()
{
  resetTime();
  return GetSelected();
}
