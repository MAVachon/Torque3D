//-----------------------------------------------------------------------------
// Copyright (c) 2012 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "gui/lab/world/terrainLabEditor.h"

#include "core/frameAllocator.h"
#include "core/strings/stringUnit.h"
#include "console/consoleTypes.h"
#include "console/simEvents.h"
#include "console/engineAPI.h"
#include "sim/netConnection.h"
#include "math/mathUtils.h"
#include "gfx/primBuilder.h"
#include "gfx/gfxDrawUtil.h"
#include "gui/core/guiCanvas.h"
#include "gui/lab/world/terrainLabActions.h"
#include "gui/worldEditor/terrainActions.h"
#include "gui/worldEditor/terrainEditor.h"
#include "terrain/terrMaterial.h"



IMPLEMENT_CONOBJECT(TerrainLabEditor);

ConsoleDocClass(TerrainLabEditor,
	"@brief The base Terrain Editor tool\n\n"
	"Editor use only.\n\n"
	"@internal"
	);

TerrainLabEditor::TerrainLabEditor()
{
	VECTOR_SET_ASSOCIATION(mActions);

	//
	resetCurrentSel();

	//
	mBrushPressure = 1.0f;
	mBrushSize.set(1, 1);
	mBrushSoftness = 1.0f;
	mBrushChanged = true;
	mMouseBrush = new BoxBrush(this);
	mMouseDownSeq = 0;
	mIsDirty = false;
	mIsMissionDirty = false;
	mPaintIndex = -1;

	
}

TerrainLabEditor::~TerrainLabEditor()
{
	// mouse
	delete mMouseBrush;

	// terrain actions
	U32 i;
	for (i = 0; i < mActions.size(); i++)
		delete mActions[i];

	// undo stuff
	delete mUndoSel;
}
void TerrainLabEditor::autoMaterialLayer(F32 mMinHeight, F32 mMaxHeight, F32 mMinSlope, F32 mMaxSlope, F32 mCoverage)
{
	if (!mActiveTerrain)
		return;

	S32 mat = getPaintMaterialIndex();
	if (mat == -1)
		return;

	mUndoSel = new Selection;

	U32 terrBlocks = mActiveTerrain->getBlockSize();
	for (U32 y = 0; y < terrBlocks; y++)
	{
		for (U32 x = 0; x < terrBlocks; x++)
		{
			// get info  
			GridPoint gp;
			gp.terrainBlock = mActiveTerrain;
			gp.gridPos.set(x, y);

			GridInfo gi;
			getGridInfo(gp, gi);

			if (gi.mMaterial == mat)
				continue;

			if (mRandI(0, 100) > mCoverage)
				continue;

			Point3F wp;
			gridToWorld(gp, wp);

			if (!(wp.z >= mMinHeight && wp.z <= mMaxHeight))
				continue;

			// transform wp to object space  
			Point3F op;
			mActiveTerrain->getWorldTransform().mulP(wp, &op);

			Point3F norm;
			mActiveTerrain->getNormal(Point2F(op.x, op.y), &norm, true);

			if (mMinSlope > 0)
				if (norm.z > mSin(mDegToRad(90.0f - mMinSlope)))
					continue;

			if (mMaxSlope < 90)
				if (norm.z < mSin(mDegToRad(90.0f - mMaxSlope)))
					continue;

			gi.mMaterialChanged = true;
			mUndoSel->add(gi);
			gi.mMaterial = mat;
			setGridInfo(gi);
		}
	}

	if (mUndoSel->size())
		submitUndo(mUndoSel);
	else
		delete mUndoSel;

	mUndoSel = 0;

	scheduleMaterialUpdate();
}

DefineEngineMethod(TerrainLabEditor, autoMaterialLayer, void, (F32 minHeight, F32 maxHeight, F32 minSlope, F32 maxSlope, F32 coverage), ,
	"Rule based terrain painting.\n"
	"@param minHeight Minimum terrain height."
	"@param maxHeight Maximum terrain height."
	"@param minSlope Minimum terrain slope."
	"@param maxSlope Maximum terrain slope."
	"@param coverage Terrain coverage amount.")
{
	object->autoMaterialLayer(minHeight, maxHeight, minSlope, maxSlope, coverage);
}