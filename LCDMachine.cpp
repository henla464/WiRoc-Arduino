#include "LCDMachine.h"

AbstractState* LCDMachine::CurrentMenu;

SIStation *LCDMachine::Station;
Radio *LCDMachine::OutboundRadio;
uint8_t LCDMachine::BacklightPin;
uint8_t LCDMachine::ButtonPin;
uint8_t LCDMachine::buttonJustPressed = false;
uint8_t LCDMachine::buttonJustReleased = false;
uint8_t LCDMachine::buttonWas = BUTTON_NONE;
uint8_t LCDMachine::buttonReleased = BUTTON_NONE;
uint32_t LCDMachine::millisLastTick = 0;
uint32_t LCDMachine::millisLastButtonPress = 0;

void LCDMachine::Init(SIStation *station, Radio *outboundRadio, uint8_t buttonPin, uint8_t backlightPin)
{
   Station = station;
   OutboundRadio = outboundRadio;
   ButtonPin = buttonPin;
   BacklightPin = backlightPin;
   
    //button adc input
   pinMode( ButtonPin, INPUT );         //ensure A0 is an input
   digitalWrite( ButtonPin, LOW );      //ensure pullup is off on A0
   digitalWrite( BacklightPin, LOW );  // Set to low then control with setting it as input or output as workaround hw bug
   TurnBacklightOn();
  
   AbstractState::lcd.begin(16,2);
//   lcd.setCursor( 0, 0 );                    //top left
//   lcd.print( "  INITIALIZING  " );          //Print some initial text to the LCD.
//   lcd.setCursor( 0, 1 );                    //bottom left
//   lcd.print( "  ------------  " );
   CurrentMenu = (AbstractState*)&LCDStates::TheMainMenu;
   CurrentMenu->Init();
}

void LCDMachine::TurnBacklightOn()
{
  pinMode( BacklightPin, INPUT );     // INPUT == ON, OUTPUT == OFF
}

void LCDMachine::TurnBacklightOff()
{
  pinMode( BacklightPin, OUTPUT );     // INPUT == ON, OUTPUT == OFF
}

void LCDMachine::Tick()
{
  // no need to run too often
  if (millis() - millisLastTick < 100)
  {
    return;
  }
  millisLastTick = millis();

  if (CurrentMenu == (AbstractState*)&LCDStates::TheMainMenu)
  {
    if (millis() - millisLastButtonPress > 20000)
    {
      TurnBacklightOff();
    }
  }

  
  uint8_t button = ReadButtons();

  AbstractState* newState = NULL;
  if (CurrentMenu) 
  {
    newState = CurrentMenu->Tick();
  }
   //show text label for the button pressed
   if (buttonJustPressed  == false && buttonJustReleased == true)
   {
     if (buttonReleased != BUTTON_NONE)
     {
        millisLastButtonPress = millis();
        TurnBacklightOn();
     }
     switch( buttonReleased )
     {
        case BUTTON_RIGHT:
        {
           newState = CurrentMenu->Right();
           break;
        }
        case BUTTON_UP:
        {
           newState = CurrentMenu->Up();
           break;
        }
        case BUTTON_DOWN:
        {
           newState = CurrentMenu->Down();
           break;
        }
        case BUTTON_LEFT:
        {
          newState = CurrentMenu->Left();
          break;
       }
       case BUTTON_SELECT:
       {
          newState = CurrentMenu->Select();  
          break;
       }
     }
   }
   buttonJustPressed = false;
   buttonJustReleased = false;

   if (newState) {
     millisLastButtonPress = millis();
     CurrentMenu = newState;
     newState = NULL;
     CurrentMenu->Init();
   }
}


/*--------------------------------------------------------------------------------------
  ReadButtons()
  Detect the button pressed and return the value
  Uses static class values buttonWas, buttonJustPressed, buttonJustReleased.
--------------------------------------------------------------------------------------*/
uint8_t LCDMachine::ReadButtons()
{
   unsigned int buttonVoltage;
   byte button = BUTTON_NONE;   // return no button pressed if the below checks don't write to btn
   
   //read the button ADC pin voltage
   buttonVoltage = analogRead( ButtonPin );

   //sense if the voltage falls within valid voltage windows
   if( buttonVoltage < ( RIGHT_10BIT_ADC + BUTTONHYSTERESIS ) )
   {
      button = BUTTON_RIGHT;
   }
   else if(   buttonVoltage >= ( UP_10BIT_ADC - BUTTONHYSTERESIS )
           && buttonVoltage <= ( UP_10BIT_ADC + BUTTONHYSTERESIS ) )
   {
      button = BUTTON_UP;
   }
   else if(   buttonVoltage >= ( DOWN_10BIT_ADC - BUTTONHYSTERESIS )
           && buttonVoltage <= ( DOWN_10BIT_ADC + BUTTONHYSTERESIS ) )
   {
      button = BUTTON_DOWN;
   }
   else if(   buttonVoltage >= ( LEFT_10BIT_ADC - BUTTONHYSTERESIS )
           && buttonVoltage <= ( LEFT_10BIT_ADC + BUTTONHYSTERESIS ) )
   {
      button = BUTTON_LEFT;
   }
   else if(   buttonVoltage >= ( SELECT_10BIT_ADC - BUTTONHYSTERESIS )
           && buttonVoltage <= ( SELECT_10BIT_ADC + BUTTONHYSTERESIS ) )
   {
      button = BUTTON_SELECT;
   }
   //handle button flags for just pressed and just released events
   if( ( buttonWas == BUTTON_NONE ) && ( button != BUTTON_NONE ) )
   {
      //the button was just pressed, set buttonJustPressed, this can optionally be used to trigger a once-off action for a button press event
      //it's the duty of the receiver to clear these flags if it wants to detect a new button change event
      buttonJustPressed  = true;
      buttonJustReleased = false;
   }
   if( ( buttonWas != BUTTON_NONE ) && ( button == BUTTON_NONE ) )
   {
      buttonJustPressed  = false;
      buttonJustReleased = true;
   }
      
   //save the latest button value, for change event detection next time round
   buttonReleased = buttonWas;
   buttonWas = button;
   
   return( button );
}
