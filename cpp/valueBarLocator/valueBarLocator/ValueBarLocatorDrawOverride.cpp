//
// Copyright (C) BrenainnJordan
// 
// File: ValueBarLocatorDrawOverride.cpp
//
// Dependency Graph Node: valueBarLocator
//
// Author: Maya Plug-in Wizard 2.0
//

#include "valueBarLocatorNode.h"


ValueBarLocatorDrawOverride::ValueBarLocatorDrawOverride(const MObject& obj)
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

ValueBarLocatorDrawOverride::~ValueBarLocatorDrawOverride()
{
	// destructor
	// do nothing
}


float ValueBarLocatorDrawOverride::getInput(const MDagPath& objPath) const
{
	// Retrieve value of the size attribute from the node
	MStatus status;
	MObject node = objPath.node(&status);

	if (status)
	{
		MPlug plug(node, valueBarLocator::input);
		if (!plug.isNull())
		{
			return plug.asFloat();
		}
	}

	return 1.0f;
}

MHWRender::DrawAPI ValueBarLocatorDrawOverride::supportedDrawAPIs() const
{
	// this plugin supports both GL and DX
	return (MHWRender::kOpenGL | MHWRender::kDirectX11 | MHWRender::kOpenGLCoreProfile);
}


MUserData* ValueBarLocatorDrawOverride::prepareForDraw(
	const MDagPath& objPath,
	const MDagPath& cameraPath,
	const MHWRender::MFrameContext& frameContext,
	MUserData* oldData)
{

	// Cache user attr values and precalculate draw values

	ValueBarLocatorData* data = dynamic_cast<ValueBarLocatorData*>(oldData);
	if (!data)
	{
		data = new ValueBarLocatorData();
	}

	float input = getInput(objPath);

	data->input = input;
	data->origin = MPoint(0.0, 0.0, 0.0);
	data->center_point = MPoint(0.0, 1.0, 0.0) * input;
	//data->aim_vector = MVector(0.0, 1.0, 0.0);
	//data->normal_vector = MVector(0.0, 0.0, 1.0);
	//data->width = input;
	//data->height = input;

	data->fColor = MColor(1.0, 0.0, 0.0, 1.0);

	return data;
}


void ValueBarLocatorDrawOverride::addUIDrawables(
	const MDagPath& objPath,
	MHWRender::MUIDrawManager& drawManager,
	const MHWRender::MFrameContext& frameContext,
	const MUserData* data
)
{
	// Get data cached by prepareForDraw() for each drawable instance
	// and draw some stuff

	ValueBarLocatorData* cache_data = (ValueBarLocatorData*)data;
	if (!cache_data)
	{
		return;
	}

	drawManager.beginDrawable();

	drawManager.setColor(cache_data->fColor);

	// draw something super simple
	//drawManager.rect(
	//	cache_data->center_point,
	//	cache_data->aim_vector,
	//	cache_data->normal_vector,
	//	cache_data->width,
	//	cache_data->height,
	//	true // filled = True
	//);
	drawManager.line(
		cache_data->center_point,
		cache_data->origin
	);

	drawManager.endDrawable();
}
