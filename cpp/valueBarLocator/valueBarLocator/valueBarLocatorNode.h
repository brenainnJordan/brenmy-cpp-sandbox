#ifndef _valueBarLocatorNode
#define _valueBarLocatorNode
//
// Copyright (C) BrenainnJordan
// 
// File: valueBarLocatorNode.h
//
// Dependency Graph Node: valueBarLocator
//
// Author: Maya Plug-in Wizard 2.0
//

#include <maya/MPxNode.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MTypeId.h> 
#include <maya/MPxLocatorNode.h>
#include <maya/MUserData.h>
#include <maya/MColor.h>
#include <maya/MPxDrawOverride.h>
#include <maya/MDrawRegistry.h>
#include <maya/MGlobal.h>
#include <maya/MEvaluationNode.h>

class valueBarLocator : public MPxLocatorNode
{
public:
						valueBarLocator();
	virtual				~valueBarLocator(); 
	static  void*		creator();
	static  MStatus		initialize();
	MStatus preEvaluation(const MDGContext& context, const MEvaluationNode& evaluationNode) override;

public:
	// user input attrs
	static  MObject		input;

	static	MTypeId		id;
	static	MString		drawDbClassification;
	static	MString		drawRegistrantId;
};


class ValueBarLocatorData : public MUserData
{
public:
	ValueBarLocatorData() : MUserData(false) {} // don't delete after draw
	~ValueBarLocatorData() override {}

	MColor fColor;
	
	// user plugs
	float input;

	// pre calculated values
	MPoint origin;
	MPoint center_point;
	MVector aim_vector;
	MVector normal_vector;
	float width;
	float height;
};


class ValueBarLocatorDrawOverride : public MHWRender::MPxDrawOverride
{
public:

	static MHWRender::MPxDrawOverride* Creator(const MObject& obj)
	{
		return new ValueBarLocatorDrawOverride(obj);
	}

	~ValueBarLocatorDrawOverride() override;

	MHWRender::DrawAPI supportedDrawAPIs() const override;

	MUserData* prepareForDraw(
		const MDagPath& objPath,
		const MDagPath& cameraPath,
		const MHWRender::MFrameContext& frameContext,
		MUserData* oldData) override;

	bool hasUIDrawables() const override { return true; }

	void addUIDrawables(
		const MDagPath& objPath,
		MHWRender::MUIDrawManager& drawManager,
		const MHWRender::MFrameContext& frameContext,
		const MUserData* data
	) override;

private:

	ValueBarLocatorDrawOverride(const MObject& obj); // TODO what does this line do??
	float getInput(const MDagPath& objPath) const; // TODO what's the const for?

};

#endif
