#include "psditNode.h"

using namespace std;

MObject PsdItNode::activate;
MObject PsdItNode::basePoints;
MObject PsdItNode::correctiveMesh;
MObject PsdItNode::inversionMatrix;


const MTypeId PsdItNode::id( 0x80000 );


void* PsdItNode::creator()
{
    return new PsdItNode();
}


MStatus PsdItNode::initialize()
{
    MFnNumericAttribute   nAttr;
    MFnTypedAttribute     tAttr;
    MFnMatrixAttribute    mAttr;

    PsdItNode::activate = nAttr.create( "activate", "activate", MFnNumericData::kBoolean, 0 );
    addAttribute(PsdItNode::activate);

    PsdItNode::correctiveMesh = tAttr.create( "correctiveMesh", "cm", MFnData::kMesh );
    addAttribute(PsdItNode::correctiveMesh);

    PsdItNode::basePoints = tAttr.create( "basePoints", "bp", MFnData::kPointArray );
    addAttribute(PsdItNode::basePoints);

    PsdItNode::inversionMatrix = mAttr.create( "inversionMatrix", "im", MFnMatrixAttribute::kDouble );
    mAttr.setDefault( MMatrix::identity );
    mAttr.setArray(true);
    mAttr.setHidden(true);
    mAttr.setUsesArrayDataBuilder(true);
    addAttribute( PsdItNode::inversionMatrix );

    attributeAffects( PsdItNode::activate, PsdItNode::outputGeom );
    attributeAffects( PsdItNode::correctiveMesh, PsdItNode::outputGeom );
    return MS::kSuccess;
}


MStatus PsdItNode::jumpToElement( MArrayDataHandle& arrayHandle, unsigned int index )
{
    MStatus status;
    status = arrayHandle.jumpToElement(index);
    if ( !status )
    {
        MArrayDataBuilder builder = arrayHandle.builder();
        builder.addElement(index);
        arrayHandle.set(builder);
        arrayHandle.jumpToElement(index);
    }
    return MS::kSuccess;
}


MStatus PsdItNode::deform(MDataBlock& block, 
                          MItGeometry& iter, 
                          const MMatrix& localToWorldMatrix, 
                          unsigned int geomIndex)
{
    MThreadUtils::syncNumOpenMPThreads();

    if (block.inputValue(activate).asBool() == 0)
        return MS::kSuccess;

    MObject basePointsArrayDataObject = block.inputValue(basePoints).data();
    MFnPointArrayData basePointsArrayData(basePointsArrayDataObject);
    basePointsArrayData.copyTo(basePointsCopy);
    if (basePointsCopy.length() == 0)
        return MS::kSuccess;

    MFnMesh crtMesh(block.inputValue(correctiveMesh).asMesh());
    crtMesh.getPoints(correctivePoints);

    MArrayDataHandle hMatrix = block.inputArrayValue(inversionMatrix);
    for (int i=0; i<correctivePoints.length(); i++)
    {
        jumpToElement(hMatrix, i);
        MMatrix mat = hMatrix.inputValue().asMatrix();
        homeMMatrixarray.append(mat);
    }

    MPointArray verts;
    iter.allPositions(verts);
    for (int index=0; index<verts.length(); index++)
    {
        MVector delta = correctivePoints[index] - basePointsCopy[index];
        MPoint offset = delta * homeMMatrixarray[index];
        if (offset.distanceTo(verts[index]) > 0.0001)
        {
            verts[index] += offset;
        }
        
    }
    iter.setAllPositions(verts);
    return MS::kSuccess;
}


MStatus initializePlugin(MObject obj)
{
    MStatus   status;
    MFnPlugin plugin(obj, "virustyle", "1.0", "Any");

    status = plugin.registerNode(PSDIT_NODE, 
                                 PsdItNode::id, 
                                 PsdItNode::creator, 
                                 PsdItNode::initialize, 
                                 MPxNode::kDeformerNode);
    return status;
}


MStatus uninitializePlugin(MObject obj)
{
    MStatus   status;
    MFnPlugin plugin(obj);
    status = plugin.deregisterNode(PsdItNode::id);
    return status;
}
