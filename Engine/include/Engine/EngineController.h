#pragma once

#include "Engine/Interface.h"


namespace engine
{

class Engine;

class EngineController : public Interface
{
public:
    virtual void setEngine(Engine* engine) = 0;

    virtual bool exec(unsigned int deltaTime) = 0;
};


}//namespace engine
