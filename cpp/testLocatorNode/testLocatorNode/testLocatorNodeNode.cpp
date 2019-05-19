//
// Copyright (C) BrenainnJordan
// 
// File: testLocatorNodeNode.cpp
//
// Dependency Graph Node: testLocatorNode
//
// Author: Maya Plug-in Wizard 2.0
//
/*

Super simple example of how to create a custom locator and have input attributes update draw calls correctly.

*/

#include "testLocatorNodeNode.h"

#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>


MTypeId testLocatorNode::id( 0x00003 );
MString	testLocatorNode::drawDbClassification("drawdb/geometry/testLocatorNode");
MString	testLocatorNode::drawRegistrantId("TestLocatorNodePlugin");
MObject testLocatorNode::size;

testLocatorNode::testLocatorNode() {}
testLocatorNode::~testLocatorNode() {}

void* testLocatorNode::creator()
{
	return new testLocatorNode();
}

MStatus testLocatorNode::initialize()
{

	MFnNumericAttribute nAttr;
	MStatus				stat;

	size = nAttr.create( "size", "s", MFnNumericData::kFloat, 0.0 );
	// Attribute will be written to files when this type of node is stored
 	nAttr.setStorable(true);
	// Attribute is keyable and will show up in the channel box
 	nAttr.setKeyable(true);

	stat = addAttribute(size);
		if (!stat) { stat.perror("addAttribute"); return stat;}

	return MS::kSuccess;

}

// Called before this node is evaluated by Evaluation Manager

// ** super important!! **
// this is what causes our dirty size attr to trigger draw calls
// such as if the attr is animated or connected
// saves having to set "isAlwaysDirty" flag

// TODO compare how well this optimizes over isAlwaysDirty (if at all?)

MStatus testLocatorNode::preEvaluation(
	const MDGContext& context,
	const MEvaluationNode& evaluationNode)
{
	if (context.isNormal())
	{
		MStatus status;
		if (evaluationNode.dirtyPlugExists(size, &status) && status)
		{
			MHWRender::MRenderer::setGeometryDrawDirty(thisMObject());
		}
	}

	return MStatus::kSuccess;
}

TestLocatorDrawOverride::TestLocatorDrawOverride(const MObject& obj)
/*
By setting isAlwaysDirty to false in MPxDrawOverride constructor, the
draw override will be updated(via prepareForDraw()) only when the node
is marked dirty via DG evaluation or dirty propagation.Additional
callback is also added to explicitly mark the node as being dirty (via
MRenderer::setGeometryDrawDirty()) for certain circumstances.
*/
	: MHWRender::MPxDrawOverride(obj, NULL, false) // is this like an inheritance thing?? like calling super?
{
	// constructor
	// do nothing
}

TestLocatorDrawOverride::~TestLocatorDrawOverride()
{
	// destructor
	// do nothing
}


float TestLocatorDrawOverride::getSize(const MDagPath& objPath) const
{
	// Retrieve value of the size attribute from the node
	MStatus status;
	MObject node = objPath.node(&status);

	if (status)
	{
		MPlug plug(node, testLocatorNode::size);
		if (!plug.isNull())
		{
			return plug.asFloat();
		}
	}

	return 1.0f;
}

MHWRender::DrawAPI TestLocatorDrawOverride::supportedDrawAPIs() const
{
	// this plugin supports both GL and DX
	return (MHWRender::kOpenGL | MHWRender::kDirectX11 | MHWRender::kOpenGLCoreProfile);
}


// Called by Maya each time the object needs to be drawn.
MUserData* TestLocatorDrawOverride::prepareForDraw(
	const MDagPath& objPath,
	const MDagPath& cameraPath,
	const MHWRender::MFrameContext& frameContext,
	MUserData* oldData)
{
	// Any data needed from the Maya dependency graph must be retrieved and cached in this stage.
	// There is one cache data for each drawable instance, if it is not desirable to allow Maya to handle data
	// caching, simply return null in this method and ignore user data parameter in draw callback method.
	// e.g. in this sample, we compute and cache the data for usage later when we create the 
	// MUIDrawManager to draw footprint in method addUIDrawables().
	TestLocatorData* data = dynamic_cast<TestLocatorData*>(oldData);
	if (!data)
	{
		data = new TestLocatorData();
	}

	float size = getSize(objPath);

	data->size = size;
	data->center_point = MPoint(0.0, 1.0, 0.0) * size;
	data->aim_vector = MVector(0.0, 1.0, 0.0);
	data->normal_vector = MVector(0.0, 0.0, 1.0);
	data->width = size;
	data->height = size;

	data->fColor = MColor(1.0, 0.0, 0.0, 1.0);

	return data;
}


// addUIDrawables() provides access to the MUIDrawManager, which can be used
// to queue up operations for drawing simple UI elements such as lines, circles and
// text. To enable addUIDrawables(), override hasUIDrawables() and make it return true.
void TestLocatorDrawOverride::addUIDrawables(
	const MDagPath& objPath,
	MHWRender::MUIDrawManager& drawManager,
	const MHWRender::MFrameContext& frameContext,
	const MUserData* data
)
{
	// Get data cached by prepareForDraw() for each drawable instance
	// TODO is this casting to TestLocatorData class? if so why?
	TestLocatorData* pLocatorData = (TestLocatorData*)data;
	if (!pLocatorData)
	{
		return;
	}

	drawManager.beginDrawable();

	drawManager.setColor(pLocatorData->fColor);
	
	// draw something super simple
	drawManager.rect(
		pLocatorData->center_point,
		pLocatorData->aim_vector,
		pLocatorData->normal_vector,
		pLocatorData->width,
		pLocatorData->height,
		true // filled = True
	);

	drawManager.endDrawable();
}
