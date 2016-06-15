#ifndef __AbstractState_H__
#define __AbstractState_H__
#include <Arduino.h>
#include <LiquidCrystal.h>

class AbstractState {
    public:
        static LiquidCrystal lcd;
        AbstractState();
        virtual void Init();
        void ClearScreen();
        virtual AbstractState* Select();
        virtual AbstractState* Left();
        virtual AbstractState* Right();
        virtual AbstractState* Up();
        virtual AbstractState* Down();
        virtual AbstractState* Tick();
    protected:
        uint32_t millisAtInit;
        void resetTime();
    private:
};

#endif // __AbstractState_H__
