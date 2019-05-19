//
// Copyright (C) BrenainnJordan
// 
// File: valueBarLocatorNode.cpp
//
// Dependency Graph Node: valueBarLocator
//
// Author: Maya Plug-in Wizard 2.0
//

#include "valueBarLocatorNode.h"

#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>

#include <maya/MGlobal.h>

// You MUST change this to a unique value!!!  The id is a 32bit value used
// to identify this type of node in the binary file format.  
//
// #error change the following to a unique value and then erase this line 
MTypeId valueBarLocator::id( 0x00002 );

// draw plugin values
MString	valueBarLocator::drawDbClassification("drawdb/geometry/valueBarLocator");
MString	valueBarLocator::drawRegistrantId("valueBarLocatorPlugin");

// user input attributes
MObject valueBarLocator::input;        


valueBarLocator::valueBarLocator() {}
valueBarLocator::~valueBarLocator() {}

// creator
void* valueBarLocator::creator()
{
	return new valueBarLocator();
}


MStatus valueBarLocator::initialize()
//
//	Description:
//		This method is called to create and initialize all of the attributes
//      and attribute dependencies for this node type.  This is only called 
//		once when the node type is registered with Maya.
//
//	Return Values:
//		MS::kSuccess
//		MS::kFailure
//		
{

	MFnNumericAttribute nAttr;
	MStatus				stat;

	input = nAttr.create( "input", "in", MFnNumericData::kFloat, 0.0 );
	// Attribute will be written to files when this type of node is stored
 	nAttr.setStorable(true);
	// Attribute is keyable and will show up in the channel box
 	nAttr.setKeyable(true);


	// Add the attributes we have created to the node
	//
	stat = addAttribute( input );
		if (!stat) { stat.perror("addAttribute"); return stat;}


	return MS::kSuccess;

}


MStatus valueBarLocator::preEvaluation(
	const MDGContext& context,
	const MEvaluationNode& evaluationNode)
{
	// force redraw on dirty inputs
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



