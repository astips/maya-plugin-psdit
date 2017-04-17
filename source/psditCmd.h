#ifndef _PSDIT_CMD
#define _PSDIT_CMD

#include <maya/MSyntax.h>
#include <maya/MFnMesh.h>
#include <maya/MGlobal.h>
#include <maya/MString.h>
#include <maya/MPxCommand.h>
#include <maya/MFnPlugin.h>
#include <maya/MArgList.h>
#include <maya/MArgDatabase.h>
#include <maya/MSelectionList.h>
#include <maya/MPointArray.h>
#include <maya/MItGeometry.h>
#include <maya/MDagPath.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MSelectionList.h>
#include <maya/MMatrix.h>
#include <maya/MFnMatrixData.h>
#include <maya/MFnPointArrayData.h>
#include <maya/MDGModifier.h>

#define     kBaseMeshFlag               "-b"
#define     kBaseMeshLongFlag           "-base"

#define     kCorrectiveMeshFlag         "-c"
#define     kCorrectiveMeshLongFlag     "-corrective"

#define     kNameFlag                   "-n"
#define     kNameLongFlag               "-name"

#define     kDeformerFlag               "-d"
#define     kDeformerLongFlag           "-deformer"

#define     kHelpFlag                   "-h"
#define     kHelpLongFlag               "-help"


#define     PSDIT_CMD                   "psditCmd"

#define helpStr "psditCmd Help : \n// -b  --base   < string : base object name >\n// \
-c  --corrective   < string : correctived object name >\n// \
-n  --name   < string : inverter object name >\n// -d  --deformer   < bool : if keep psditNode deformer >"


class PsdItCmd : public MPxCommand
{
public:
                            PsdItCmd() {};
    virtual                 ~PsdItCmd() {};
    static      void*       creator();
    static      MSyntax     newSyntax();
    MStatus                 doIt( const MArgList& );
    MStatus                 undoIt();
    bool                    isUndoable() const;

private:
    MFnMesh                 origMesh;
    MFnMesh                 baseMesh;
    MFnMesh                 correctiveMesh;
    MFnMesh                 realMesh;
    MDGModifier             dgModifier;
    MStatus                 parseArgs( const MArgDatabase& );
    MStatus                 getBaseCorrective( MSelectionList& selectionList );

private:
    MString                 baseName;
    MString                 correctiveName;
    MString                 name;
    bool                    keepDeformer;

    void                    getObjectByName( const MString& name, MObject& object );
    void                    getDagPathByName( const MString& name, MDagPath& path );
    void                    getPoints( const MString& name, MPointArray& pointArray );
    void                    setPoints( const MString& name, MPointArray& pointArray );
};

#endif
