#ifndef __SIStation_H__
#define __SIStation_H__

#include <CP210x.h>
#include <SPI.h>
#include <Usb.h>
#include "Util.h"

class CP210xAsync : public CP210xAsyncOper
{
  public:
    uint8_t OnInit(CP210x *cp210x);
};



class SIStation
{
  private:
    uint8_t receivedData[19];
    uint8_t totalReceived;
    bool IsCorrectMSModeDirectResponse(uint8_t cnt, uint8_t *data);
    void GetDataToReceivedSIMessage(uint8_t expectedTotalLength);
  public:
    void Init();
    uint8_t GetSIPunchData(SIPunchMessage *siPunchMessage);
    void GetTestSIPunchMessage(SIPunchMessage *siPunchMessage, uint16_t stationCode, uint32_t cardNumber);
};

extern USB              Usb;
extern CP210xAsync      Cp210xAsync;
extern CP210x           Cp210x;

#endif // __SIStation_H__
