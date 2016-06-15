#ifndef __P2PRetryManager_H__
#define __P2PRetryManager_H__
#include <Arduino.h>
#include "Radio.h"
#include "Util.h"


class P2PRetryManager
{
  private:
    static bool isAckReceived;
    static uint32_t timeMessageSent;
  public:
    static void MessageSent();
    static void AckMessageReceived();
    static bool IsAckReceivedOrTimeout();
};



#endif // __P2PRetryManager_H__
