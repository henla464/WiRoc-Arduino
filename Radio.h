#ifndef __Radio_H__
#define __Radio_H__

#include <Arduino.h>
#include <SoftwareSerial.h>
#include "Util.h"

class Radio
{
  private:
    static float _slopeCoefficient[6];
    static uint8_t _m[6];

    char _channel[3];
    //uint8_t receivedData[12];
    RadioMessage receivedMessage;
    RadioMessageSimpleAck receivedSimpleAckMessage;
    uint8_t totalReceived;
    uint8_t GetExpectedTotalLength();
    void GetRadioDataToReceivedData(uint8_t expectedTotalLength);
    void GetRadioDataToReceivedSimpleAck();
    void SendData(RadioMessage *message);
    void SendData(SIPunchMessage *message);
    uint8_t CalculateCS(uint8_t *data, uint8_t len);
  public:
    static char Channels[][3];
    HardwareSerial* radioSerial;
    Radio(HardwareSerial* serial);
    void Init(const char channel[3]);
    float GetSlopeCoefficient();
    uint8_t GetM();
    uint8_t GetRadioData(RadioMessage *message);
    void SendSIPunchDataTest(SIPunchMessage *message);
    void SendPunchData(RadioMessage *message);
    void SendTimeSlotMessage(uint8_t *timeSlotData, uint8_t dataLength);
    void SendSimpleAckMessage(uint8_t messageNumberToAck);
    uint8_t GetSimpleAckMessage(RadioMessageSimpleAck *message);
};



#endif // __Radio_H__
