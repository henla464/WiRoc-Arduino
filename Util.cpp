#include "Util.h"
#include "Settings.h"

uint8_t Util::MessageNumber;

void Util::InitRadioMessage(RadioMessage *message, uint8_t messageType) {
  message->stx = STX;
  message->messageVersion = RADIO_MESSAGE_VERSION;
  message->dataLength = 0;
  message->messageType = messageType;
  message->fromNode = Settings::GetNodeNumber();
  message->messageNumber = 0;
  message->checkSum = 0x00;
}

void Util::InitRadioMessageSimpleAck(RadioMessageSimpleAck *message) {
  message->stx = STX;
  message->messageVersion = RADIO_MESSAGE_SIMPLE_ACK_VERSION;
  message->dataLength = 0;
  message->messageNumberToAck = 0;
  message->checkSum = 0x00;
}

//***************************************************************************
//** Author: Jürgen Ehms
//** File name: crc529.c
//** Description: Program to generate 16 BIT CRC
//** Return values: 16 BIT CRC
//** Error messages: none
//** Version last change description
//** 1.00 07.09.2004
//***************************************************************************
#define POLYNOM 0x8005
unsigned int Util::crc(unsigned int uiCount, unsigned char *pucDat)
{
  short int iTmp;
  unsigned short int uiTmp,uiTmp1,uiVal;
  unsigned char *pucTmpDat;
  if (uiCount < 2) return(0);
  // response value is "0" for none or one data byte
  pucTmpDat = pucDat;
  uiTmp1 = *pucTmpDat++;
  uiTmp1 = (uiTmp1<<8) + *pucTmpDat++;
  if (uiCount == 2) return(uiTmp1); // response value is CRC for two data bytes
  for (iTmp=(int)(uiCount>>1);iTmp>0;iTmp--)
  {
    if (iTmp>1)
    {
      uiVal = *pucTmpDat++;
      uiVal= (uiVal<<8) + *pucTmpDat++;
    }
    else
    {
      if (uiCount&1)
        // odd number of data bytes, complete with "0"
      {
        uiVal = *pucTmpDat;
        uiVal= (uiVal<<8);
      }
      else
      {
        uiVal=0; //letzte Werte mit 0
      }
    }
    for (uiTmp=0;uiTmp<16;uiTmp++)
    {
      if (uiTmp1 & 0x8000)
      {
        uiTmp1 <<= 1;
        if (uiVal & 0x8000)uiTmp1++;
          uiTmp1 ^= POLYNOM;
      }
      else
      {
        uiTmp1 <<= 1;
        if (uiVal & 0x8000)uiTmp1++;
      }
      uiVal <<= 1;
    }
  }
  return(uiTmp1);
}

uint8_t Util::GetNextMessageNumber()
{
  MessageNumber++;
  if (MessageNumber == 0)
  {
    MessageNumber++;
  }
  //Serial.print("GetNextMessageNumber() "); Serial.println(MessageNumber);
  return MessageNumber;
}
  
uint8_t Util::GetCurrentMessageNumber()
{
  if (MessageNumber == 0)
  {
    MessageNumber++;
  }
  //Serial.print("GetCurrentMessageNumber() "); Serial.println(MessageNumber);
  return MessageNumber;
}

