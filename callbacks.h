#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <AntTweakBar.h>

class ICallbacks
{
    public:
        virtual void SpecialKeyboardCB(int Key, int x, int y) = 0;
        virtual void KeyboardCB(unsigned char Key, int x, int y) = 0;
        virtual void PassiveMouseCB(int x, int y) = 0;
        virtual void RenderSceneCB() = 0;
        virtual void IdleCB() = 0;
        virtual ~ICallbacks() {}
        virtual void InitTwBar(TwBar *GUI) = 0;
};

#endif /* CALLBACKS_H */
