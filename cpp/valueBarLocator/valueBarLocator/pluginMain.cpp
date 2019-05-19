//
// Copyright (C) BrenainnJordan
// 
// File: pluginMain.cpp
//
// Author: Maya Plug-in Wizard 2.0
//

#include "valueBarLocatorNode.h"

#include <maya/MFnPlugin.h>

MStatus initializePlugin( MObject obj )
{ 
	MStatus   status;
	MFnPlugin plugin( obj, "BrenainnJordan", "2018", "Any");

	status = plugin.registerNode(
		"valueBarLocator",
		valueBarLocator::id,
		valueBarLocator::creator,
		valueBarLocator::initialize,
		MPxNode::kLocatorNode,
		&valueBarLocator::drawDbClassification
	);

	if (!status) {
		status.perror("registerNode");
		return status;
	}

	status = MDrawRegistry::registerDrawOverrideCreator(
		valueBarLocator::drawDbClassification,
		valueBarLocator::drawRegistrantId,
		ValueBarLocatorDrawOverride::Creator
	);

	if (!status) {
		status.perror("registerDrawOverrideCreator");
		return status;
	}

	return status;
}

MStatus uninitializePlugin( MObject obj)
{
	MStatus   status;
	MFnPlugin plugin( obj );

	status = plugin.deregisterNode( valueBarLocator::id );

	if (!status) {
		status.perror("deregisterNode");
		return status;
	}

	status = MDrawRegistry::deregisterDrawOverrideCreator(
		valueBarLocator::drawDbClassification,
		valueBarLocator::drawRegistrantId
	);
	
	if (!status) {
		status.perror("deregisterDrawOverrideCreator");
		return status;
	}

	return status;
}
