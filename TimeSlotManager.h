#ifndef __TimeSlotManager_H__
#define __TimeSlotManager_H__
#include <Arduino.h>
#include "Radio.h"
#include "Util.h"

struct TimeSlotInterval {
  uint32_t timeSlotStart;
  uint32_t timeSlotEnd;
};

class TimeSlotManager
{
  private:
    static uint8_t noOfTimeSlotIntervalsForThisNode;
    static TimeSlotInterval timeSlotIntervals[];
    static uint32_t timeMessageReceived;
    static uint32_t timeTimeSlotReceived;
    static uint32_t GetAvailableTimeSlotLengthMicroSeconds(uint8_t currentTimeSlotIndex);
    static int8_t GetCurrentOrNextTimeSlotIndex();
  public:
    static void RadioMessageAvailable();
    static void TimeSlotMessageReceived();
    static bool IsInStartOfTimeSlot(uint8_t messageLengthInBytes, Radio *radio);
    static void UpdateTimeSlot(RadioMessage *message);
    static uint8_t GetNoOfPunchesFittingIntoTimeSlot(Radio *radio);
 
};



#endif // __TimeSlotManager_H__
