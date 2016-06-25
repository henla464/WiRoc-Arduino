#ifndef __LCDStates_H__
#define __LCDStates_H__
#include <Arduino.h>
#include "MainMenu.h"
#include "SettingsMenu.h"
#include "OutboundChannelMenu.h"
#include "SplashMenu.h"
#include "StationNumberMenu.h"
#include "BatteryMenu.h"
#include "SystemInfoMenu.h"
#include "HiddenMenu.h"
#include "InboundChannelMenu.h"
#include "TestMenu.h"
#include "TestSendInSlotMenu.h"
#include "TestForceSendMenu.h"
#include "TestReceiveMenu.h"
#include "InboundRadioNodeMenu.h"
#include "AddNodeMenu.h"
#include "OutboundRadioModeMenu.h"
#include "InboundRadioModeMenu.h"
#include "EraseDBMenu.h"
#include "BootMenu.h"

class LCDStates {
    public:
        static MainMenu TheMainMenu;
        static SettingsMenu TheSettingsMenu;
        static InboundRadioNodeMenu TheInboundRadioNodeMenu;
        static OutboundChannelMenu TheOutboundChannelMenu;
        static SplashMenu TheSplashMenu;
        static StationNumberMenu TheStationNumberMenu;
        static BatteryMenu TheBatteryMenu;
        static SystemInfoMenu TheSystemInfoMenu;
        static HiddenMenu TheHiddenMenu;
        static InboundChannelMenu TheInboundChannelMenu;
        static TestMenu TheTestMenu;
        static TestSendInSlotMenu TheTestSendInSlotMenu;
        static TestForceSendMenu TheTestForceSendMenu;
        static TestReceiveMenu TheTestReceiveMenu;
        static AddNodeMenu TheAddNodeMenu;
        static OutboundRadioModeMenu TheOutboundRadioModeMenu;
        static InboundRadioModeMenu TheInboundRadioModeMenu;
        static EraseDBMenu TheEraseDBMenu;
        static BootMenu TheBootMenu;
};

#endif // __LCDStates_H__
