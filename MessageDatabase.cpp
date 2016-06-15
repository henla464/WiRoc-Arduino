#include "MessageDatabase.h"
#include "ModuleDefs.h"
#include "Settings.h"

void DatabaseMessage::SaveToFile(String desc, File file) 
{
  file.print(desc);
  file.print(" id:");
  file.print(id, HEX);
  file.print(" ");
  if (sent) file.print("SENT"); else file.print("not SENT");
  file.print(" ");
  if (ackSent) file.print("ackSent"); else file.print("not ackSent");
  file.print(" sentas:");
  file.print(sentAsMessageNumber, HEX);
  if (Type == RadioMessageType) 
  {
    file.print(" RD ");
    file.print(Msg.radioMessage.stx, HEX);
    file.print(" ");
    file.print(Msg.radioMessage.dataLength, HEX);
    file.print(" ");
    file.print(Msg.radioMessage.messageVersion, HEX);
    file.print(" ");
    file.print(Msg.radioMessage.fromNode, HEX);
    file.print(" ");
    file.print(Msg.radioMessage.messageType, HEX);
    file.print(" ");
    file.print(Msg.radioMessage.messageNumber, HEX);
    file.print(" ");
    file.print(Msg.radioMessage.checkSum, HEX);
    file.print(" ");
    uint8_t len = Msg.radioMessage.dataLength;
    for (uint8_t i = 0; i < len; i++)
    {
      file.print(Msg.radioMessage.data[i], HEX);
      file.print(" ");
    }
    file.println("");
  } else {
    file.print(" SI stx:");
    file.print(Msg.siPunchMessage.stx, HEX);
    file.print(" cmd:");
    file.print(Msg.siPunchMessage.commandCode, HEX);
    file.print(" len:");
    file.print(Msg.siPunchMessage.len, HEX);
    file.print(" stncode:");
    file.print(Msg.siPunchMessage.stationCode, HEX);
    file.print(" crdno:");
    file.print(Msg.siPunchMessage.dataRecord.siCardNumber, HEX);
    file.print(" 24h:");
    file.print(Msg.siPunchMessage.dataRecord.twentyFourHour, HEX);
    file.print(" wd:");
    file.print(Msg.siPunchMessage.dataRecord.weekDay, HEX);
    file.print(" 4wc:");
    file.print(Msg.siPunchMessage.dataRecord.fourWeekCounter, HEX);
    file.print(" fill:");
    file.print(Msg.siPunchMessage.dataRecord.fill, HEX);
    file.print(" 12hc:");
    file.print(Msg.siPunchMessage.dataRecord.twelveHourTimer, HEX);
    file.print(" subsec:");
    file.print(Msg.siPunchMessage.dataRecord.subSecond, HEX);
    file.print(" mem2:");
    file.print(Msg.siPunchMessage.mem2, HEX);
    file.print(" mem1:");
    file.print(Msg.siPunchMessage.mem1, HEX);
    file.print(" mem0:");
    file.print(Msg.siPunchMessage.mem0, HEX);
    file.print(" crc:");
    file.print(Msg.siPunchMessage.crc, HEX);
    file.print(" etx:");
    file.println(Msg.siPunchMessage.etx, HEX);
  }
  file.flush();
  
}


EDB MessageDatabase::db(&MessageDatabase::Writer, &MessageDatabase::Reader);
File MessageDatabase::dbFile;
File MessageDatabase::logFile;
uint32_t MessageDatabase::LastSICardNumber = 0;
uint8_t MessageDatabase::LastMessageNumber = 0;
uint8_t MessageDatabase::LastFromNode = 0;
uint8_t MessageDatabase::ReceiveCount = 0;

void MessageDatabase::Init(enum EventType Type)
{
  pinMode(4, OUTPUT); // chip select for SD, rerouted to pin 10
  pinMode(53, OUTPUT); // hardware SS pin for SD, not used but must be left as output

  if (!SD.begin(4)) {
    Serial.println("SD initialization failed");
    return;
  }
  Serial.println("SD initialization done.");
  uint32_t totalSize = 0;
  dbFile = SD.open("database.db", FILE_WRITE);
  
  totalSize += dbFile.size();
  if (totalSize == 0) 
  {
    db.create(0, TABLE_SIZE, sizeof(DatabaseMessage));
  } 
  else
  {
    db.open(0);
  }

  uint8_t logFileNumberToUse = 255;
  char logFileNameToUse[10];
  uint8_t logFileNumber = 0;
  for(; logFileNumber < 20; logFileNumber++)
  {
    char fileName[10]; 
    MessageDatabase::GetLogFileName(logFileNumber, fileName);
    //Serial.println(fileName);
    if (SD.exists(fileName))
    {
      Serial.print(fileName); Serial.println(" exists");
      logFile = SD.open(fileName, FILE_WRITE);
      totalSize += logFile.size();
      logFile.close();
   } else {
      //Serial.println("not exists");
      if (logFileNumberToUse == 255)
      {
        strncpy(logFileNameToUse, fileName, 10);
        logFileNumberToUse = logFileNumber;
      }
    }
  }

  // remove the next log file number so that there will always be
  // at least one available
  char fileName[10]; 
  MessageDatabase::GetLogFileName((logFileNumberToUse+1) % 20, fileName);
  if (SD.exists(fileName))
  {
    logFile = SD.open(fileName, FILE_WRITE);
    totalSize -= logFile.size();
    logFile.close();
    SD.remove(fileName);
    Serial.print("Removed "); Serial.println(fileName);
  } 

  logFileNumber = (logFileNumber++ % 20);
  // Delete log files if more than 64 MB used already, reduce until there is at least 64 MB
  for(; logFileNumber != logFileNumberToUse && totalSize >= 64000000
      ; logFileNumber = (logFileNumber++ % 20))
  {
      char fileName[10]; 
      MessageDatabase::GetLogFileName(logFileNumber, fileName);
      if (SD.exists(fileName))
      {
        logFile = SD.open(fileName, FILE_WRITE);
        totalSize -= logFile.size();
        logFile.close();
        SD.remove(fileName);
        Serial.print("Removed "); Serial.println(fileName);
      }
  }

  Serial.print("Opened ");
  Serial.println(logFileNameToUse);
  logFile = SD.open(logFileNameToUse, FILE_WRITE);
}

void MessageDatabase::GetLogFileName(uint8_t fileNo, char * fileNameCharArray)
{
   String fileName = "log";
   if (fileNo <= 9) {
      fileName += "0";
   } 
   fileName += fileNo;
   fileName += ".txt";
   fileName.toCharArray(fileNameCharArray, 10, 0);
}

bool MessageDatabase::EraseDB()
{
  return SD.remove("database.db");
}

bool MessageDatabase::DoesDBExist()
{
  return SD.exists("database.db");
}


void MessageDatabase::Writer(unsigned long address, uint8_t data)
{
  //delay(1);
  while (!dbFile.seek(address))
  {
    //uint32_t fileSize;
    //fileSize = dbFile.size();
    Serial.print("!seek address: "); Serial.println(address, HEX); 
    //Serial.print(" file size: "); Serial.print(fileSize, HEX);
    delay(1);
  }
  //Serial.print(" write address: "); Serial.print(address); Serial.print(" data "); Serial.print(data); Serial.print(" size "); Serial.print(dbFile.size());
  while (!dbFile.write(data))
  {
    Serial.println("MessageDatabase::Writer write error");
    dbFile.seek(address);
    delay(1);
  }
  dbFile.flush();
  //Serial.print(" size after "); Serial.print(dbFile.size()); 
}

uint8_t MessageDatabase::Reader(unsigned long address)
{
  while (!dbFile.seek(address))
  {
    uint32_t fileSize;
    fileSize = dbFile.size();
    Serial.print("!seek address: "); Serial.print(address, HEX); Serial.print(" file size: "); Serial.print(fileSize, HEX);
    delay(100);
  }
  uint8_t data = dbFile.read(); 
  //Serial.print(" read address: "); Serial.print(address); Serial.print(" data "); Serial.print(data);
  return data;
}

void MessageDatabase::SaveSIPunchMessage(SIPunchMessage *siMessage)
{
  //Serial.println("savesipunchmessage");
  DatabaseMessage dbMsg;
  dbMsg.id = 0xFFFF;
  dbMsg.sent = false;
  dbMsg.ackSent = false;
  dbMsg.sentAsMessageNumber = 0;
  dbMsg.Type = SIPunchMessageType;
  dbMsg.Msg.siPunchMessage = *siMessage;
  /*Serial.print("savesipunchmessage 2: ");
    for (int i= 0; i < sizeof(DatabaseMessage); i++) {
            Serial.print(((uint8_t *)&dbMsg)[i], HEX); Serial.print(" ");
      }
      Serial.println("");  
  */
  LastMessageNumber = 0;
  LastFromNode = Settings::GetNodeNumber();
  LastSICardNumber = siMessage->dataRecord.siCardNumber;
  ReceiveCount++;

  uint8_t edbRetValue = db.appendRec((byte*) &dbMsg);
  //Serial.print("edbRetValue"); Serial.println(edbRetValue);
  dbMsg.SaveToFile("SIPunchMessageAdded", logFile);
}
  
void MessageDatabase::SaveRadioMessage(RadioMessage *radioMessage)
{

  DatabaseMessage dbMsg;
  dbMsg.id = 0xFFFF;
  dbMsg.sent = false;
  dbMsg.ackSent = false;
  dbMsg.sentAsMessageNumber = 0;
  dbMsg.Type = RadioMessageType;
  dbMsg.Msg.radioMessage = *radioMessage;

  //-- save some stats used in receive menu
  LastMessageNumber = radioMessage->messageNumber;
  LastFromNode = radioMessage->fromNode;
  if (radioMessage->messageType == PUNCH)
  {
    LastSICardNumber = ((RadioMessageDataRecord*)&radioMessage->data)[0].siCardNumber;
  } else if (radioMessage->messageType == COMBINED_PUNCH)
  {
    LastSICardNumber = ((RadioMessageDataCombinedRecord*)&radioMessage->data)[0].siCardNumber;
  }
  ReceiveCount++;
  
  db.appendRec((byte*) &dbMsg);
  dbMsg.SaveToFile("RadioMessageAdded", logFile);
}

int16_t MessageDatabase::GetDBMessage(DatabaseMessage *dbMsg, uint8_t numberFromEnd = 0)
{
  uint32_t cnt = db.count();
  //Serial.print("numberFromEnd: "); Serial.println(numberFromEnd);
  //Serial.print("cnt: "); Serial.println(cnt);
  if (cnt-numberFromEnd <= 0)
  {
    return -1;
  }
  if (db.readRec(cnt-numberFromEnd, (byte*) dbMsg) == EDB_OK)
  {
    //Serial.print("getdbmessage: ");
    //for (uint8_t i = 0; i < sizeof(DatabaseMessage); i++) {
    //        Serial.print(((uint8_t *)dbMsg)[i], HEX); Serial.print(" ");
    //  }
    //  Serial.println("");  
    return cnt-numberFromEnd;
  }
  else
  {
//    Serial.print("not edb_ok");
    return -1;
  }
 
}

int16_t MessageDatabase::GetCombinedRadioMessage(RadioMessage *radioMessage, uint8_t noOfPunchesFittingIntoTimeSlot)
{
  DatabaseMessage dbMsg;
  Util::InitRadioMessage(radioMessage, COMBINED_PUNCH);
  bool punchAdded = false;
  bool relayedRadioMessage = false;
  uint8_t punchNumber = 0;
  for (int8_t numberFromEnd = 0; 
      punchNumber < min(noOfPunchesFittingIntoTimeSlot,2) && numberFromEnd < db.count(); 
      numberFromEnd++)
  {
    int16_t recNo = MessageDatabase::GetDBMessage(&dbMsg, numberFromEnd);
    //Serial.print("recNo: "); Serial.print(recNo);// Serial.print(" sent: "); 
    //if (dbMsg.sent) { Serial.print("true "); Serial.println(dbMsg.sent); } else { Serial.print("false "); Serial.println(dbMsg.sent); }
    if (recNo > 0 && !dbMsg.sent)
    {
      RadioMessageDataCombinedRecord dataCombinedRecord;
      
      if (dbMsg.Type == SIPunchMessageType)
      {
        //Serial.print(" sipunchmessagetype ");
        memcpy(&dataCombinedRecord,&dbMsg.Msg.siPunchMessage.dataRecord,sizeof(RadioMessageDataRecord));
        dataCombinedRecord.origFromNode = Settings::GetNodeNumber();
        ((RadioMessageDataCombinedRecord*)radioMessage->data)[punchNumber] = dataCombinedRecord;
        punchNumber++;
        relayedRadioMessage = false;
      } 
      else if (dbMsg.Type == RadioMessageType) 
      {
        relayedRadioMessage = true;
        if (dbMsg.Msg.radioMessage.messageType == PUNCH) 
        {
           //Serial.print(" punch ");
           memcpy(&dataCombinedRecord,&dbMsg.Msg.radioMessage.data,sizeof(RadioMessageDataRecord));
           dataCombinedRecord.origFromNode = dbMsg.Msg.radioMessage.fromNode; //Settings::GetNodeNumber();
           ((RadioMessageDataCombinedRecord*)radioMessage->data)[punchNumber] = dataCombinedRecord;
           punchNumber++;
        } 
        else if (dbMsg.Msg.radioMessage.messageType == COMBINED_PUNCH)
        {
          //Serial.print(" combined punch ");
          uint8_t noOfPunches = dbMsg.Msg.radioMessage.dataLength / sizeof(RadioMessageDataCombinedRecord);
          if (punchNumber + noOfPunches >= min(noOfPunchesFittingIntoTimeSlot,2)) 
          {
            // Cannot fit all the punches in this database message into the radiomessage, so skip
            continue;
          }
          for (uint8_t i = 0; i < noOfPunches; i++) {
            memcpy(&dataCombinedRecord,&dbMsg.Msg.radioMessage.data,sizeof(dataCombinedRecord));
            ((RadioMessageDataCombinedRecord*)radioMessage->data)[punchNumber] = dataCombinedRecord;
            punchNumber++;
          }
        }
      }
      punchAdded = true;
      if (radioMessage->messageNumber == 0)
      {
        // call GetNextMessageNumber() only once per message returned
        radioMessage->messageNumber = Util::GetNextMessageNumber();
      }
      SetSentAsMessageNumber(recNo, Util::GetCurrentMessageNumber());
    }
  }
  
  if (punchAdded)
  {
    if (punchNumber == 1 && !relayedRadioMessage)
    {
      // Only one punch AND not a relayed message so change to PUNCH message type 
      // When relayed we need the origFromNode, so can't change
      radioMessage->messageType = PUNCH;
      radioMessage->dataLength = sizeof(RadioMessageDataRecord);
    } 
    else
    {
      radioMessage->dataLength = sizeof(RadioMessageDataCombinedRecord) * punchNumber;
    }
    return Util::GetCurrentMessageNumber();
  }
  return -1;
}

int16_t MessageDatabase::GetSIPunchMessage(SIPunchMessage *siMessage)
{
  DatabaseMessage dbMsg;
  int16_t recNo = MessageDatabase::GetDBMessage(&dbMsg);
  if (recNo > 0)
  {
    *siMessage = dbMsg.Msg.siPunchMessage;
  }
  return recNo;
}

int16_t MessageDatabase::GetRadioPunchMessage(RadioMessage *radioMessage)
{
  DatabaseMessage dbMsg;
  int16_t recNo = MessageDatabase::GetDBMessage(&dbMsg);
  if (recNo > 0)
  {
    *radioMessage = dbMsg.Msg.radioMessage;
  }
  return recNo;
}

void MessageDatabase::SetSentByMessageNumber(uint8_t messageNumber) 
{
  DatabaseMessage dbMsg;
  for (uint32_t recNo = db.count(); recNo > 0; recNo--)
  {
    if (db.readRec(recNo, (byte*) &dbMsg) == EDB_OK)
    {
      if (dbMsg.sentAsMessageNumber == messageNumber)
      {
        dbMsg.sent = true;
        db.updateRec(recNo, (byte*) &dbMsg);
        dbMsg.SaveToFile("SetSentByMessageNumber", logFile);
        Serial.println("SetSentByMessageNumber");
      } else {
        logFile.print("SetSentByMessageNumber not sentAsMessageNumber, recNo ");
        logFile.print(recNo);
        logFile.print(" messageNumber: ");
        logFile.println(messageNumber);
        logFile.flush();
      }
    }
    else
    {
      logFile.print("SetSentByMessageNumber failed for recNo: ");
      logFile.println(recNo);
      logFile.flush();
    }
  }
}

void MessageDatabase::SetSentAsMessageNumber(uint16_t recNo, uint8_t messageNumber) 
{
  DatabaseMessage dbMsg;
  if (db.readRec(recNo, (byte*) &dbMsg) == EDB_OK)
  {
    dbMsg.sentAsMessageNumber = messageNumber;
    db.updateRec(recNo, (byte*) &dbMsg);
    dbMsg.SaveToFile("SetSentAsMessageNumber", logFile);
    Serial.print("SetSentAsMessageNumber recNo: "); Serial.print(recNo); Serial.print(" messageNumber: "); Serial.println(messageNumber);
  } else {
    logFile.print("SetSentAsMessageNumber failed for recNo: ");
    logFile.println(recNo);
    logFile.flush();
    Serial.print("SetSentAsMessageNumber failed for recNo: "); Serial.println(recNo);
  }
}

bool MessageDatabase::DeleteByRadioMessageNumber(uint8_t messageNumber) 
{
  DatabaseMessage dbMsg;
  Serial.print("DeleteByRadioMessageNumber, messageNumber: ");
  Serial.print(messageNumber);
  bool anyDeleted = false;
  bool allOk = true;
  for (uint32_t recNo = db.count(); recNo > 0; recNo--)
  {
    Serial.print("db.count()"); Serial.print(db.count()); Serial.print("recNo: "); Serial.println(recNo);
    if (db.readRec(recNo, (byte*) &dbMsg) == EDB_OK)
    {
        if (dbMsg.sentAsMessageNumber == messageNumber)
        {
          if (db.deleteRec(recNo) == EDB_OK)
          {
            Serial.println(" Deleted");
            dbMsg.SaveToFile("MessageDeleted", logFile);
          } else {
            allOk = false;
          }
          anyDeleted = true;
        } else if (anyDeleted) {
          break;
        }
    }
  }
  Serial.print("before if !allOk");
  if (!allOk)
  {
    Serial.println("");
    logFile.print("MessageDeletedFailed for messageNo: ");
    logFile.println(messageNumber);
    logFile.flush();
  }
  Serial.print("return");
  return allOk;
}

void MessageDatabase::ResetSentFlag() 
{
  DatabaseMessage dbMsg;
  for (uint32_t recNo = db.count(); recNo > 0; recNo--)
  {
    if (db.readRec(recNo, (byte*) &dbMsg) == EDB_OK)
    {
      if (dbMsg.sent)
      {
        dbMsg.sent = false;
        db.updateRec(recNo, (byte*) &dbMsg);
        dbMsg.SaveToFile("ResetSentFlag", logFile);
      }
    }
  }
}

bool MessageDatabase::DeleteMessagesByTimeSlotMsg(RadioMessage *radioMessageTimeSlot) 
{
  uint8_t noOfTimeSlotRecords = radioMessageTimeSlot->dataLength / sizeof(RadioMessageDataTimeSlotRecord);
  RadioMessageDataTimeSlotRecord* timeSlot = (RadioMessageDataTimeSlotRecord*)&(radioMessageTimeSlot->data);
  bool isAllOK = true;
  uint8_t thisNodeNumber = Settings::GetNodeNumber();
  for (uint8_t i = 0; i < noOfTimeSlotRecords; i++)
  {
    if (timeSlot->nodeNumber == thisNodeNumber && timeSlot->messageNumber > 0)
    {
      if (!MessageDatabase::DeleteByRadioMessageNumber(timeSlot->messageNumber))
      {
        isAllOK = false;
      }
      
    }
    timeSlot++;
  }
  MessageDatabase::ResetSentFlag();
  return isAllOK;
}

bool MessageDatabase::DeleteMessagesBySimpleAckMsg(RadioMessageSimpleAck *simpleAckMessage) 
{
  bool isAllOK = true;
  if (!MessageDatabase::DeleteByRadioMessageNumber(simpleAckMessage->messageNumberToAck))
  {
     isAllOK = false;
  }
  //MessageDatabase::ResetSentFlag();
  return isAllOK;
}

int16_t MessageDatabase::GetMessageNumberToAck()
{
  uint16_t messageNumber = -1;
  DatabaseMessage dbMsg;
  for (uint32_t recNo = db.count(); recNo > 0; recNo--)
  {
    if (db.readRec(recNo, (byte*) &dbMsg) == EDB_OK)
    {
      if (!dbMsg.ackSent)
      {
        messageNumber = dbMsg.Msg.radioMessage.messageNumber;
      }
    }
  }
  return messageNumber;
}

uint8_t MessageDatabase::GetTimeSlotData(RadioMessageDataTimeSlotRecord *timeSlotRecords)
{
  uint8_t nodes[5];
  uint8_t numberOfNodes = Settings::GetInboundRadioNodes(nodes);
  bool messageNumberAddedToSlotAlready[numberOfNodes];
  for(uint8_t i = 0; i < numberOfNodes; i++)
  {
    messageNumberAddedToSlotAlready[i] = false;
    RadioMessageDataTimeSlotRecord timeSlot;
    timeSlot.timeSlotLength = 1;
    timeSlot.nodeNumber = nodes[i];
    timeSlot.messageNumber = 0;
    memcpy(timeSlotRecords+i, ((uint8_t*)&timeSlot), sizeof(timeSlot) );  
  }
   
  DatabaseMessage dbMsg;
  uint8_t nextAvailTimeSlotIndex = numberOfNodes;
  for (uint32_t recNo = db.count(); 
      recNo > 0 && nextAvailTimeSlotIndex < MAX_NO_OF_TIMESLOTS_PER_ROUND; 
      recNo--)
  {
    if (db.readRec(recNo, (byte*) &dbMsg) == EDB_OK)
    {
      if (!dbMsg.ackSent)
      {
        bool messageNumberSet = false;
        for(uint8_t i = 0; i < numberOfNodes; i++)
        {
          uint8_t nodeNumber = (timeSlotRecords+i)->nodeNumber;
          if (nodeNumber == dbMsg.Msg.radioMessage.fromNode 
              && !messageNumberAddedToSlotAlready[i])
          {
            (timeSlotRecords+i)->messageNumber = dbMsg.Msg.radioMessage.messageNumber;
            messageNumberAddedToSlotAlready[i] = true;
            messageNumberSet = true;
          }
        }

        if (!messageNumberSet)
        {
          RadioMessageDataTimeSlotRecord timeSlot;
          timeSlot.timeSlotLength = 0;
          timeSlot.nodeNumber = dbMsg.Msg.radioMessage.fromNode;
          timeSlot.messageNumber = dbMsg.Msg.radioMessage.messageNumber;
          memcpy(timeSlotRecords+nextAvailTimeSlotIndex, ((uint8_t*)&timeSlot), sizeof(timeSlot) );
          nextAvailTimeSlotIndex++;
        }
      }
    }
  }
  return (nextAvailTimeSlotIndex-1);
}

