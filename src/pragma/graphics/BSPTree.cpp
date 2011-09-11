/**
 *  @file   BSPTree.cpp
 *  @author Victor Soria
 **/

#include <pragma/graphics/BSPTree.h>
#include <pragma/graphics/Mesh.h>
#include <pragma/math/functions.h>
#include <pragma/geometry/functions.h>

#define MAX_PARTITION_SIZE 10

namespace pragma
{
    struct Plane
    {
        Plane(const Point& aPoint, const vector3f& aNormal)
        : mPoint(aPoint)
        , mNormal(aNormal)
        {
            mDistance = DotProduct(aPoint, aNormal);
        }
        vector3f    mNormal;
        float       mDistance;
        Point       mPoint;
    };
    
    static Point intersect_line_plane(const Point& aA, const Point& aB, const Plane& aPlane)
    {
        vector3f lDir = Normalize(aB - aA);
        
        float lD = DotProduct( (aPlane.mPoint - aA), aPlane.mNormal ) / DotProduct( lDir, aPlane.mNormal );
        
        return aA * lD;
    }

//----------------------------------------------------------------------------------------------------------------------
struct BSPTree::Data
{
    struct TTri
    {
        TTri(const Point& aA, const Point& aB, const Point& aC, unsigned aTriangleID)
        : mA(aA), mB(aB), mC(aC), mTriangle(aTriangleID)
        { }
        Point       mA;
        Point       mB;
        Point       mC;
        unsigned    mTriangle;
    };
    
    struct BSPNode
    {
        BSPNode() : mPartitionPlane(-1), mFront(0), mBack(0) { }
        std::vector<TTri>   mTriangles;
        int                 mPartitionPlane;
        BSPNode*            mFront;
        BSPNode*            mBack;
    };
    
    std::vector<TTri> mTriangles;

    Data(const Mesh& aMesh)
    {
        size_t lCount;
        const Mesh::TTriangle* lTriangles = aMesh.GetTriangles(lCount);
        size_t lVertexCount;
        const Point* lVertices = aMesh.GetVertexs(lVertexCount);
        mTriangles.reserve(lCount);
        for(size_t i = 0; i < lCount; i++)
        {
            mTriangles.push_back( TTri( lVertices[lTriangles[i].mVertex[0]]
                                      , lVertices[lTriangles[i].mVertex[1]]
                                      , lVertices[lTriangles[i].mVertex[2]]
                                      , i ) );
        }
    }

    void split(BSPNode* aNode)
    {
        if(aNode->mTriangles.size() <= MAX_PARTITION_SIZE)
        {
            return;
        }
        aNode->mFront = new BSPNode;
        aNode->mBack = new BSPNode;
        int lBest = 0;
        int lBestScore = 0;
        for(size_t i = 0; i < aNode->mTriangles.size(); i++)
        {
            int lFrontCount = 0;
            int lBackCount = 0;
            vector3f lNormal = CrossProduct( aNode->mTriangles[i].mB - aNode->mTriangles[i].mA
                                           , aNode->mTriangles[i].mC - aNode->mTriangles[i].mA );
            Plane lPlane(aNode->mTriangles[i].mA, lNormal);
            split_count(aNode->mTriangles, lPlane, lFrontCount, lBackCount);
            int lScore = abs(lFrontCount - lBackCount);
            if(lScore < lBestScore || i == 0)
            {
                lBest = i;
                lBestScore = lScore;
            }
        }
        const TTri& lBestTri = aNode->mTriangles[lBest];
        vector3f lNormal = CrossProduct(lBestTri.mB -lBestTri.mA, lBestTri.mC - lBestTri.mA);
        Plane lBestPlane(aNode->mTriangles[lBest].mA, lNormal);
        split(aNode->mTriangles, lBestPlane, aNode->mFront->mTriangles, aNode->mBack->mTriangles);
        split(aNode->mFront);
        split(aNode->mBack);
    }

    void split_count(const std::vector<TTri>& aInput, const Plane& aPlane, int& aFront, int& aBack)
    {
        aFront = aBack = 0;
        for(size_t i = 0; i < aInput.size(); i++)
        {
            const Point& lA = aInput[i].mA;
            const Point& lB = aInput[i].mB;
            const Point& lC = aInput[i].mC;
            int lSignA = DotProduct(lA, aPlane.mNormal) - aPlane.mDistance;
            int lSignB = DotProduct(lB, aPlane.mNormal) - aPlane.mDistance;
            int lSignC = DotProduct(lC, aPlane.mNormal) - aPlane.mDistance;
            
            if( lSignA >= 0 && lSignB >= 0 && lSignC >= 0 )
            {
                aFront++;
            }
            else if( lSignA < 0 && lSignB < 0 && lSignC < 0 )
            {
                aBack++;
            }
            else
            {
                if(lSignA >= 0 && lSignB >= 0 && lSignC < 0)        // 001
                {
                    aBack++;
                    aFront+= 2;
                }
                else if(lSignA >= 0 && lSignB < 0 && lSignC >= 0)   // 010
                {
                    aBack++;
                    aFront+= 2;
                }
                if(lSignA >= 0 && lSignB < 0 && lSignC < 0)         // 011
                {
                    aBack+= 2;
                    aFront++;                }
                if(lSignA < 0 && lSignB >= 0 && lSignC >= 0)        // 100
                {
                    aBack++;
                    aFront+= 2;
                }
                if(lSignA < 0 && lSignB >= 0 && lSignC < 0)         // 101
                {
                    aBack+= 2;
                    aFront++;
                }
                if(lSignA < 0 && lSignB < 0 && lSignC >= 0)         // 110
                {
                    aBack++;
                    aFront+= 2;
                }
            }
        }
    }
    
    void split(const std::vector<TTri>& aInput, const Plane& aPlane, std::vector<TTri>& aFront, std::vector<TTri>& aBack)
    {
        for(size_t i = 0; i < aInput.size(); i++)
        {
            const Point& lA = aInput[i].mA;
            const Point& lB = aInput[i].mB;
            const Point& lC = aInput[i].mC;
            int lSignA = DotProduct(lA, aPlane.mNormal) - aPlane.mDistance;
            int lSignB = DotProduct(lB, aPlane.mNormal) - aPlane.mDistance;
            int lSignC = DotProduct(lC, aPlane.mNormal) - aPlane.mDistance;

            if( lSignA >= 0 && lSignB >= 0 && lSignC >= 0 )
            {
                aFront.push_back(aInput[i]);
            }
            else if( lSignA < 0 && lSignB < 0 && lSignC < 0 )
            {
                aBack.push_back(aInput[i]);
            }
            else
            {
                if(lSignA >= 0 && lSignB >= 0 && lSignC < 0)        // 001
                {
                    vector3f lNewA = intersect_line_plane( aInput[i].mA, aInput[i].mC, aPlane );
                    vector3f lNewB = intersect_line_plane( aInput[i].mB, aInput[i].mC, aPlane );
                    aBack.push_back( TTri(lNewA, lNewB, lC, aInput[i].mTriangle) );
                    aFront.push_back( TTri( lNewA, aInput[i].mA, lB, aInput[i].mTriangle) );
                    aFront.push_back( TTri( lNewA, lB, lNewB, aInput[i].mTriangle) );
                }
                else if(lSignA >= 0 && lSignB < 0 && lSignC >= 0)   // 010
                {
                    vector3f lNewA = intersect_line_plane( aInput[i].mA, aInput[i].mB, aPlane );
                    vector3f lNewC = intersect_line_plane( aInput[i].mC, aInput[i].mB, aPlane );
                    aFront.push_back( TTri(   lA, lNewA, lC, aInput[i].mTriangle) );
                    aFront.push_back( TTri(lNewA, lNewC, lC, aInput[i].mTriangle) );
                    aBack.push_back ( TTri(lNewA, lB, lNewC, aInput[i].mTriangle) );
                }
                if(lSignA >= 0 && lSignB < 0 && lSignC < 0)         // 011
                {
                    vector3f lNewB = intersect_line_plane( aInput[i].mB, aInput[i].mA, aPlane );
                    vector3f lNewC = intersect_line_plane( aInput[i].mC, aInput[i].mA, aPlane );
                    aFront.push_back( TTri(   lA, lNewB, lNewC, aInput[i].mTriangle) );
                    aBack.push_back ( TTri(lNewB,    lB,    lC, aInput[i].mTriangle) );
                    aBack.push_back ( TTri(lNewB,    lC, lNewC, aInput[i].mTriangle) );
                }
                if(lSignA < 0 && lSignB >= 0 && lSignC >= 0)        // 100
                {
                    vector3f lNewB = intersect_line_plane( aInput[i].mB, aInput[i].mA, aPlane );
                    vector3f lNewC = intersect_line_plane( aInput[i].mC, aInput[i].mA, aPlane );
                    aFront.push_back( TTri(lNewC, lNewB,    lB, aInput[i].mTriangle) );
                    aFront.push_back( TTri(lNewC,    lB,    lC, aInput[i].mTriangle) );
                    aBack.push_back ( TTri(   lA, lNewB, lNewC, aInput[i].mTriangle) );
                }
                if(lSignA < 0 && lSignB >= 0 && lSignC < 0)         // 101
                {
                    vector3f lNewA = intersect_line_plane( aInput[i].mA, aInput[i].mB, aPlane );
                    vector3f lNewC = intersect_line_plane( aInput[i].mC, aInput[i].mB, aPlane );
                    aFront.push_back( TTri(lNewA,    lB, lNewC, aInput[i].mTriangle) );
                    aBack.push_back ( TTri(   lA, lNewA, lNewC, aInput[i].mTriangle) );
                    aBack.push_back ( TTri(   lA, lNewC,    lC, aInput[i].mTriangle) );
                }
                if(lSignA < 0 && lSignB < 0 && lSignC >= 0)         // 110
                {
                    vector3f lNewA = intersect_line_plane( aInput[i].mA, aInput[i].mC, aPlane );
                    vector3f lNewB = intersect_line_plane( aInput[i].mB, aInput[i].mC, aPlane );
                    aFront.push_back( TTri(lA,    lB, lNewB, aInput[i].mTriangle) );
                    aFront.push_back( TTri(lA, lNewB, lNewA, aInput[i].mTriangle) );
                    aBack.push_back ( TTri(lNewA, lNewB, lC, aInput[i].mTriangle) );
                }
            }
        }
    }
};

//----------------------------------------------------------------------------------------------------------------------
//  Init
//----------------------------------------------------------------------------------------------------------------------
bool BSPTree::Init(const Mesh &aMesh)
{
    if(IsOk())
        End();

    bool lRetVal = true;
    
    mData = new Data(aMesh);

    Data::BSPNode lRoot;
    lRoot.mTriangles = mData->mTriangles;
    mData->split(&lRoot);
    
    return lRetVal;
}


//----------------------------------------------------------------------------------------------------------------------
//  End
//----------------------------------------------------------------------------------------------------------------------
void BSPTree::End()
{
    
}


//----------------------------------------------------------------------------------------------------------------------
//  IsOk
//----------------------------------------------------------------------------------------------------------------------
bool BSPTree::IsOk() const
{
    return true;
}

} // Closing namespace pragma
