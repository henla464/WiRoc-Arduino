#include <Usb.h>
#include <CP210x.h>
#include <SPI.h>
#include <SD.h>
#include <EEPROM.h>
#include <EDB.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include "TimerOne.h"
#include "TimerThree.h"
#include "Settings.h"
#include "SIStation.h"
#include "Radio.h"
#include "TimeSlotManager.h"
#include "MessageDatabase.h"
#include "Battery.h"
#include "LCDMachine.h"
#include "P2PRetryManager.h"
#include "ModuleDefs.h"


SIStation station;
Radio outboundRadio(&Serial2); // 17, 16

#ifdef IS_RELAY_MODULE
Radio inboundRadio(&Serial1); // 19,18
#endif

uint8_t inboundRadioMode;
uint8_t outboundRadioMode;

void setup()
{
  
  Serial.begin( 115200 );
  
  delay( 40 );
  if (Usb.Init() == -1)
      Serial.println("OSC did not start.");

  Settings::Init();
  outboundRadio.Init(Settings::GetOutboundChannel());
  #ifdef IS_RELAY_MODULE
  inboundRadio.Init(Settings::GetInboundChannel());
  #endif
  LCDMachine::Init(&station, &outboundRadio, BUTTON_ADC_PIN, LCD_BACKLIGHT_PIN);
  LCDMachine::Tick();

  delay( 400 );
  #ifdef IS_STATION_MODULE
  MessageDatabase::Init(SIPunchMessageType);
  #endif

  outboundRadioMode = Settings::GetOutboundRadioMode();
  #ifdef IS_RELAY_MODULE
  MessageDatabase::Init(RadioMessageType);

  inboundRadioMode = Settings::GetInboundRadioMode();
  if (inboundRadioMode == P2M_RETRY) //P2M retry
  {
    Timer1.initialize(8388000);         // initialize timer1
    Timer1.attachInterrupt(SendTimeSlotMessage);  
  }
  #endif
  
  Serial.println("after message db init"); 
  delay( 200 );

  Timer3.initialize(1000000); // initialize timer1
  Timer3.attachInterrupt(Battery::UpdateBattery);  


  attachInterrupt(digitalPinToInterrupt(OUTBOUND_AUX_PIN), TimeSlotManager::RadioMessageAvailable, FALLING);
  delay(10);
}

#ifdef IS_RELAY_MODULE
RadioMessage radioMessage;

void SendTimeSlotMessage() {
  //Serial.println("callback interrupt timer method");
  // read messages from SD card
  RadioMessageDataTimeSlotRecord timeSlotRecords[MAX_NO_OF_TIMESLOTS_PER_ROUND];
  uint8_t cnt = MessageDatabase::GetTimeSlotData(timeSlotRecords);
  inboundRadio.SendTimeSlotMessage((uint8_t*)&timeSlotRecords, cnt*sizeof(RadioMessageDataTimeSlotRecord));
}
#endif


void radioMessageAvailable()
{
  TimeSlotManager::RadioMessageAvailable();
}

bool alreadyInitialized = false;
bool siStationDetached = false;

void loop()
{
    LCDMachine::Tick();
    GetAckAndDeleteAckedMessages();
    #ifdef IS_STATION_MODULE
    Usb.Task();
    uint8_t state = Usb.getUsbTaskState();
    switch (state)
    {
      case USB_STATE_RUNNING:
      {
        InitSIStation();
        //Serial.println("after init");
        GetSIPunchDataToDB();
        //delay(30);
        break;
      }
      case USB_STATE_ERROR:
      {
         Serial.println("usb state error");
         Usb.setUsbTaskState(USB_DETACHED_SUBSTATE_INITIALIZE);
         
         if (Usb.Init() == -1)
           Serial.println("OSC did not start.");
         break;
      }
      case USB_DETACHED_SUBSTATE_WAIT_FOR_DEVICE:
      {
        siStationDetached = true;
        break;
      }
      case USB_ATTACHED_SUBSTATE_SETTLE:
      {
        if (siStationDetached) {
          alreadyInitialized = false;
          if (Usb.Init() == -1) {
            Serial.println("OSC did not start.");
          } else {
            siStationDetached = false;
            Serial.println("Usb init");
            delay(400);
          }
        }
        break;
      }
      default:
      {
        Serial.print("State: ");
        Serial.println(state);
      }
    }
    #endif
    
    #ifdef IS_RELAY_MODULE
    GetInboundRadioMessageToDB();
    
    if (inboundRadioMode == P2P_RETRY) //P2P retry
    {
       // send simple ack
       int16_t messageNumberToAck = MessageDatabase::GetMessageNumberToAck();
       if (messageNumberToAck >= 0)
       {
         inboundRadio.SendSimpleAckMessage(messageNumberToAck);
       }
    }
    #endif
    
    //Serial.println("SendOutboundRadioMessages");
    SendOutboundRadioMessages();
}

void PrintTimeSlotMessage(RadioMessage *timeSlotRadioMessage)
{
  Serial.print("TimeSlot: "); 
  uint8_t noOfTimeSlotRecords = timeSlotRadioMessage->dataLength / sizeof(RadioMessageDataTimeSlotRecord);
  RadioMessageDataTimeSlotRecord* timeSlot = (RadioMessageDataTimeSlotRecord*)&(timeSlotRadioMessage->data);
  for (uint8_t i = 0; i < noOfTimeSlotRecords; i++)
  {
    Serial.print("Node number: "); Serial.print(timeSlot->nodeNumber);  
    Serial.print(" Message number: "); Serial.print(timeSlot->messageNumber); 
    Serial.print(" Timeslot length: "); Serial.println(timeSlot->timeSlotLength); 
    timeSlot++;
  }
}

void PrintRadioMessage(RadioMessage *radioMessage)
{
  uint8_t noOfDataRecords = 0;
  if (radioMessage->messageType == PUNCH)
  {
    noOfDataRecords = radioMessage->dataLength / sizeof(RadioMessageDataRecord);
    RadioMessageDataRecord* record = (RadioMessageDataRecord*)&(radioMessage->data);
    Serial.print("PUNCH: messageNumber: "); Serial.print(radioMessage->messageNumber);
    for (uint8_t i = 0; i < noOfDataRecords; i++)
    {
      Serial.print(" siCardNumber: "); Serial.print(record->siCardNumber);  
      record++;
    }
    Serial.println("");
  } else if (radioMessage->messageType == COMBINED_PUNCH)
  {
    noOfDataRecords = radioMessage->dataLength / sizeof(RadioMessageDataCombinedRecord);
    RadioMessageDataCombinedRecord* record = (RadioMessageDataCombinedRecord*)&(radioMessage->data);
    Serial.print("COMBINED_PUNCH: messageNumber: "); Serial.print(radioMessage->messageNumber);
    for (uint8_t i = 0; i < noOfDataRecords; i++)
    {
      Serial.print(" siCardNumber: "); Serial.print(record->siCardNumber);  
      record++;
    }
    Serial.println("");
  }
}

void GetAckAndDeleteAckedMessages()
{
    if (outboundRadioMode == P2M_RETRY) //P2M retry
    {
      // get timeslot and ack message
      RadioMessage receivedTimeSlotMessage;
      uint8_t rCnt = outboundRadio.GetRadioData(&receivedTimeSlotMessage);
      if (rCnt) 
      {
        //Serial.print("Received from radio: 0x"); Serial.print(rCnt, HEX); Serial.print(" bytes, Data: ");
        //for (int i= 0; i < rCnt; i++) {
        //   Serial.print(((uint8_t *)&receivedTimeSlotMessage)[i], HEX); Serial.print(" ");
        //}
        //Serial.println(""); 
        if (receivedTimeSlotMessage.messageType == ACKS_AND_TIMESLOT)
        {
          TimeSlotManager::TimeSlotMessageReceived();
          TimeSlotManager::UpdateTimeSlot(&receivedTimeSlotMessage);
          //PrintTimeSlotMessage(&receivedTimeSlotMessage);
          MessageDatabase::DeleteMessagesByTimeSlotMsg(&receivedTimeSlotMessage);
        }
      }
    }
    else if (outboundRadioMode == P2P_RETRY) //P2P retry
    {
      // get simple ack
      RadioMessageSimpleAck simpleAckMessage;
      if (outboundRadio.GetSimpleAckMessage(&simpleAckMessage) > 0)
      {
        P2PRetryManager::AckMessageReceived();
        MessageDatabase::DeleteMessagesBySimpleAckMsg(&simpleAckMessage);
      }
    }
}

void GetSIPunchDataToDB()
{
  SIPunchMessage siMessage;
  uint8_t cnt = station.GetSIPunchData(&siMessage);
  if (cnt) {
          //handle data
          
          Serial.print("Received: 0x"); Serial.print(cnt, HEX); Serial.print(" bytes, Data: ");
          for (int i= 0; i < cnt; i++) {
            Serial.print(((uint8_t *)&siMessage)[i], HEX); Serial.print(" ");
          }
          Serial.println("");  
          
    MessageDatabase::SaveSIPunchMessage(&siMessage);
  }
}

#ifdef IS_RELAY_MODULE
void GetInboundRadioMessageToDB()
{
  RadioMessage radioMessage;
  uint8_t cnt = inboundRadio.GetRadioData(&radioMessage);
  if (cnt) {
    Serial.print("Received from radio: 0x"); Serial.print(cnt, HEX); Serial.print(" bytes, Data: ");
    for (int i= 0; i < cnt; i++) {
      Serial.print(((uint8_t *)&radioMessage)[i], HEX); Serial.print(" ");
    }
    Serial.println(""); 
    if (radioMessage.messageType == PUNCH || radioMessage.messageType == COMBINED_PUNCH)
    {
      MessageDatabase::SaveRadioMessage(&radioMessage);
    }
  }
}
#endif


RadioMessage outboundRadioMessageToSend;
bool getNewOutboundRadioMessage = true;
void SendOutboundRadioMessagesP2MRetry()
{
  uint8_t noOfPunchesFittingIntoTimeSlot = TimeSlotManager::GetNoOfPunchesFittingIntoTimeSlot(&outboundRadio);
  //Serial.print("noOfPunchesFittingIntoTimeSlot: "); Serial.println(noOfPunchesFittingIntoTimeSlot);      
  if (noOfPunchesFittingIntoTimeSlot > 0) 
  {
    //Serial.println("> 0 punch fits into timeslot");
    int16_t messageNo = -1;
    if (getNewOutboundRadioMessage)
    {
      messageNo = MessageDatabase::GetCombinedRadioMessage(&outboundRadioMessageToSend, noOfPunchesFittingIntoTimeSlot);
      //Serial.print("getNewOutboundRadioMessage, messageNo: "); Serial.println(messageNo);
      if (messageNo >= 0)
      {
        getNewOutboundRadioMessage = false;
        Serial.println("Fetched message from database");
      }
    } else {
      messageNo = outboundRadioMessageToSend.messageNumber;
    }
    //Serial.print("Message no: "); Serial.println(messageNo);
    if (messageNo >= 0)
    {
      if (!IsTransmittingOrReceiving())
      {
        Serial.println("Not transmitting or receiving");
        if (TimeSlotManager::IsInStartOfTimeSlot(outboundRadioMessageToSend.dataLength + RADIO_HEADER_LENGTH, &outboundRadio))
        {
          Serial.println("In timeslot, send message");
          PrintRadioMessage(&outboundRadioMessageToSend);
          outboundRadio.SendPunchData(&outboundRadioMessageToSend);
          MessageDatabase::SetSentByMessageNumber(messageNo);
          getNewOutboundRadioMessage = true;
        } else {
          Serial.print("Not in timeslot, time: "); Serial.println(micros());
        }
      }
    } 
  }
}

void SendOutboundRadioMessagesP2PRetry()
{
    int16_t messageNo = -1;
    if (getNewOutboundRadioMessage)
    {
      messageNo = MessageDatabase::GetCombinedRadioMessage(&outboundRadioMessageToSend, 3);
      //Serial.print("getNewOutboundRadioMessage, messageNo: "); Serial.println(messageNo);
      if (messageNo >= 0)
      {
        getNewOutboundRadioMessage = false;
        Serial.println("Fetched message from database");
      }
    } else {
      messageNo = outboundRadioMessageToSend.messageNumber;
    }
    //Serial.print("message no "); Serial.println(messageNo);
    if (messageNo >= 0
        && !IsTransmittingOrReceiving()
        && P2PRetryManager::IsAckReceivedOrTimeout())
    {
       Serial.println("In P2P timeslot, send message");
       PrintRadioMessage(&outboundRadioMessageToSend);
       MessageDatabase::ResetSentFlag();
       outboundRadio.SendPunchData(&outboundRadioMessageToSend);
       P2PRetryManager::MessageSent();
       MessageDatabase::SetSentByMessageNumber(messageNo);
       getNewOutboundRadioMessage = true;
    } 
}

void SendOutboundRadioMessagesP2PFireForget()
{
    int16_t messageNo = -1;
    if (getNewOutboundRadioMessage)
    {
      messageNo = MessageDatabase::GetCombinedRadioMessage(&outboundRadioMessageToSend, 3);
      //Serial.print("getNewOutboundRadioMessage, messageNo: "); Serial.println(messageNo);
      if (messageNo >= 0)
      {
        getNewOutboundRadioMessage = false;
        Serial.println("Fetched message from database");
      }
    } else {
      messageNo = outboundRadioMessageToSend.messageNumber;
    }
    //Serial.print("message no "); Serial.println(messageNo);
    if (messageNo >= 0 && !IsTransmittingOrReceiving())
    {
       Serial.println("In P2P fire forget, send message");
       PrintRadioMessage(&outboundRadioMessageToSend);
       outboundRadio.SendPunchData(&outboundRadioMessageToSend);
       MessageDatabase::DeleteByRadioMessageNumber(messageNo);
       getNewOutboundRadioMessage = true;
    } 
}

void SendOutboundRadioMessages()
{
   if (outboundRadioMode == P2P_RETRY) //P2P retry
    {
      SendOutboundRadioMessagesP2PRetry();
    } else if (outboundRadioMode == P2P_FIRE_FORGET) //P2P fire forget
    {
      SendOutboundRadioMessagesP2PFireForget();
    } else if (outboundRadioMode == P2M_RETRY) //P2M retry 
    {
      SendOutboundRadioMessagesP2MRetry();
    }
}

bool IsTransmittingOrReceiving()
{
  return (digitalRead(OUTBOUND_AUX_PIN) == LOW);
}


void InitSIStation()
{
  if (!alreadyInitialized) {
     Serial.println("init station"); 
     station.Init();
     alreadyInitialized = true;
  }
}

