//
// Copyright (C) BrenainnJordan
// 
// File: pluginMain.cpp
//
// Author: Maya Plug-in Wizard 2.0
//

#include "testLocatorNodeNode.h"

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
	MFnPlugin plugin( obj, "BrenainnJordan", "2018", "Any");

	status = plugin.registerNode(
		"testLocatorNode",
		testLocatorNode::id,
		testLocatorNode::creator,
		testLocatorNode::initialize,
		// IMPORTANT: to draw stuff we need to add the following flags
		// in addition to the usual node registering stuff.
		MPxNode::kLocatorNode,
		&testLocatorNode::drawDbClassification // TODO learn more about & (bitwise??)
	);

	if (!status) {
		status.perror("registerNode");
		return status;
	}

	status = MDrawRegistry::registerDrawOverrideCreator(
		testLocatorNode::drawDbClassification,
		testLocatorNode::drawRegistrantId,
		TestLocatorDrawOverride::Creator
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

	status = plugin.deregisterNode( testLocatorNode::id );
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}

	status = MDrawRegistry::deregisterDrawOverrideCreator(
		testLocatorNode::drawDbClassification,
		testLocatorNode::drawRegistrantId);
	if (!status) {
		status.perror("deregisterDrawOverrideCreator");
		return status;
	}

	return status;
}
