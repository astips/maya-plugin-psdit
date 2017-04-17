#include "psditCmd.h"
using namespace std;


MSyntax PsdItCmd::newSyntax()
{
    MSyntax syntax;
    syntax.addFlag( kBaseMeshFlag, kBaseMeshLongFlag, MSyntax::kString );
    syntax.addFlag( kCorrectiveMeshFlag, kCorrectiveMeshLongFlag, MSyntax::kString );
    syntax.addFlag( kNameFlag, kNameLongFlag, MSyntax::kString );
    syntax.addFlag( kDeformerFlag, kDeformerLongFlag, MSyntax::kBoolean );
    syntax.addFlag( kHelpFlag, kHelpLongFlag );
    return syntax;
}


void *PsdItCmd::creator()
{
    return new PsdItCmd();
}


MStatus PsdItCmd::parseArgs( const MArgDatabase& argData )
{
    MStatus status = MS::kSuccess;

    // -- base mesh arg -- //
    if ( argData.isFlagSet( kBaseMeshFlag ) )
    {
        argData.getFlagArgument( kBaseMeshFlag, 0, baseName );
    }
    else if ( argData.isFlagSet( kBaseMeshLongFlag ) )
    {
        argData.getFlagArgument( kBaseMeshLongFlag, 0, baseName );
    }
    else
    {
        status = MS::kFailure;
    }

    // -- corrective mesh arg -- //
    if ( argData.isFlagSet( kCorrectiveMeshFlag ) )
    {
        argData.getFlagArgument( kCorrectiveMeshFlag, 0, correctiveName );
    }
    else if ( argData.isFlagSet( kCorrectiveMeshLongFlag ) )
    {
        argData.getFlagArgument( kCorrectiveMeshLongFlag, 0, correctiveName );
    }
    else
    {
        status = MS::kFailure;
    }

    // -- name arg -- //
    if ( argData.isFlagSet( kNameFlag ) )
    {
        argData.getFlagArgument( kNameFlag, 0, name );
    }
    else if ( argData.isFlagSet( kNameLongFlag ) )
    {
        argData.getFlagArgument( kNameLongFlag, 0, name );
    }
    else
    {
        name = "";
    }

    // -- deformer arg -- //
    if ( argData.isFlagSet( kDeformerFlag ) )
    {
        argData.getFlagArgument( kDeformerFlag, 0, keepDeformer );
    }
    else if ( argData.isFlagSet( kDeformerLongFlag ) )
    {
        argData.getFlagArgument( kDeformerLongFlag, 0, keepDeformer );
    }
    else
    {
        keepDeformer = false;
    }
    return status;
}


MStatus PsdItCmd::getBaseCorrective( MSelectionList& selectionList )
{
    if ( selectionList.length() != 2 )
    {
        MGlobal::displayError( "Select base & corrective mesh." );
        return MS::kFailure;
    }
    else
    {   
        // -- Deal with orig & base mesh -- //
        MDagPath baseDagPath;
        selectionList.getDagPath( 0, baseDagPath );
        unsigned int baseShapeCount = baseDagPath.childCount();
        if ( baseShapeCount == 0 )
        {
            MGlobal::displayError( "Wrong Selected." );
            return MS::kFailure;;
        }

        for (unsigned int i=0; i<baseShapeCount; i++)
        {
            MObject obj = baseDagPath.child(i);
            if (!obj.hasFn(MFn::kMesh))
            {
                continue;
            }

            MFnMesh tempMesh(obj);
            if (tempMesh.isIntermediateObject())
            {
                MPlug worldMeshPlug = tempMesh.findPlug( "worldMesh" );
                if ( worldMeshPlug.numConnectedElements() >= 1 )
                {   
                    origMesh.setObject(obj);
                }
            }
            else
            {
                baseMesh.setObject(obj);
            }
        }
        if (origMesh.object() == MObject::kNullObj)
        {
            MGlobal::displayError( "No intermediate shape found for base object." );
            return MS::kFailure;
        }

        // -- Deal with corrective mesh -- //
        MDagPath correctiveDagPath;
        selectionList.getDagPath( 1, correctiveDagPath );
        unsigned int correctiveShapeCount = correctiveDagPath.childCount();
        if ( correctiveShapeCount == 0 )
        {
            MGlobal::displayError( "Wrong Selected." );
            return MS::kFailure;;
        }
        for (unsigned int i=0; i<correctiveShapeCount; i++)
        {
            MObject obj = correctiveDagPath.child(i);
            if (!obj.hasFn(MFn::kMesh))
            {
                continue;
            }
            MFnMesh tempMesh(obj);
            if (tempMesh.isIntermediateObject())
            {
                continue;
            }
            else
            {
                correctiveMesh.setObject(obj);
            }
        }
        if (correctiveMesh.object() == MObject::kNullObj)
        {
            MGlobal::displayError( "No shape found for correctiveMesh object." );
            return MS::kFailure;
        }
    }
    return MS::kSuccess;;
}


MStatus PsdItCmd::doIt( const MArgList& args )
{   
    MStatus status = MS::kFailure;

    MArgDatabase argData( syntax(), args );
    if ( argData.isFlagSet( kHelpFlag ) || argData.isFlagSet( kHelpLongFlag ) )
    {
        MGlobal::displayInfo( helpStr );
        return MS::kSuccess;
    }

    status = parseArgs( argData );
    if ( !status )
    {
        MSelectionList selectionList;
        MGlobal::getActiveSelectionList( selectionList );
        if ( !getBaseCorrective(selectionList) )
        {
            return MS::kFailure;
        }
    }
    else
    {
        MSelectionList selectionList;
        selectionList.add(baseName);
        selectionList.add(correctiveName);
        if ( !getBaseCorrective(selectionList) )
        {
            return MS::kFailure;
        }
    }
    MPointArray origPointArray;
    getPoints( origMesh.fullPathName(), origPointArray );

    MPointArray basePointArray;
    getPoints( baseMesh.fullPathName(), basePointArray );

    realMesh.copy( origMesh.object(), MObject::kNullObj );
    MDagPath realMeshDagPath;
    getDagPathByName( realMesh.fullPathName(), realMeshDagPath );
    if ( !name.length() )
    {   
        MDagPath correctiveMeshDagPath;
        getDagPathByName( correctiveMesh.fullPathName(), correctiveMeshDagPath );
        MFnDagNode correctiveDagPath( correctiveMeshDagPath.transform() );
        name = correctiveDagPath.name() + "_INVERTED";
    }
    dgModifier.renameNode( realMeshDagPath.transform(), name );

    MPointArray  xPoints(origPointArray);
    MPointArray  yPoints(origPointArray);
    MPointArray  zPoints(origPointArray);

    int numPoints = origPointArray.length();
    for (int i=0; i<numPoints; i++)
    {
        xPoints[i].x += 1.0;
        yPoints[i].y += 1.0;
        zPoints[i].z += 1.0;
    }
    setPoints( origMesh.fullPathName(), xPoints );
    getPoints( baseMesh.fullPathName(), xPoints );

    setPoints( origMesh.fullPathName(), yPoints );
    getPoints( baseMesh.fullPathName(), yPoints );

    setPoints( origMesh.fullPathName(), zPoints );
    getPoints( baseMesh.fullPathName(), zPoints );

    setPoints( origMesh.fullPathName(), origPointArray );

    MString cmd_str = "deformer ";
    cmd_str += "-type psditNode ";
    cmd_str += realMesh.fullPathName();
    cmd_str += ";";
    MStringArray result;
    MGlobal::executeCommand( cmd_str, result );
    MObject deformerObj;
    getObjectByName( result[0], deformerObj );
    MFnDependencyNode deformer(deformerObj);
    MPlug matrixPlug = deformer.findPlug( "inversionMatrix", false );
    MFnMatrixData matrixData;
    for ( int i=0; i<numPoints; i++ )
    {
        double matrixPre[4][4];

        MPoint  xPoint = xPoints[i] - basePointArray[i];
        MPoint  yPoint = yPoints[i] - basePointArray[i];
        MPoint  zPoint = zPoints[i] - basePointArray[i];

        matrixPre[0][0] = xPoint.x;
        matrixPre[0][1] = xPoint.y;
        matrixPre[0][2] = xPoint.z;
        matrixPre[0][3] = 0.0;

        matrixPre[1][0] = yPoint.x;
        matrixPre[1][1] = yPoint.y;
        matrixPre[1][2] = yPoint.z;
        matrixPre[1][3] = 0.0;

        matrixPre[2][0] = zPoint.x;
        matrixPre[2][1] = zPoint.y;
        matrixPre[2][2] = zPoint.z;
        matrixPre[2][3] = 0.0;

        matrixPre[3][0] = 0.0;
        matrixPre[3][1] = 0.0;
        matrixPre[3][2] = 0.0;
        matrixPre[3][3] = 1.0;

        MMatrix matrix( matrixPre );
        matrix = matrix.inverse();
        
        MObject matrixObject( matrixData.create( matrix ) );
        matrixPlug.elementByLogicalIndex(i).setMObject( matrixObject );
    }
    MFnPointArrayData pointArrayData;
    MPlug basePointsPlug = deformer.findPlug( "basePoints", false );
    MObject basePointArrayObject( pointArrayData.create( basePointArray ) );
    basePointsPlug.setMObject( basePointArrayObject );

    dgModifier.connect(correctiveMesh.findPlug( "outMesh", false), 
                       deformer.findPlug( "correctiveMesh", false ));
    dgModifier.newPlugValueBool( deformer.findPlug( "activate", false ), true );
    dgModifier.doIt();

    if ( !keepDeformer )
    {
        MGlobal::executeCommand( "delete -ch " + realMesh.fullPathName() );
    }
    return MS::kSuccess;
}


MStatus PsdItCmd::undoIt()
{
    MDagPath realDagPath;
    getDagPathByName( realMesh.fullPathName(), realDagPath );
    MObject realObject( realDagPath.transform() );
    MGlobal::deleteNode( realObject );
    return MS::kSuccess;
}


bool PsdItCmd::isUndoable() const
{
    return true;
}


void PsdItCmd::getObjectByName( const MString& name, MObject& object )
{
    object = MObject::kNullObj;
    MSelectionList selectionList;
    selectionList.add(name);
    selectionList.getDependNode(0, object);
}


void PsdItCmd::getDagPathByName( const MString& name, MDagPath& path )
{
    MSelectionList selectionList;
    selectionList.add( name );
    selectionList.getDagPath(0, path);
}


void PsdItCmd::getPoints( const MString& name, MPointArray& points )
{
    MSelectionList tempSelectionList;
    tempSelectionList.add(name);
    MDagPath path;
    tempSelectionList.getDagPath(0, path);
    MItGeometry iter( path );
    iter.allPositions( points, MSpace::kObject );
}


void PsdItCmd::setPoints( const MString& name, MPointArray& points )
{
    MSelectionList tempSelectionList;
    tempSelectionList.add(name);
    MDagPath path;
    tempSelectionList.getDagPath(0, path);
    MItGeometry iter( path );
    iter.setAllPositions( points, MSpace::kObject );
}



MStatus initializePlugin(MObject obj)
{
    MStatus   status;
    MFnPlugin plugin( obj, "virustyle", "1.0", "Any" );
    status = plugin.registerCommand(PSDIT_CMD, 
                                    PsdItCmd::creator, 
                                    PsdItCmd::newSyntax);
    return status;
}


MStatus uninitializePlugin( MObject obj )
{
    MStatus   status;
    MFnPlugin plugin( obj );
    status = plugin.deregisterCommand( PSDIT_CMD );
    return status;
}
