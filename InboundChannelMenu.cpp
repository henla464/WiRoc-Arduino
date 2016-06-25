#include "InboundChannelMenu.h"
#include "LCDStates.h"
#include "ModuleDefs.h"

InboundChannelMenu::InboundChannelMenu() : AbstractState()
{
  channelIndex = 0;
}

void InboundChannelMenu::Init()
{
  AbstractState::Init();
    
  ClearScreen();
  initChannelIndex();
  lcd.setCursor(0,0);
  lcd.print("Inb. Channel: ");

  printChannel();
}

AbstractState* InboundChannelMenu::Tick()
{
   if (millis()-millisAtInit > 10000)
   {
      return (AbstractState*)&LCDStates::TheMainMenu;
   }
   return NULL;
}

void InboundChannelMenu::initChannelIndex()
{
  char* inboundChannel = Settings::GetInboundChannel();
  for(uint8_t i = 0; i < 24; i++)
  {
    if (Radio::Channels[i][0] == inboundChannel[0] &&
        Radio::Channels[i][1] == inboundChannel[1])
    {
      channelIndex = i;      
    }
  }
}

void InboundChannelMenu::printChannel()
{
  lcd.setCursor(14,0);
  lcd.print(Radio::Channels[channelIndex]);
}

AbstractState* InboundChannelMenu::Select()
{
  resetTime();
  Settings::SetInboundChannel(Radio::Channels[channelIndex]);
  LCDStates::TheSplashMenu.Init("Saved", false);
  return (AbstractState*)&LCDStates::TheSplashMenu;
}

AbstractState* InboundChannelMenu::Up()
{
  resetTime();
  if (channelIndex > 0)
  {
    channelIndex--;
  }
  printChannel();
  return NULL;
}

AbstractState* InboundChannelMenu::Down()
{
  resetTime();
  if (channelIndex < 23)
  {
    channelIndex++;
  }
  printChannel();
  return NULL;
}

AbstractState* InboundChannelMenu::Left()
{
  resetTime();
  return (AbstractState*)&LCDStates::TheSettingsMenu;
}
