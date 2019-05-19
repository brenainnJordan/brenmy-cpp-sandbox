//
// Copyright (C) 
// 
// File: testLocatorGeomNode.cpp
//
// Dependency Graph Node: testLocatorGeom
//
// Author: Maya Plug-in Wizard 2.0
//

#include "testLocatorGeomNode.h"

#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>

#include <maya/MGlobal.h>




MTypeId testLocatorGeom::id( 0x00003);
MObject testLocatorGeom::input;        

MString	testLocatorGeom::drawDbClassification("drawdb/geometry/light/testLocatorGeom");

//static bool sMakeFootPrintDirLight = (getenv("MAYA_FOOTPRINT_GEOMETRY_OVERRIDE_AS_DIRLIGHT") != NULL);

//static MString lightClassification("light:drawdb/geometry/light/testLocatorGeom:drawdb/light/directionalLight");

MString	testLocatorGeom::drawRegistrantId("testLocatorGeomPlugin");

//static bool sUseLegacyDraw = (getenv("MAYA_ENABLE_VP2_PLUGIN_LOCATOR_LEGACY_DRAW") != NULL);

testLocatorGeom::testLocatorGeom() {}
testLocatorGeom::~testLocatorGeom() {}


void* testLocatorGeom::creator()
{
	return new testLocatorGeom();
}

MStatus testLocatorGeom::initialize()
{

	MFnNumericAttribute nAttr;
	MStatus				stat;

	input = nAttr.create( "input", "in", MFnNumericData::kFloat, 0.0 );
 	nAttr.setStorable(true);
 	nAttr.setKeyable(true);

	stat = addAttribute( input );
		if (!stat) { stat.perror("addAttribute"); return stat;}

	return MS::kSuccess;

}


MStatus testLocatorGeom::preEvaluation(
	const MDGContext& context,
	const MEvaluationNode& evaluationNode)
{
	if (context.isNormal())
	{
		MStatus status;
		if (evaluationNode.dirtyPlugExists(input, &status) && status)
		{
			MHWRender::MRenderer::setGeometryDrawDirty(thisMObject());
		}
	}

	return MStatus::kSuccess;
}
