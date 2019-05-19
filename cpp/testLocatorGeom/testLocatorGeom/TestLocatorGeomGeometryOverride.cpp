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

TestLocatorGeomGeometryOverride::TestLocatorGeomGeometryOverride(const MObject& obj)
	: MHWRender::MPxGeometryOverride(obj)
	// TODO are these like default values??
	, mLocatorNode(obj)
	, mInput(0.0f)
	, mInputChanged(true)
{
}

TestLocatorGeomGeometryOverride::~TestLocatorGeomGeometryOverride()
{
}

MHWRender::DrawAPI TestLocatorGeomGeometryOverride::supportedDrawAPIs() const
{
	// this plugin supports both GL and DX
	return (MHWRender::kOpenGL | MHWRender::kDirectX11 | MHWRender::kOpenGLCoreProfile);
}

void TestLocatorGeomGeometryOverride::updateDG()
{
	MPlug plug(mLocatorNode, testLocatorGeom::input);
	float newInput = 1.0f;
	if (!plug.isNull())
	{
		newInput = plug.asFloat();
	}

	if (newInput != mInput)
	{
		mInput = newInput;
		mInputChanged = true;
	}
}


void TestLocatorGeomGeometryOverride::updateRenderItems(const MDagPath& path, MHWRender::MRenderItemList& list)
{
	MColor color = MHWRender::MGeometryUtilities::wireframeColor(path);
	MHWRender::MShaderInstance* shader = get3dSolidShader(color);
	if (!shader) return;

	unsigned int depthPriority;

	switch (MHWRender::MGeometryUtilities::displayStatus(path))
	{
		case MHWRender::kLead:
		case MHWRender::kActive:
		case MHWRender::kHilite:
		case MHWRender::kActiveComponent:
			depthPriority = MHWRender::MRenderItem::sActiveWireDepthPriority;
			break;
		default:
			depthPriority = MHWRender::MRenderItem::sDormantFilledDepthPriority;
			break;
	}

	MHWRender::MRenderItem* wireframeItem = NULL;

	int index = list.indexOf(wireframeItemName_);
	if (index < 0)
	{
		wireframeItem = MHWRender::MRenderItem::Create(
			wireframeItemName_,
			MHWRender::MRenderItem::DecorationItem,
			MHWRender::MGeometry::kLines);
		wireframeItem->setDrawMode(MHWRender::MGeometry::kWireframe);
		list.append(wireframeItem);
	}
	else
	{
		wireframeItem = list.itemAt(index);
	}

	if (wireframeItem)
	{
		wireframeItem->setShader(shader);
		wireframeItem->depthPriority(depthPriority);
		wireframeItem->enable(true);
	}

	MHWRender::MRenderItem* shadedItem = NULL;

	index = list.indexOf(shadedItemName_);
	if (index < 0)
	{
		shadedItem = MHWRender::MRenderItem::Create(
			shadedItemName_,
			MHWRender::MRenderItem::DecorationItem,
			MHWRender::MGeometry::kTriangles);
		shadedItem->setDrawMode((MHWRender::MGeometry::DrawMode)(MHWRender::MGeometry::kShaded | MHWRender::MGeometry::kTextured));
		list.append(shadedItem);
	}
	else
	{
		shadedItem = list.itemAt(index);
	}

	if (shadedItem)
	{
		shadedItem->setShader(shader);
		shadedItem->depthPriority(depthPriority);
		shadedItem->enable(true);
	}
}



void TestLocatorGeomGeometryOverride::populateGeometry(
	const MHWRender::MGeometryRequirements& requirements,
	const MHWRender::MRenderItemList& renderItems,
	MHWRender::MGeometry& data)
{
	MHWRender::MVertexBuffer* verticesBuffer = NULL;

	float* vertices = NULL;

	const MHWRender::MVertexBufferDescriptorList& vertexBufferDescriptorList = requirements.vertexRequirements();
	const int numberOfVertexRequirments = vertexBufferDescriptorList.length();

	MHWRender::MVertexBufferDescriptor vertexBufferDescriptor;
	for (int requirmentNumber = 0; requirmentNumber < numberOfVertexRequirments; ++requirmentNumber)
	{
		if (!vertexBufferDescriptorList.getDescriptor(requirmentNumber, vertexBufferDescriptor))
		{
			continue;
		}

		switch (vertexBufferDescriptor.semantic())
		{
			case MHWRender::MGeometry::kPosition:
			{
				if (!verticesBuffer)
				{
					verticesBuffer = data.createVertexBuffer(vertexBufferDescriptor);

					if (verticesBuffer)
					{
						//vertices = (float*)verticesBuffer->acquire(soleCount + heelCount);
						vertices = (float*)verticesBuffer->acquire(squareCount);
					}
				}
			}
			break;
			default:
				// do nothing for stuff we don't understand
				break;
		}
	}

	int verticesPointerOffset = 0;

	// We concatenate the heel and sole positions into a single vertex buffer.
	// The index buffers will decide which positions will be selected for each render items.
	/*for (int currentVertex = 0; currentVertex < soleCount + heelCount; ++currentVertex)
	{
		if (vertices)
		{
			if (currentVertex < heelCount)
			{
				int heelVtx = currentVertex;
				vertices[verticesPointerOffset++] = heel[heelVtx][0] * mInput;
				vertices[verticesPointerOffset++] = heel[heelVtx][1] * mInput;
				vertices[verticesPointerOffset++] = heel[heelVtx][2] * mInput;
			}
			else
			{
				int soleVtx = currentVertex - heelCount;
				vertices[verticesPointerOffset++] = sole[soleVtx][0] * mInput;
				vertices[verticesPointerOffset++] = sole[soleVtx][1] * mInput;
				vertices[verticesPointerOffset++] = sole[soleVtx][2] * mInput;
		}
	}
*/
	for (int currentVertex = 0; currentVertex < squareCount; ++currentVertex)
	{
		if (vertices)
		{
			//vertices[verticesPointerOffset++] = heel[currentVertex][0] * mInput;
			//vertices[verticesPointerOffset++] = heel[currentVertex][1] * mInput;
			//vertices[verticesPointerOffset++] = heel[currentVertex][2] * mInput;
			vertices[verticesPointerOffset++] = square[currentVertex][0] * mInput;
			vertices[verticesPointerOffset++] = square[currentVertex][1] * mInput;
			vertices[verticesPointerOffset++] = square[currentVertex][2] * mInput;
		}
	}

	if (verticesBuffer && vertices)
	{
		verticesBuffer->commit(vertices);
	}

	for (int i = 0; i < renderItems.length(); ++i)
	{
		const MHWRender::MRenderItem* item = renderItems.itemAt(i);

		if (!item)
		{
			continue;
		}

		MHWRender::MIndexBuffer* indexBuffer = data.createIndexBuffer(MHWRender::MGeometry::kUnsignedInt32);

		if (item->name() == wireframeItemName_)
		{
			int primitiveIndex = 0;
			//int startIndex = 0;
			//int numPrimitive = heelCount + soleCount - 2;
			int numPrimitive = squareCount;
			int numIndex = numPrimitive * 2;

			unsigned int* indices = (unsigned int*)indexBuffer->acquire(numIndex);

			//for (int i = 0; i < numIndex; )
			//{
			//	if (i < (heelCount - 1) * 2)
			//	{
			//		startIndex = 0;
			//		primitiveIndex = i / 2;
			//	}
			//	else
			//	{
			//		startIndex = heelCount;
			//		primitiveIndex = i / 2 - heelCount + 1;
			//	}
			//	indices[i++] = startIndex + primitiveIndex;
			//	indices[i++] = startIndex + primitiveIndex + 1;
			//}

			//for (int i = 0; i < numIndex; )
			//{
			//	primitiveIndex = i / 2;

			//	indices[i++] = primitiveIndex;
			//	indices[i++] = primitiveIndex + 1;
			//}

			// edges
			indices[0] = 0;
			indices[1] = 1;

			indices[2] = 1;
			indices[3] = 2;

			indices[4] = 2;
			indices[5] = 3;

			indices[6] = 3;
			indices[7] = 0;

			indexBuffer->commit(indices);
		}
		else if (item->name() == shadedItemName_)
		{
			int primitiveIndex = 0;
			//int startIndex = 0;
			//int numPrimitive = heelCount + soleCount - 4;
			int numPrimitive = squareCount - 2;
			int numIndex = numPrimitive * 3;

			unsigned int* indices = (unsigned int*)indexBuffer->acquire(numIndex);

			//for (int i = 0; i < numIndex; )
			//{
			//	if (i < (heelCount - 2) * 3)
			//	{
			//		startIndex = 0;
			//		primitiveIndex = i / 3;
			//	}
			//	else
			//	{
			//		startIndex = heelCount;
			//		primitiveIndex = i / 3 - heelCount + 2;
			//	}
			//	indices[i++] = startIndex;
			//	indices[i++] = startIndex + primitiveIndex + 1;
			//	indices[i++] = startIndex + primitiveIndex + 2;
			//}

			//for (int i = 0; i < numIndex; )
			//{
			//	primitiveIndex = i / 3;
			//	
			//	indices[i++] = 0;
			//	indices[i++] = primitiveIndex + 1;
			//	indices[i++] = primitiveIndex + 2;
			//}


			// faces
			indices[0] = 0;
			indices[1] = 1;
			indices[2] = 2;

			indices[3] = 2;
			indices[4] = 3;
			indices[5] = 0;

			indexBuffer->commit(indices);
		}

		item->associateWithIndexBuffer(indexBuffer);
	}

	mInputChanged = false;
}
