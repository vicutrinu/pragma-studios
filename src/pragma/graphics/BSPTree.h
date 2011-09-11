/**
 *  @file   BSPTree.h
 *  @author Victor Soria
 **/

#ifndef _PRAGMA_SRC_GRAPHICS_BSPTREE_H_
#define _PRAGMA_SRC_GRAPHICS_BSPTREE_H_

namespace pragma
{
    
    class Mesh;
    
    class BSPTree
    {
    public:
        bool    Init(const Mesh& aMesh);
    void    End();
    
    bool    IsOk() const;

    private:
        struct Data;
        Data*   mData;
    };

} // Closing namespace pragma
    
#endif//_PRAGMA_SRC_GRAPHICS_BSPTREE_H_