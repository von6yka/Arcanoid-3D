#pragma once

#include "Engine/Interface.h"

namespace engine
{


class WindowObserver : public Interface
{
public:
    virtual void onWindowResized() = 0;
};


}//namespace engine
