#ifndef __Util_H__
#define __Util_H__
#include <Arduino.h>
#include "ModuleDefs.h"


#pragma pack (push, 1)
struct RadioMessage {
  uint8_t stx;
  uint8_t dataLength : 6;
  uint8_t messageVersion : 2;
  uint8_t fromNode : 5;
  uint8_t messageType : 3;
  uint8_t messageNumber;
  uint8_t checkSum;
  uint8_t data[18];
};
#pragma pack(pop)

#pragma pack (push, 1)
struct RadioMessageSimpleAck {
  uint8_t stx;
  uint8_t dataLength : 6;
  uint8_t messageVersion : 2;
  uint8_t messageNumberToAck;
  uint8_t checkSum;
};
#pragma pack(pop)

#pragma pack (push, 1)
struct RadioMessageDataRecord {
  uint32_t siCardNumber;
  uint8_t twentyFourHour : 1;
  uint8_t weekDay : 3;
  uint8_t fourWeekCounter : 2;
  uint8_t fill : 2;
  uint16_t twelveHourTimer;
  uint8_t subSecond;
};
#pragma pack(pop)

#pragma pack (push, 1)
struct RadioMessageDataTimeSlotRecord {
  uint8_t timeSlotLength : 3;
  uint8_t nodeNumber: 5;
  uint8_t messageNumber;
};
#pragma pack(pop)

#pragma pack (push, 1)
struct RadioMessageDataCombinedRecord {
  uint32_t siCardNumber;
  uint8_t twentyFourHour : 1;
  uint8_t weekDay : 3;
  uint8_t fourWeekCounter : 2;
  uint8_t fill : 2;
  uint16_t twelveHourTimer;
  uint8_t subSecond;
  uint8_t origFromNode;
};
#pragma pack(pop)

#pragma pack (push, 1)
struct SIPunchMessage {
  uint8_t stx;
  uint8_t commandCode;
  uint8_t len;
  uint16_t stationCode;
  RadioMessageDataRecord dataRecord;
  uint8_t mem2;
  uint8_t mem1;
  uint8_t mem0;
  uint16_t crc;
  uint8_t etx;
};
#pragma pack(pop)



class Util
{
  private:
    static uint8_t MessageNumber;
  public:
    static const uint16_t TimeSlotMargin = 10000;
    static unsigned int crc(unsigned int uiCount, unsigned char *pucDat);
    static void InitRadioMessage(RadioMessage *message, uint8_t messageType);
    static void InitRadioMessageSimpleAck(RadioMessageSimpleAck *message);
    static uint8_t GetNextMessageNumber();
    static uint8_t GetCurrentMessageNumber();
    
   // static void ConvertToRelayMessage(RadioMessage *message);
};

#endif // __Util_H__
