#include "OutboundChannelMenu.h"
#include "LCDStates.h"
#include "ModuleDefs.h"

OutboundChannelMenu::OutboundChannelMenu() : AbstractState()
{
  channelIndex = 0;
}

void OutboundChannelMenu::Init()
{
  AbstractState::Init();
  
  ClearScreen();
  initChannelIndex();
  lcd.setCursor(0,0);
  lcd.print("Outb. Channel: ");
   
  printChannel();
}

AbstractState* OutboundChannelMenu::Tick()
{
   if (millis()-millisAtInit > 10000)
   {
      return (AbstractState*)&LCDStates::TheMainMenu;
   }
   return NULL;
}

void OutboundChannelMenu::initChannelIndex()
{
  char* outboundChannel = Settings::GetOutboundChannel();
  for(uint8_t i = 0; i < 24; i++)
  {
    if (Radio::Channels[i][0] == outboundChannel[0] &&
        Radio::Channels[i][1] == outboundChannel[1])
    {
      channelIndex = i;      
    }
  }
}

void OutboundChannelMenu::printChannel()
{
  lcd.setCursor(14,0);
  lcd.print(Radio::Channels[channelIndex]);
}

AbstractState* OutboundChannelMenu::Select()
{
  resetTime();
  Settings::SetOutboundChannel(Radio::Channels[channelIndex]);
  LCDStates::TheSplashMenu.Init("Saved", false);
  return (AbstractState*)&LCDStates::TheSplashMenu;
}

AbstractState* OutboundChannelMenu::Up()
{
  resetTime();
  if (channelIndex > 0)
  {
    channelIndex--;
  }
  printChannel();
  return NULL;
}

AbstractState* OutboundChannelMenu::Down()
{
  resetTime();
  if (channelIndex < 23)
  {
    channelIndex++;
  }
  printChannel();
  return NULL;
}

AbstractState* OutboundChannelMenu::Left()
{
  resetTime();
  return (AbstractState*)&LCDStates::TheSettingsMenu;
}
