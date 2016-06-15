#include "SIStation.h"
#include "Util.h"

uint8_t CP210xAsync::OnInit(CP210x *cp210x)
{
    uint8_t rcode = 0;
    rcode = cp210x->IFCEnable();

    if (rcode)
    {
        return rcode;
    }
    
    rcode = cp210x->SetBaudRate(38400); //38400);

    if (rcode)
    {
        return rcode;
    }
   
    rcode = cp210x->SetDataBits(8);
    if (rcode)
    {
        return rcode;
    }
    
    rcode = cp210x->SetStopBits(CP210X_STOP_BITS_1);
    if (rcode)
    {
      return rcode;
    }
    
    rcode = cp210x->SetParity(CP210X_PARITY_NONE);
    if (rcode)
    {
          return rcode;
    }
    
    rcode = cp210x->SetFlowControl(CP210X_FLOW_CONTROL_OFF);
    if (rcode)
    {
          return rcode;
    }


    return rcode;
}


USB              Usb;
CP210xAsync      Cp210xAsync;
CP210x           Cp210x(&Usb, &Cp210xAsync);
SIPunchMessage   receivedSIPunchMessage;

bool SIStation::IsCorrectMSModeDirectResponse(uint8_t cnt, uint8_t *data)
{
  return cnt == 9 && data[0] == STX && data[5] == 0x4D;
}

void SIStation::Init() {
    delay(100);
    
    // set master-mode to direct
    uint8_t setMSDMode[8] = {0xFF, 0x02, 0xF0, 0x01, 0x4D, 0x6D, 0x0A, 0x03};
    Cp210x.SndData(8, setMSDMode);

    delay(100);
    
    uint16_t rcvd = 20;
    uint8_t  buf[20];
    
    for (uint8_t i=0; i<20; i++)
      buf[i] = 0;
    
    rcvd = 20;
    Cp210x.RcvData(&rcvd, buf);


      Serial.print("Received: 0x"); Serial.print(rcvd, HEX); Serial.print(" bytes, Data: ");
                for (uint8_t i = 0; i <= rcvd; i++) {
                    Serial.print(buf[i], HEX); Serial.print(" ");
                }
                Serial.println(""); 

    uint8_t i = 0;                
    while (buf[i] != STX && i < 20) {
          i++;
    }
    
    if (IsCorrectMSModeDirectResponse(rcvd-i, buf+i)) {
      Serial.println("38400 kbit/s works");
      return;
    }
      
      
    // something wrong, try other baudrate
    Cp210x.SetBaudRate(4800);
    delay(10);
    
    Cp210x.SndData(8, setMSDMode);
    delay(100);
    for (uint8_t i=0; i<9; i++)
      buf[i] = 0;
    
    rcvd = 9;
    Cp210x.RcvData(&rcvd, buf);
    if (IsCorrectMSModeDirectResponse(rcvd, buf)) {
      Serial.println("4800 kbit/s works");
      return;
    }
    
    Serial.print("Could not communicate with master station");
}


void SIStation::GetDataToReceivedSIMessage(uint8_t expectedTotalLength) {
  // read until STX
  uint16_t noOfByteRead = expectedTotalLength - totalReceived;
  uint8_t buf[expectedTotalLength];
  uint8_t rcode;

  rcode = Cp210x.RcvData(&noOfByteRead, buf);
  if (rcode)
  {
    if (rcode == hrNAK) {
        //Serial.print("rcode: hrNAK");
        delay(10);
        return;
    } else if (rcode == hrBUSY) {
        Serial.println("rcode: hrBUSY");
        delay(10);
        return;
    } else
    {
        Serial.print("rcode: "); Serial.println(rcode,HEX);
        delay(10);
        return;
    }
  }

  Serial.print("nob: "); Serial.println(noOfByteRead, HEX);

   // find STX
  uint8_t i = 0;
  uint8_t startCopyFromIndex = 0;
  if (totalReceived == 0)
  {
    while (buf[i] != STX && i < noOfByteRead) {
      i++;
      Serial.print("skip ");
    }
    startCopyFromIndex = i;
  }
  // copy from stx
  memcpy((((uint8_t *)&receivedSIPunchMessage)+totalReceived), buf+startCopyFromIndex, noOfByteRead-startCopyFromIndex);
  totalReceived += noOfByteRead-startCopyFromIndex;
}


uint8_t SIStation::GetSIPunchData(SIPunchMessage *siPunchMessage) {
  uint8_t totalLength = 19;
  GetDataToReceivedSIMessage(totalLength);
  if (totalReceived == 19) {
    *siPunchMessage = receivedSIPunchMessage;
    // fix byteorder in si card number
    uint32_t byte0 = siPunchMessage->dataRecord.siCardNumber & 0xFF;
    uint32_t byte1 = (siPunchMessage->dataRecord.siCardNumber >> 8) & 0xFF;
    uint32_t byte2 = (siPunchMessage->dataRecord.siCardNumber >> 16) & 0xFF;
    uint32_t byte3 = (siPunchMessage->dataRecord.siCardNumber >> 24) & 0xFF;
    uint8_t series = byte1;
    if (series >= 1 && series <= 4) {
      // Special handling, add series times 100 000
      siPunchMessage->dataRecord.siCardNumber = (series * 100000) + ((byte2 << 8) | byte3);
    } else {
      siPunchMessage->dataRecord.siCardNumber = (byte0 << 24) | (byte1 << 16) | (byte2 << 8) | byte3;
    }
    
    Serial.print("SiCardNumber fixed:"); Serial.println(siPunchMessage->dataRecord.siCardNumber);
    // fix byteorder in 24h timer
    uint16_t twelveHourTimerByte0 = siPunchMessage->dataRecord.twelveHourTimer & 0xFF;
    uint16_t twelveHourTimerByte1 = (siPunchMessage->dataRecord.twelveHourTimer >> 8) & 0xFF;
    siPunchMessage->dataRecord.twelveHourTimer = (twelveHourTimerByte0 << 8) | twelveHourTimerByte1;
    uint8_t retVal = totalReceived;
    totalReceived = 0;
    return retVal;
  } else {
    return 0;
  }
}

void SIStation::GetTestSIPunchMessage(SIPunchMessage *siPunchMessage, uint16_t stationCode, uint32_t cardNumber)
{
      siPunchMessage->stx = STX;
      siPunchMessage->commandCode = COMMAND_CODE_TRANSMIT_RECORD;
      siPunchMessage->len = 0x0D; // 13 bytes before CRC
      siPunchMessage->stationCode = stationCode;
      siPunchMessage->dataRecord.siCardNumber = cardNumber;
      siPunchMessage->dataRecord.twentyFourHour = 1;
      siPunchMessage->dataRecord.weekDay = 1;
      siPunchMessage->dataRecord.fourWeekCounter = 0;
      siPunchMessage->dataRecord.fill = 0;
      siPunchMessage->dataRecord.twelveHourTimer = 36000;
      siPunchMessage->dataRecord.subSecond = 0;
      siPunchMessage->mem2 = 0;
      siPunchMessage->mem1 = 0;
      siPunchMessage->mem0 = 0;
      siPunchMessage->crc = 0;
      siPunchMessage->etx = ETX;
}

