//#define IS_STATION_MODULE
#define IS_RELAY_MODULE

// SI punch message
#define STX 0x02
#define ETX 0x03
#define COMMAND_CODE_TRANSMIT_RECORD 0xD3

// Radio message
#define RADIO_HEADER_LENGTH 0x05
#define NO_OF_BYTES_PER_PUNCH 0x08
#define MAX_NO_OF_TIMESLOTS_PER_ROUND 0x07

#define RADIO_MESSAGE_VERSION 0x00
#define RADIO_MESSAGE_SIMPLE_ACK_VERSION 0x00

#define PUNCH (uint8_t)0x00
#define ACKS_AND_TIMESLOT (uint8_t)0x01
#define COMBINED_PUNCH (uint8_t)0x02


//Radio modes
#define P2P_RETRY 0x00
#define P2P_FIRE_FORGET 0x01
#define P2M_RETRY 0x02

// Pins in use
#define BUTTON_ADC_PIN           A0  // A0 is the button ADC input
#define LCD_BACKLIGHT_PIN        A2  // Control backlight pin
#define OUTBOUND_AUX_PIN         21  // outbound radio aux pin

#define SOFTWARE_VERSION "1"
