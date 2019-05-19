#ifndef _testLocatorNodeNode
#define _testLocatorNodeNode
//
// Copyright (C) BrenainnJordan
// 
// File: testLocatorNodeNode.h
//
// Dependency Graph Node: testLocatorNode
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

class testLocatorNode : public MPxLocatorNode
{
public:
	testLocatorNode();
	~testLocatorNode() override;

	static  void*		creator();
	static  MStatus		initialize();

	MStatus preEvaluation(const MDGContext& context, const MEvaluationNode& evaluationNode) override;

public:

	static  MObject		size;

	static	MTypeId		id;
	static	MString		drawDbClassification;
	static	MString		drawRegistrantId;
};

class TestLocatorData : public MUserData
{
public:
	TestLocatorData() : MUserData(false) {} // don't delete after draw
	~TestLocatorData() override {}

	MColor fColor;
	unsigned int fDepthPriority;
	float size;
	MPoint center_point;
	MVector aim_vector;
	MVector normal_vector;
	float width;
	float height;
};


class TestLocatorDrawOverride : public MHWRender::MPxDrawOverride
{
public:

	static MHWRender::MPxDrawOverride* Creator(const MObject& obj)
	{
		return new TestLocatorDrawOverride(obj);
	}

	~TestLocatorDrawOverride() override;

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

	TestLocatorDrawOverride(const MObject& obj);
	float getSize(const MDagPath& objPath) const; // TODO what's the const for?

};

#endif
