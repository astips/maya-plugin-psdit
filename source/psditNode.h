#ifndef _PSDIT_NODE
#define _PSDIT_NODE

#include <maya/MStatus.h>
#include <maya/MMatrix.h>
#include <maya/MTypeId.h> 
#include <maya/MFnMesh.h>
#include <maya/MFnData.h>
#include <maya/MPxDeformerNode.h>
#include <maya/MDataBlock.h>
#include <maya/MItGeometry.h>
#include <maya/MFnPlugin.h>
#include <maya/MObject.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNumericData.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MFnPointArrayData.h>
#include <maya/MMatrixArray.h>
#include <maya/MVector.h>
#include <maya/MThreadUtils.h>
#include <maya/MArrayDataBuilder.h>


#define     PSDIT_NODE    "psditNode"

class PsdItNode : public MPxDeformerNode
{
public:
                                PsdItNode() {};
    virtual                     ~PsdItNode() {};

    static        void*         creator();
    static        MStatus       initialize();

    virtual       MStatus       deform( MDataBlock& block, 
                                        MItGeometry& iter, 
                                        const MMatrix& localToWorldMatrix, 
                                        unsigned int geomIndex );
    
    static        MStatus       jumpToElement(MArrayDataHandle& arrayHandle, unsigned int index);

public:
    static  const MTypeId       id;
    static        MObject       activate;
    static        MObject       correctiveMesh;
    static        MObject       inversionMatrix;
    static        MObject       basePoints;

    MPointArray                 basePointsCopy;
    MPointArray                 correctivePoints;
    MMatrixArray                homeMMatrixarray;
};


#endif
