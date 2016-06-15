#include "Radio.h"
#include "Util.h"

Radio::Radio(HardwareSerial* serial): 
  radioSerial(serial)
{
  
}

char Radio::Channels[][3] = {{"A1"},{"B1"},{"C1"},{"D1"},{"E2"},{"F2"},{"G2"},{"H2"},{"I3"},{"J3"},{"K3"},{"L3"},{"A4"},{"B4"},{"C4"},{"D4"},{"E5"},{"F5"},{"G5"},{"H5"},{"I6"},{"J6"},{"K6"},{"L6"}};
float Radio::_slopeCoefficient[6] = {77332.743718593, 52590.391959799, 26331.9798994975, 7132.2211055276, 2301.4874371859, 964.7236180905};
uint8_t Radio::_m[6] = {22, 16, 16, 15, 15, 26};

uint8_t Radio::CalculateCS(uint8_t *data, uint8_t len) {
  uint16_t sum = 0;
  for(int i = 0; i < len; i++) {
    sum += data[i];
  }
  return sum % 256;
}

void Radio::Init(const char channel[3]) {
    Serial.print("Channel ");
    Serial.println(channel);
    _channel[0] = channel[0];
    _channel[1] = channel[1];
    _channel[2] = channel[2];
    totalReceived = 0;
   
    uint8_t readSettingArray[] = { 0xAF, 0xAF, // sync word
                              0x00, 0x00, // id code
                              0xAF,       // header
                              0x80,       // command (sending)
                              0x02,       // command type (read)
                              0x0C,       // length (data section, from here to CS)
                              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                              0x9B,       // CS (calculate and set)
                              0x0D, 0x0A  // end code
                                };

    uint8_t data[23];
    
      //data[0] = 0;
      //data[1];
      uint32_t baudrate = 9600;
      Serial.print("Baud rate: ");
      Serial.println(baudrate);
       delay(1000);
      
      radioSerial->begin(baudrate);
      delay(1000);
      radioSerial->write(readSettingArray,23);
      delay(1000);
      Serial.print("Radio read response: ");
      while(radioSerial->available())
      {  
        uint8_t b = radioSerial->read();
        Serial.print("READ ");
        if (b == 0xAF)
        {
          data[0] = b;
          int i = 1;
          Serial.print(" AF");
      while(radioSerial->available()) {
        if (i < 23){
          data[i++] = radioSerial->read();
        } else {
          radioSerial->read();
        }
            delay(2);
        Serial.print(" "); Serial.print(data[i-1], HEX);
      }
      Serial.println("");
          break;
        }
      }
      
    uint32_t lowest_frequency = 7204063;
    uint16_t frequency_diff[] = { 0, 1229, 2458, 3687, 410, 1639, 2867, 4096, 819, 2048, 3277, 4506 };
    uint32_t frequency = lowest_frequency + frequency_diff[channel[0]-'A'];
    uint8_t frequencyOne = ((frequency & 0xFF0000)>>16);
    uint8_t frequencyTwo = ((frequency & 0xFF00)>>8);
    uint8_t frequencyThree = (frequency & 0xFF);
    uint8_t speed_number = (channel[1]-'0');
    uint8_t rf_factor;
    uint8_t rf_bw;
    if (speed_number == 1) {
      rf_factor =  12;
      rf_bw = 6;
    } else if (speed_number == 2) {
      rf_factor =  12;
      rf_bw = 7;
    } else if (speed_number == 3) {
      rf_factor =  12;
      rf_bw = 8;
    } else if (speed_number == 4) {
      rf_factor =  11;
      rf_bw = 9;
    } else if (speed_number == 5) {
      rf_factor =  9;
      rf_bw = 9;
    } else { //6
      rf_factor =  7;
      rf_bw = 9;
    }
    if (data[8] == 0x04 
        && data[9] == 0x00 
        && data[10] == frequencyOne 
        && data[11] == frequencyTwo
        && data[12] == frequencyThree 
        && data[13] == rf_factor 
        && data[14] == 0x00 
        && data[15] == rf_bw) {
      Serial.println("Already configured correctly");
    } else {
      uint8_t settingsArray[] = { 0xAF, 0xAF, // sync word
                                  0x00, 0x00, // id code
                                  0xAF,       // header
                                  0x80,       // command (sending)
                                  0x01,       // command type (write)
                                  0x0C,       // length (data section, from here to CS)
                                  0x04,       // baud rate (1=1200, 2=2400, 3=4800, 4=9600, 5=19200,6=38400, 7=57600)
                                  0x00,       // parity (0=no parity check, 1=odd parity, 2=even parity)
                                  frequencyOne, frequencyTwo, frequencyThree,  // frequency (The value=Frequency/61.035)
                                  rf_factor,  // rf factor (7=128, 8=256, 9=512, 10=1024, 11=2048, 12=4096)
                                  0x00,       // Mode (0=standard, 1=central, 2=node)
                                  rf_bw,       // rf_bw (6=62.5k, 7=125k, 8=250k, 9=500k)
                                  0x00,0x00,  // ID
                                  0x00,       // NetID
                                  0x07,       // RF power
                                  0x00,       // CS (calculate and set)
                                  0x0D, 0x0A  // end code
                                };
      settingsArray[20] = CalculateCS(settingsArray, 8 + settingsArray[7]);
      radioSerial->write(settingsArray,23);
  
      uint8_t data[23];
      int i = 0;
      delay(1000);
      Serial.print("Radio set response: ");
      while(radioSerial->available() && i < 23) {
        data[i++] = radioSerial->read();
        Serial.print(" "); Serial.print(data[i-1], HEX);
      }
      Serial.println("");
    }
}

float Radio::GetSlopeCoefficient()
{
  return _slopeCoefficient[_channel[1]-'1'];
}

uint8_t Radio::GetM()
{
  return _m[_channel[1]-'1'];
}

void Radio::SendData(RadioMessage *message) {
  for(uint8_t i = 0; i < message->dataLength + RADIO_HEADER_LENGTH; i++) {
    radioSerial->write(((uint8_t*)message)[i]);
    //Serial.print(" "); Serial.print(((uint8_t*)message)[i], HEX);
  }
}

void Radio::SendSIPunchDataTest(SIPunchMessage *siPunchMessage) {
  RadioMessage message;
  Util::InitRadioMessage(&message, PUNCH);
  message.dataLength = sizeof(RadioMessageDataRecord);
  message.messageNumber = 1;
  
  memcpy(message.data, (uint8_t*)&siPunchMessage->dataRecord, sizeof(RadioMessageDataRecord));
  uint16_t checkSum16 = Util::crc(message.dataLength + RADIO_HEADER_LENGTH, (uint8_t *)&message);
  message.checkSum  = (((checkSum16 >> 8) ^ (0xFF & checkSum16)) & 0xFF);
  SendData(&message);
}

uint8_t Radio::GetExpectedTotalLength() {
  uint8_t expectedTotalLength = RADIO_HEADER_LENGTH;
  if (totalReceived >=2) {
     expectedTotalLength = RADIO_HEADER_LENGTH + (receivedMessage.dataLength);
  }
  return expectedTotalLength;
}


void Radio::GetRadioDataToReceivedData(uint8_t expectedTotalLength) {
  // read until STX
  uint8_t byteRead;
  while(totalReceived == 0 && radioSerial->available()) {
     byteRead = radioSerial->read();
     //Serial.print(byteRead, HEX); Serial.print(" ");
     if (byteRead == STX) {
      receivedMessage.stx = byteRead;
      totalReceived++;
      break;
     }
  }
  
  
  //Serial.println("");
  //digitalWrite(PB0, HIGH);
  while(radioSerial->available() && totalReceived < expectedTotalLength) {
    ((uint8_t *)&receivedMessage)[totalReceived++] = radioSerial->read();
    //Serial.print("byte received: "); Serial.print(((uint8_t *)&receivedMessage)[totalReceived-1], HEX);
  }
  //digitalWrite(PB0, LOW);
}


void Radio::SendPunchData(RadioMessage *radioMessage) {
  //RadioMessage message;
  //Util::InitRadioMessage(&message, PUNCH);
  //message.dataLength = 8;
  //message.messageNumber = 1;
  
  //memcpy(message.data, radioMessage->data, 8 );
  radioMessage->checkSum = 0;
  uint8_t checkSum16 = Util::crc(radioMessage->dataLength + RADIO_HEADER_LENGTH, (uint8_t *)radioMessage);
  radioMessage->checkSum  = (((checkSum16 >> 8) ^ (0xFF & checkSum16)) & 0xFF);
  SendData(radioMessage);
}


void Radio::SendTimeSlotMessage(uint8_t *timeSlotData, uint8_t dataLength) {
  RadioMessage timeSlotMessage;
  Util::InitRadioMessage(&timeSlotMessage, ACKS_AND_TIMESLOT);
  timeSlotMessage.dataLength = dataLength;
  timeSlotMessage.messageNumber = 1;
  
  memcpy(timeSlotMessage.data, timeSlotData, dataLength );
  
  uint16_t checkSum16 = Util::crc(timeSlotMessage.dataLength + RADIO_HEADER_LENGTH, (uint8_t *)&timeSlotMessage);
  timeSlotMessage.checkSum = (((checkSum16 >> 8) ^ (0xFF & checkSum16)) & 0xFF);
  SendData(&timeSlotMessage);
}



uint8_t Radio::GetRadioData(RadioMessage *message) {
  uint8_t expectedTotalLength = GetExpectedTotalLength();
  //Serial.print("expected total: "); Serial.println(expectedTotalLength, HEX);
  GetRadioDataToReceivedData(expectedTotalLength);
  uint8_t newExpectedTotalLength = GetExpectedTotalLength();
  if (expectedTotalLength != newExpectedTotalLength) {
    GetRadioDataToReceivedData(newExpectedTotalLength);
  }

  if (totalReceived >= RADIO_HEADER_LENGTH && totalReceived == newExpectedTotalLength) {
    *message = receivedMessage;
    uint8_t retVal = totalReceived;
    totalReceived = 0;
    return retVal;
  } else {
    return 0;
  }
}





void Radio::GetRadioDataToReceivedSimpleAck() {
  // read until STX
  uint8_t byteRead;
  while(totalReceived == 0 && radioSerial->available()) {
     byteRead = radioSerial->read();
     //Serial.print(byteRead, HEX); Serial.print(" ");
     if (byteRead == STX) {
      receivedSimpleAckMessage.stx = byteRead;
      totalReceived++;
      break;
     }
  }
  
  
  //Serial.println("");
  //digitalWrite(PB0, HIGH);
  while(radioSerial->available() && totalReceived < sizeof(RadioMessageSimpleAck)) {
    ((uint8_t *)&receivedSimpleAckMessage)[totalReceived++] = radioSerial->read();
    //Serial.print("byte received: "); Serial.print(((uint8_t *)&receivedSimpleAckMessage)[totalReceived-1], HEX);
  }
  //digitalWrite(PB0, LOW);
}


uint8_t Radio::GetSimpleAckMessage(RadioMessageSimpleAck *message) {
  //Serial.print("expected total: "); Serial.println(expectedTotalLength, HEX);
  GetRadioDataToReceivedSimpleAck();

  if (totalReceived == sizeof(RadioMessageSimpleAck)) {
    *message = receivedSimpleAckMessage;
    uint8_t retVal = totalReceived;
    totalReceived = 0;
    return retVal;
  } else {
    return 0;
  }
}

void Radio::SendSimpleAckMessage(uint8_t messageNumberToAck) {
  RadioMessageSimpleAck simpleAckMessage;
  Util::InitRadioMessageSimpleAck(&simpleAckMessage);
  simpleAckMessage.messageNumberToAck = messageNumberToAck;
  
  uint16_t checkSum16 = Util::crc(sizeof(simpleAckMessage), (uint8_t *)&simpleAckMessage);
  simpleAckMessage.checkSum = (((checkSum16 >> 8) ^ (0xFF & checkSum16)) & 0xFF);

  for(uint8_t i = 0; i < sizeof(RadioMessageSimpleAck); i++) {
    radioSerial->write(((uint8_t*)&simpleAckMessage)[i]);
    //Serial.print(" "); Serial.print(((uint8_t*)&simpleAckMessage)[i], HEX);
  }
}




