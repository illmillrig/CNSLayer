

#include "CNSLayer.h"
#include <maya/MFnPlugin.h>


MStatus initializePlugin(MObject obj) {
    MStatus status;
    MFnPlugin plugin( obj, "Travis Miller", "2016.5", "Any");

    status = plugin.registerNode("cnsLayer", CNSLayer::id, CNSLayer::creator, CNSLayer::initialize, MPxNode::kDependNode);
    if (!status) {
        status.perror("registerNode");
        return status;
    }

    return status;
}

MStatus uninitializePlugin(MObject obj) {
    MStatus status;
    MFnPlugin plugin( obj );

    status = plugin.deregisterNode(CNSLayer::id);
    if (!status) {
        status.perror("deregisterNode");
        return status;
    }

    return status;
}