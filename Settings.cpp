#include <EEPROM.h>

#include "Settings.h"
#include <Arduino.h>


struct NodeSettings Settings::settings;
struct HWSettings Settings::hwSettings;

uint32_t Settings::RadioTotalSlotLengthMicroSeconds[] = {5800000, 5800000, 5800000, 5800000, 5800000, 5800000};

void Settings::Init()
{
  EEPROM.get(0, settings);
  EEPROM.get(50, hwSettings);
}

uint32_t Settings::GetOutboundRadioTotalSlotLengthMicroSeconds()
{
  return RadioTotalSlotLengthMicroSeconds[settings.OutboundChannel[1]-'0'-1];
}

uint32_t Settings::GetInboundRadioTotalSlotLengthMicroSeconds()
{
  return RadioTotalSlotLengthMicroSeconds[settings.InboundChannel[1]-'0'-1];
}

char* Settings::GetOutboundChannel()
{
  return settings.OutboundChannel;
}

char* Settings::GetInboundChannel()
{
  return settings.InboundChannel;
}

void Settings::SetOutboundChannel(char channel[3])
{
  settings.OutboundChannel[0] = channel[0];
  settings.OutboundChannel[1] = channel[1];
  settings.OutboundChannel[2] = channel[2];
  SaveSettings();
}

void Settings::SetInboundChannel(char channel[3])
{
  settings.InboundChannel[0] = channel[0];
  settings.InboundChannel[1] = channel[1];
  settings.InboundChannel[2] = channel[2];
  SaveSettings();
}

void Settings::SetInboundRadioMode(uint8_t radioModeIndex)
{
  settings.InboundRadioMode = radioModeIndex;
  SaveSettings();
}

uint8_t Settings::GetInboundRadioMode()
{
  if (settings.InboundRadioMode < 3)
  {
    return settings.InboundRadioMode;
  }
  return 0;
}

void Settings::SetOutboundRadioMode(uint8_t radioModeIndex)
{
  settings.OutboundRadioMode = radioModeIndex;
  SaveSettings();
}

uint8_t Settings::GetOutboundRadioMode()
{
  if (settings.OutboundRadioMode < 3)
  {
    return settings.OutboundRadioMode;
  }
  return 0;
}

uint8_t Settings::GetNodeNumber()
{
  return settings.NodeNumber;
}

void Settings::SetNodeNumber(uint8_t nodeNumber)
{
  settings.NodeNumber = nodeNumber;
  SaveSettings();
}

uint8_t Settings::GetInboundRadioNodes(uint8_t* nodes)
{
  memcpy(nodes, settings.InboundRadioNodes, sizeof(settings.InboundRadioNodes));
  if (settings.NumberOfInboundRadioNodes > sizeof(settings.InboundRadioNodes))
  {
    settings.NumberOfInboundRadioNodes = 0;
  }
  return settings.NumberOfInboundRadioNodes;
}

void Settings::SetInboundRadioNodes(uint8_t* nodes, uint8_t numberOfNodes)
{
  memcpy(settings.InboundRadioNodes, nodes, numberOfNodes);
  settings.NumberOfInboundRadioNodes = numberOfNodes;
  SaveSettings();
}

uint8_t Settings::GetHWVersion()
{
  return hwSettings.HWVersion;
}

void Settings::SetHWVersion(uint8_t hwVersion)
{
  hwSettings.HWVersion = hwVersion;
  EEPROM.put(50, hwSettings);
}
    

void Settings::SaveSettings()
{
  EEPROM.put(0, settings);
}




