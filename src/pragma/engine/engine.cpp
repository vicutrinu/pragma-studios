/**
 *  @file   engine.cpp
 *  @author Victor Soria
 **/

#include "pragma/engine/engine.h"

namespace pragma
{

    //--
    Engine* Engine::GetInstance()
    {
        static Engine sEngine;
        return &sEngine;
    }
    
    //--
    bool Engine::Init()
    {
        bool lOk = true;
        
        return lOk;
    }

    //--
    void Engine::Shutdown()
    {
        
    }
}
