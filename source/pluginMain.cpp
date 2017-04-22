/*

Author  :   astips

Github  :   https://github.com/astips

*/

#include "psditNode.h"
#include "psditCmd.h"
#include <maya/MFnPlugin.h>

using namespace std;


MStatus initializePlugin(MObject obj)
{
    MStatus   status;
    MFnPlugin plugin( obj, "astips", "1.0", "Any" );

    status = plugin.registerNode( PSDIT_NODE, 
                                  PsdItNode::id, 
                                  PsdItNode::creator, 
                                  PsdItNode::initialize, 
                                  MPxNode::kDeformerNode );

    status = plugin.registerCommand( PSDIT_CMD, 
                                     PsdItCmd::creator, 
                                     PsdItCmd::newSyntax );

    return status;
}


MStatus uninitializePlugin(MObject obj)
{
    MStatus   status;
    MFnPlugin plugin(obj);
    status = plugin.deregisterNode( PsdItNode::id );
    status = plugin.deregisterCommand( PSDIT_CMD );
    return status;
}
