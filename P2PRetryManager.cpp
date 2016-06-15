#include "P2PRetryManager.h"
#include "Settings.h"

uint32_t P2PRetryManager::timeMessageSent;
bool P2PRetryManager::isAckReceived;

void P2PRetryManager::MessageSent()
{
  timeMessageSent = micros();
  isAckReceived = false;
}

void P2PRetryManager::AckMessageReceived()
{
  isAckReceived = true;
}

bool P2PRetryManager::IsAckReceivedOrTimeout()
{
  if (isAckReceived) 
  {
    return true;
  }
  if (micros() - timeMessageSent > 6000000)
  {
    return true;
  }
  return false;
}
