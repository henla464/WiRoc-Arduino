#ifndef __MessageDatabase_H__
#define __MessageDatabase_H__
#include <Arduino.h>
#include "Util.h"
#include <SD.h>
#include <EDB.h>

#define TABLE_SIZE 100000

enum EventType
{
  SIPunchMessageType,
  RadioMessageType
};

struct DatabaseMessage
{
  uint16_t id;
  uint8_t sent;
  uint8_t ackSent;
  uint8_t sentAsMessageNumber;
  enum EventType Type;
  union
  {
    struct RadioMessage radioMessage;
    struct SIPunchMessage siPunchMessage;
  } Msg;

  void SaveToFile(String desc, File file);
};

class MessageDatabase
{
  public:
    static void Init(enum EventType Type);
    static bool EraseDB();
    static bool DoesDBExist();
    static void SaveSIPunchMessage(SIPunchMessage *siMessage);
    static int16_t GetSIPunchMessage(SIPunchMessage *siMessage);
    static void SetSentByMessageNumber(uint8_t messageNumber);
    static void SetSentAsMessageNumber(uint16_t recNo, uint8_t messageNumber);
    static bool Delete(uint8_t recNo);
    static bool DeleteMessagesByTimeSlotMsg(RadioMessage *radioMessageTimeSlot);
    static bool DeleteMessagesBySimpleAckMsg(RadioMessageSimpleAck *simpleAckMessage);
    static bool DeleteByRadioMessageNumber(uint8_t radioMessageNumber);
    static void SaveRadioMessage(RadioMessage *radioMessage);
    static int16_t GetRadioPunchMessage(RadioMessage *radioMessage);
    static int16_t GetCombinedRadioMessage(RadioMessage *radioMessage, uint8_t noOfPunchesFittingIntoTimeSlot);
    static int16_t GetMessageNumberToAck();
    static uint8_t GetTimeSlotData(RadioMessageDataTimeSlotRecord *timeSlotRecords);
    static void ResetSentFlag();
    static uint32_t LastSICardNumber;
    static uint8_t LastMessageNumber;
    static uint8_t LastFromNode;
    static uint8_t ReceiveCount;
  private:
    static File dbFile;
    static File logFile;
    static EDB db;
    static void GetLogFileName(uint8_t fileNo, char * fileNameCharArray);
    static void Writer(unsigned long address, uint8_t data);
    static uint8_t Reader(unsigned long address);
    static int16_t GetDBMessage(DatabaseMessage *dbMsg, uint8_t numberFromEnd);
};

#endif // __MessageDatabase_H__
