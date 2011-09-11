/**
 *  @author Victor Soria
 *  @brief  Engine singleton
 */

#ifndef _PRAGMA_ENGINE_H_
#define _PRAGMA_ENGINE_H_

namespace pragma
{
    
    class Engine
    {
    public:
        static Engine* GetInstance();
        
        bool Init();
        void Shutdown();
    };
    
}


#endif//_PRAGMA_ENGINE_H_
