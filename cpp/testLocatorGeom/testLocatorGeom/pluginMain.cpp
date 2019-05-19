//
// Copyright (C) 
// 
// File: pluginMain.cpp
//
// Author: Maya Plug-in Wizard 2.0
//

#include "testLocatorGeomNode.h"

#include <maya/MFnPlugin.h>

MStatus initializePlugin( MObject obj )
//
//	Description:
//		this method is called when the plug-in is loaded into Maya.  It 
//		registers all of the services that this plug-in provides with 
//		Maya.
//
//	Arguments:
//		obj - a handle to the plug-in object (use MFnPlugin to access it)
//
{ 
	MStatus   status;
	MFnPlugin plugin( obj, "", "2018", "Any");

	status = plugin.registerNode(
		"testLocatorGeom",
		testLocatorGeom::id,
		testLocatorGeom::creator,
		testLocatorGeom::initialize,
		MPxNode::kLocatorNode,
		&testLocatorGeom::drawDbClassification
	);

	if (!status) {
		status.perror("registerNode");
		return status;
	}

	status = MHWRender::MDrawRegistry::registerGeometryOverrideCreator(
		testLocatorGeom::drawDbClassification,
		testLocatorGeom::drawRegistrantId,
		TestLocatorGeomGeometryOverride::Creator
	);

	if (!status) {
		status.perror("registerDrawOverrideCreator");
		return status;
	}

	return status;
}

MStatus uninitializePlugin( MObject obj)
//
//	Description:
//		this method is called when the plug-in is unloaded from Maya. It 
//		deregisters all of the services that it was providing.
//
//	Arguments:
//		obj - a handle to the plug-in object (use MFnPlugin to access it)
//
{
	MStatus   status;
	MFnPlugin plugin( obj );

	status = MHWRender::MDrawRegistry::deregisterGeometryOverrideCreator(
		testLocatorGeom::drawDbClassification,
		testLocatorGeom::drawRegistrantId
	);

	if (!status) {
		status.perror("deregisterDrawOverrideCreator");
		return status;
	}

	status = releaseShaders();
	if (!status) {
		status.perror("releaseShaders");
		return status;
	}

	status = plugin.deregisterNode( testLocatorGeom::id );
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}

	return status;
}
