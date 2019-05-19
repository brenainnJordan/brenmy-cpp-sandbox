#ifndef _testLocatorGeomNode
#define _testLocatorGeomNode
//
// Copyright (C) 
// 
// File: testLocatorGeomNode.h
//
// Dependency Graph Node: testLocatorGeom
//
// Author: Maya Plug-in Wizard 2.0
//

#include <maya/MPxNode.h>
#include <maya/MPxLocatorNode.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MTypeId.h> 
#include <maya/MEvaluationNode.h> 

// Viewport 2.0 includes
#include <maya/MDrawRegistry.h>
#include <maya/MPxGeometryOverride.h>
#include <maya/MUserData.h>
#include <maya/MDrawContext.h>
#include <maya/MShaderManager.h>
#include <maya/MHWGeometry.h>
#include <maya/MHWGeometryUtilities.h>
#include <maya/MPointArray.h>

#include <unordered_map>
 
// not sure if this should be in the header file??!?
namespace
{
	// square
	float square[][3] = {
		{ -0.50f, -0.50f, 0.00f },
		{ -0.50f, 0.50f, 0.00f },
		{ 0.50f, 0.50f, 0.00f },
		{ 0.50f, -0.50f, 0.00f }
	};

	// Foot Data
	//
	float sole[][3] = {
		{ 0.00f, 0.0f, -0.70f },
		{ 0.04f, 0.0f, -0.69f },
		{ 0.09f, 0.0f, -0.65f },
		{ 0.13f, 0.0f, -0.61f },
		{ 0.16f, 0.0f, -0.54f },
		{ 0.17f, 0.0f, -0.46f },
		{ 0.17f, 0.0f, -0.35f },
		{ 0.16f, 0.0f, -0.25f },
		{ 0.15f, 0.0f, -0.14f },
		{ 0.13f, 0.0f,  0.00f },
		{ 0.00f, 0.0f,  0.00f },
		{ -0.13f, 0.0f,  0.00f },
		{ -0.15f, 0.0f, -0.14f },
		{ -0.16f, 0.0f, -0.25f },
		{ -0.17f, 0.0f, -0.35f },
		{ -0.17f, 0.0f, -0.46f },
		{ -0.16f, 0.0f, -0.54f },
		{ -0.13f, 0.0f, -0.61f },
		{ -0.09f, 0.0f, -0.65f },
		{ -0.04f, 0.0f, -0.69f },
		{ -0.00f, 0.0f, -0.70f }
	};

	float heel[][3] = {
		{ 0.00f, 0.0f,  0.06f },
		{ 0.13f, 0.0f,  0.06f },
		{ 0.14f, 0.0f,  0.15f },
		{ 0.14f, 0.0f,  0.21f },
		{ 0.13f, 0.0f,  0.25f },
		{ 0.11f, 0.0f,  0.28f },
		{ 0.09f, 0.0f,  0.29f },
		{ 0.04f, 0.0f,  0.30f },
		{ 0.00f, 0.0f,  0.30f },
		{ -0.04f, 0.0f,  0.30f },
		{ -0.09f, 0.0f,  0.29f },
		{ -0.11f, 0.0f,  0.28f },
		{ -0.13f, 0.0f,  0.25f },
		{ -0.14f, 0.0f,  0.21f },
		{ -0.14f, 0.0f,  0.15f },
		{ -0.13f, 0.0f,  0.06f },
		{ -0.00f, 0.0f,  0.06f }
	};

	int soleCount = 21;
	int heelCount = 17;
	int squareCount = 4;

	// Viewport 2.0 specific data
	//
	const MString colorParameterName_ = "solidColor";
	const MString wireframeItemName_ = "footPrintLocatorWires";
	const MString shadedItemName_ = "footPrintLocatorTriangles";

	// Maintain a mini cache for 3d solid shaders in order to reuse the shader
	// instance whenever possible. This can allow Viewport 2.0 optimization e.g.
	// the GPU instancing system and the consolidation system to be leveraged.
	//
	struct MColorHash
	{
		std::size_t operator()(const MColor& color) const
		{
			std::size_t seed = 0;
			CombineHashCode(seed, color.r);
			CombineHashCode(seed, color.g);
			CombineHashCode(seed, color.b);
			CombineHashCode(seed, color.a);
			return seed;
		}

		void CombineHashCode(std::size_t& seed, float v) const
		{
			std::hash<float> hasher;
			seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}
	};

	std::unordered_map<MColor, MHWRender::MShaderInstance*, MColorHash> the3dSolidShaders;

	MHWRender::MShaderInstance* get3dSolidShader(const MColor& color)
	{
		// Return the shader instance if exists.
		auto it = the3dSolidShaders.find(color);
		if (it != the3dSolidShaders.end())
		{
			return it->second;
		}

		MHWRender::MShaderInstance* shader = NULL;

		MHWRender::MRenderer* renderer = MHWRender::MRenderer::theRenderer();
		if (renderer)
		{
			const MHWRender::MShaderManager* shaderMgr = renderer->getShaderManager();
			if (shaderMgr)
			{
				shader = shaderMgr->getStockShader(MHWRender::MShaderManager::k3dSolidShader);
			}
		}

		if (shader)
		{
			float solidColor[] = { color.r, color.g, color.b, 1.0f };
			shader->setParameter(colorParameterName_, solidColor);

			the3dSolidShaders[color] = shader;
		}

		return shader;
	}

	MStatus releaseShaders()
	{
		MHWRender::MRenderer* renderer = MHWRender::MRenderer::theRenderer();
		if (renderer)
		{
			const MHWRender::MShaderManager* shaderMgr = renderer->getShaderManager();
			if (shaderMgr)
			{
				for (auto it = the3dSolidShaders.begin(); it != the3dSolidShaders.end(); it++)
				{
					shaderMgr->releaseShader(it->second);
				}

				the3dSolidShaders.clear();
				return MS::kSuccess;
			}
		}

		return MS::kFailure;
	}
} // anonymous namespace


class testLocatorGeom : public MPxLocatorNode
{
public:
						testLocatorGeom();
	virtual				~testLocatorGeom(); 

	//virtual MStatus		compute( const MPlug& plug, MDataBlock& data );

	//void draw(
	//	M3dView & view,
	//	const MDagPath & path,
	//	M3dView::DisplayStyle style,
	//	M3dView::DisplayStatus status
	//) override;

	//bool            isBounded() const override;
	//MBoundingBox    boundingBox() const override;

	//MSelectionMask getShapeSelectionMask() const override;

	MStatus preEvaluation(const MDGContext& context, const MEvaluationNode& evaluationNode) override;

	static  void*		creator();
	static  MStatus		initialize();

public:

	
	static  MObject input;
	static	MTypeId	id;
	static	MString	drawDbClassification;
	static	MString	drawRegistrantId;
};


class TestLocatorGeomGeometryOverride : public MHWRender::MPxGeometryOverride
{
public:
	static MHWRender::MPxGeometryOverride* Creator(const MObject& obj)
	{
		return new TestLocatorGeomGeometryOverride(obj);
	}

	~TestLocatorGeomGeometryOverride() override;

	MHWRender::DrawAPI supportedDrawAPIs() const override;

	bool hasUIDrawables() const override { return false; }

	void updateDG() override;

	bool isIndexingDirty(const MHWRender::MRenderItem &item) override
	{
		return false;
	}

	bool isStreamDirty(const MHWRender::MVertexBufferDescriptor &desc) override
	{
		return mInputChanged;
	}
	
	void updateRenderItems(const MDagPath &path, MHWRender::MRenderItemList& list) override;
	
	void populateGeometry(
		const MHWRender::MGeometryRequirements &requirements,
		const MHWRender::MRenderItemList &renderItems,
		MHWRender::MGeometry &data
	) override;

	void cleanUp() override
	{
	};

private:
	TestLocatorGeomGeometryOverride(const MObject& obj);

	MObject mLocatorNode;
	float mInput;
	bool mInputChanged;
};


#endif
