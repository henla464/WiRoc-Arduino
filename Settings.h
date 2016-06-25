#ifndef __Settings_H__
#define __Settings_H__
#include <Arduino.h>


struct NodeSettings {
  uint8_t NodeNumber;
  char OutboundChannel[3];
  char InboundChannel[3];
  uint8_t NumberOfInboundRadioNodes;
  uint8_t InboundRadioMode;
  uint8_t OutboundRadioMode;
  uint8_t InboundRadioNodes[5];
};

struct HWSettings {
  uint8_t HWVersion;
};

class Settings
{
  public:
    static uint32_t RadioTotalSlotLengthMicroSeconds[];
    static bool InboundChannelChanged;
    static bool OutboundChannelChanged;
    static void Init();
    static uint32_t GetOutboundRadioTotalSlotLengthMicroSeconds();
    static uint32_t GetInboundRadioTotalSlotLengthMicroSeconds();
    static char* GetOutboundChannel();
    static char* GetInboundChannel();
    static void SetOutboundChannel(char channel[3]);
    static void SetInboundChannel(char channel[3]);
    static void SetInboundRadioMode(uint8_t radioModeIndex);
    static uint8_t GetInboundRadioMode();
    static void SetOutboundRadioMode(uint8_t radioModeIndex);
    static uint8_t GetOutboundRadioMode();
    static uint8_t GetNodeNumber();
    static void SetNodeNumber(uint8_t nodeNumber);
    static uint8_t GetHWVersion();
    static void SetHWVersion(uint8_t hwVersion);
    static uint8_t GetInboundRadioNodes(uint8_t* nodes);
    static void SetInboundRadioNodes(uint8_t* nodes, uint8_t numberOfNodes);
    static void SaveSettings();
  private:
    static struct NodeSettings settings;
    static struct HWSettings hwSettings;
};

#endif // __Settings_H__
