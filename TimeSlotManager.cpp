#include "TimeSlotManager.h"
#include "Settings.h"

uint32_t TimeSlotManager::timeMessageReceived;
uint32_t TimeSlotManager::timeTimeSlotReceived;
uint8_t TimeSlotManager::noOfTimeSlotIntervalsForThisNode;
TimeSlotInterval TimeSlotManager::timeSlotIntervals[MAX_NO_OF_TIMESLOTS_PER_ROUND];

bool TimeSlotManager::IsInStartOfTimeSlot(uint8_t messageLengthInBytes, Radio *radio)
{
  int8_t currentTimeSlotIndex = GetCurrentOrNextTimeSlotIndex();
  if (currentTimeSlotIndex < 0)
  {
    return false;
  }
  float k = radio->GetSlopeCoefficient();
  uint8_t m = radio->GetM();
  uint32_t requiredTimeSlotLengthMicroSeconds = k * (messageLengthInBytes + m);
  uint32_t timeNow = micros();
  bool passedStart = (timeNow >= timeSlotIntervals[currentTimeSlotIndex].timeSlotStart);
  uint32_t endOfStart = timeSlotIntervals[currentTimeSlotIndex].timeSlotEnd - requiredTimeSlotLengthMicroSeconds - Util::TimeSlotMargin;
  bool beforeEndOfStart = (timeNow <= endOfStart);
  Serial.print("End of start; "); Serial.println(endOfStart);
  /*Serial.print("Time now: "); Serial.print(timeNow);
  Serial.print(" | > timeSlotStart "); Serial.print(timeSlotIntervals[currentTimeSlotIndex].timeSlotStart);
  if (passedStart)
  {
    Serial.print(" TRUE | < timeSlotend ");
  } else {
    Serial.print(" FALSE | < timeSlotend ");
  }
  Serial.print(timeSlotIntervals[currentTimeSlotIndex].timeSlotEnd - requiredTimeSlotLengthMicroSeconds - 2*Util::TimeSlotMargin);
  if (beforeEndOfStart)
  {
    Serial.print(" TRUE  ");
  } else {
    Serial.print(" FALSE ");
  }*/
  //Serial.print("requiredTimeSlotLengthMicroSeconds: "); Serial.println(requiredTimeSlotLengthMicroSeconds);
  //Serial.println(timeSlotIntervals[currentTimeSlotIndex].timeSlotEnd);

  return (passedStart && beforeEndOfStart);
}


void TimeSlotManager::RadioMessageAvailable()
{
  TimeSlotManager::timeMessageReceived = micros();
}

void TimeSlotManager::TimeSlotMessageReceived()
{
  TimeSlotManager::timeTimeSlotReceived = TimeSlotManager::timeMessageReceived;
}


void TimeSlotManager::UpdateTimeSlot(RadioMessage *radioMessageTimeSlot)
{
  bool nodeFound = false;
  uint8_t sumTimeSlotLengthTotal = 0;
  uint8_t sumTimeSlotLengthBeforeNode = 0;
  uint8_t nodeTimeSlotLength = 0;
  uint8_t noOfTimeSlotsForThisNode = 0;
  uint8_t thisNodeNumber = Settings::GetNodeNumber();
  uint8_t noOfTimeSlotRecords = radioMessageTimeSlot->dataLength / sizeof(RadioMessageDataTimeSlotRecord);
  RadioMessageDataTimeSlotRecord* timeSlot = (RadioMessageDataTimeSlotRecord*) & (radioMessageTimeSlot->data);
  for (uint8_t i = 0; i < noOfTimeSlotRecords; i++)
  {
    sumTimeSlotLengthTotal += (timeSlot + i)->timeSlotLength;
  }
  for (uint8_t i = 0; i < noOfTimeSlotRecords && noOfTimeSlotsForThisNode < MAX_NO_OF_TIMESLOTS_PER_ROUND; i++)
  {
    uint8_t timeSlotLength = timeSlot->timeSlotLength;
    //Serial.print("timeslotlength: "); Serial.println(timeSlotLength);

    if (timeSlot->nodeNumber == thisNodeNumber && timeSlotLength > 0)
    {
      nodeFound = true;
      nodeTimeSlotLength = timeSlotLength;

      //Serial.print("outbound interval length: ");
      //Serial.println(Settings::GetOutboundRadioTotalSlotLengthMicroSeconds());
      //Serial.print("timeslot length total: ");
      //Serial.println(sumTimeSlotLengthTotal);

      uint32_t nodeTimeSlotLengthMicroSeconds = ((nodeTimeSlotLength * Settings::GetOutboundRadioTotalSlotLengthMicroSeconds()) / sumTimeSlotLengthTotal);
      //Serial.print("node timeslot length: ");
      //Serial.println(nodeTimeSlotLengthMicroSeconds);
      uint32_t timeBeforeNodeMicroSeconds = ((sumTimeSlotLengthBeforeNode * Settings::GetOutboundRadioTotalSlotLengthMicroSeconds()) / sumTimeSlotLengthTotal);
      uint32_t timeSlotStart = timeTimeSlotReceived + timeBeforeNodeMicroSeconds + Util::TimeSlotMargin;
      uint32_t timeSlotEnd = timeTimeSlotReceived + timeBeforeNodeMicroSeconds + nodeTimeSlotLengthMicroSeconds - Util::TimeSlotMargin;
      timeSlotIntervals[noOfTimeSlotsForThisNode].timeSlotStart = timeSlotStart;
      timeSlotIntervals[noOfTimeSlotsForThisNode].timeSlotEnd = timeSlotEnd;
      Serial.print("Update timeslot, timeSlotStart: "); Serial.print(timeSlotStart); Serial.print(" timeSlotEnd: "); Serial.println(timeSlotEnd);
      noOfTimeSlotsForThisNode++;
    }

    sumTimeSlotLengthBeforeNode += timeSlotLength;
    timeSlot++;
  }
  noOfTimeSlotIntervalsForThisNode = noOfTimeSlotsForThisNode;
  Serial.println("End update timeslot");
}


uint8_t TimeSlotManager::GetNoOfPunchesFittingIntoTimeSlot(Radio *radio)
{
  int8_t currentTimeSlotIndex = GetCurrentOrNextTimeSlotIndex();
  if (currentTimeSlotIndex < 0)
  {
    // No timeslot received
    return 0;
  }
  uint32_t timeSlotLengthMicroSeconds = GetAvailableTimeSlotLengthMicroSeconds(currentTimeSlotIndex);
  //Serial.print("timeSlotLengthMicroSeconds: "); Serial.println(timeSlotLengthMicroSeconds);
  if (timeSlotLengthMicroSeconds > 0)
  {
    float k = radio->GetSlopeCoefficient();
    uint8_t m = radio->GetM();
    int16_t noOfBytes = timeSlotLengthMicroSeconds / k - m;
    int16_t noOfPunches = (noOfBytes - RADIO_HEADER_LENGTH) / NO_OF_BYTES_PER_PUNCH;
    //Serial.print("noOfBytes: "); Serial.println(noOfBytes);
    //Serial.print("noOfPunches: "); Serial.println(noOfPunches);
    if (noOfPunches > 0)
    {
      return noOfPunches;
    }
  }
  return 0;
}

uint32_t TimeSlotManager::GetAvailableTimeSlotLengthMicroSeconds(uint8_t timeSlotIndex)
{
  if (timeSlotIntervals[timeSlotIndex].timeSlotEnd > max(timeSlotIntervals[timeSlotIndex].timeSlotStart, micros() + Util::TimeSlotMargin))
  {
    return timeSlotIntervals[timeSlotIndex].timeSlotEnd - max(timeSlotIntervals[timeSlotIndex].timeSlotStart, micros()) - Util::TimeSlotMargin;
  }
  return 0;
}

int8_t TimeSlotManager::GetCurrentOrNextTimeSlotIndex()
{
  uint32_t currentMicros = micros();
  //Serial.print("currentMicros: "); Serial.println(currentMicros);
  for (uint8_t i = 0; i < noOfTimeSlotIntervalsForThisNode; i++)
  {
    //Serial.print(timeSlotIntervals[i].timeSlotStart); Serial.print(" "); Serial.println(timeSlotIntervals[i].timeSlotEnd);
    if (currentMicros <= timeSlotIntervals[i].timeSlotEnd)
    {
      //Serial.print("TimeSlotIndex: "); Serial.println(i);
      return i;
    }
  }
  return -1;
}

