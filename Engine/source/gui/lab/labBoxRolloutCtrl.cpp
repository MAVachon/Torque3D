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
#include "labBoxRolloutCtrl.h"
#include "gui/core/guiDefaultControlRender.h"
#include "gui/containers/guiPanel.h"
#include "console/consoleTypes.h"
#include "console/engineAPI.h"
#include "gfx/gfxDrawUtil.h"
#include "gui/core/guiCanvas.h"
IMPLEMENT_CONOBJECT(LabBoxRolloutCtrl);

ConsoleDocClass(LabBoxRolloutCtrl,
	"@brief Enhanced version of GuiRollout which support docking and multiple childs.\n\n"

	"@tsexample\n"
	"// Comment:\n"
	"%okButton = new ClassObject()\n"
	"instantiation\n"
	"@endtsexample\n\n"

	"@ingroup GuiContainers"
	);


//-----------------------------------------------------------------------------


LabBoxRolloutCtrl::LabBoxRolloutCtrl()
{
	mStackChilds = true;
	mCollapsed = false;
	mCollapsedHeight = 0;
	mIsContainer = true;

	mOriginalExtents.set(10, 10);
}

//-----------------------------------------------------------------------------

LabBoxRolloutCtrl::~LabBoxRolloutCtrl()
{

}

//-----------------------------------------------------------------------------

void LabBoxRolloutCtrl::initPersistFields()
{
	addGroup("Window");
	addField("stackChilds", TypeBool, Offset(mStackChilds, LabBoxRolloutCtrl),
		"If true, the bitmap is tiled inside the control rather than stretched to fit.");
	addField("stackSpacing", TypeS32, Offset(mStackSpacing, LabBoxRolloutCtrl),
		"If true, the bitmap is tiled inside the control rather than stretched to fit.");
	addField("stackMargin", TypeRectSpacingI, Offset(mStackMargin, LabBoxRolloutCtrl));
	addField("minStackHeight", TypeS32, Offset(mMinStackHeight, LabBoxRolloutCtrl),
		"If true, the bitmap is tiled inside the control rather than stretched to fit.");
	endGroup("Window");
	addGroup("Rollout");
		addField("collapsedHeight", TypeS32, Offset(mCollapsedHeight, LabBoxRolloutCtrl),
		"If true, the bitmap is tiled inside the control rather than stretched to fit.");
	endGroup("Rollout");
	Parent::initPersistFields();
}
DefineEngineMethod(LabBoxRolloutCtrl, refresh, void, (), ,
	"Restack the child controls.\n")
{
	object->updatePanes();
}

//-----------------------------------------------------------------------------
bool LabBoxRolloutCtrl::onWake()
{
	if (!Parent::onWake())
		return false;

	if (!mProfile->mFont)
	{
		Con::errorf("GuiPaneControl::onWake - profile has no valid font");
		return false;
	}

	mProfile->constructBitmapArray();
	if (mProfile->mUseBitmapArray && mProfile->mBitmapArrayRects.size() > 6)
	{
		
		mThumbSize.set(mProfile->mBitmapArrayRects[0].extent.x, mProfile->mBitmapArrayRects[0].extent.y + mProfile->mBitmapArrayRects[6].extent.y + mCollapsedHeight);
		mThumbSize.setMax(mProfile->mBitmapArrayRects[1].extent);

		if (mProfile->mFont->getHeight() > mThumbSize.y)
			mThumbSize.y = mProfile->mFont->getHeight();
	}
	else
	{
		mThumbSize.set(20, 20);
	}

	return true;
}
void LabBoxRolloutCtrl::setCollapsed(bool isCollapsed)
{
	//Simply show/hide childs and call the stackVertical function
	// Get the child
	if (size() == 0 ) return;

	//GuiControl *gc = dynamic_cast<GuiControl*>(operator[](0));
	
	if (mCollapsed && !isCollapsed)
	{
		//resize(getPosition(), mOriginalExtents);
		mCollapsed = false;

	/*	for (S32 i = begin; i != end; i += step)
		{
			GuiControl * gc = dynamic_cast<GuiControl*>(at(i));
			if (gc)
				gc->setVisible(true);

		}
		

		stackVertical(true); */
	}
	else if (!mCollapsed && isCollapsed)
	{
		mCollapsed = true;

		/*mOriginalExtents = getExtent();

		
		resize(getPosition(), Point2I(getExtent().x, mThumbSize.y));
		for (S32 i = begin; i != end; i += step)
		{
			GuiControl * gc = dynamic_cast<GuiControl*>(at(i));
			if (gc)
				gc->setVisible(false);

		}
		//if (gc)
			//gc->setVisible(false);*/
	}
	// Stack from Child0 at top to ChildN at bottom
	S32 begin, end, step;
	begin = 0;
	end = size();
	step = 1;
	for (S32 i = begin; i != end; i += step)
	{
		GuiControl * gc = dynamic_cast<GuiControl*>(at(i));
		if (gc)
			gc->setVisible(!mCollapsed);

	}
	if (mCollapsed)
		resize(getPosition(), Point2I(getExtent().x, mThumbSize.y));
	else
		updatePanes();
		
	//stackVertical(true);
}

//-----------------------------------------------------------------------------
void LabBoxRolloutCtrl::onMouseDown(const GuiEvent &event)
{
	Point2I localClick = globalToLocalCoord(event.mousePoint);

	// If we're clicking in the header then resize
	if (localClick.y < mThumbSize.y)
	{
		mouseLock();	

		//update
		setUpdate();
	}
}

//-----------------------------------------------------------------------------

void LabBoxRolloutCtrl::onMouseUp(const GuiEvent &event)
{
	// Make sure we only get events we ought to be getting...
	if (!mActive)
		return;	

	mouseUnlock();
	setUpdate();

	Point2I localClick = globalToLocalCoord(event.mousePoint);

	// If we're clicking in the header then resize
	if (localClick.y < mThumbSize.y)
		setCollapsed(!mCollapsed);
}
bool LabBoxRolloutCtrl::resize(const Point2I &newPosition, const Point2I &newExtent)
{
	// CodeReview WTF is going on here that we need to bypass parent sanity?
	//  Investigate this [7/1/2007 justind]
	if (!Parent::resize(newPosition, newExtent))
		return false;
	return true;
	mOriginalExtents.x = getWidth();

	/*
	GuiControl *parent = getParent();
	if (parent)
	parent->childResized(this);
	setUpdate();
	*/
	S32 begin, end, step;
	// Resize the child control if we're not collapsed
	if (size() && !mCollapsed)
	{
		for (S32 i = begin; i != end; i += step)
		{
			GuiControl * gc = dynamic_cast<GuiControl*>(at(i));


			if (gc)
			{
				Point2I offset(0, mThumbSize.y);

				gc->resize(offset, newExtent - offset);
			}
		}
	}

	// For now.
	return true;
}

//-----------------------------------------------------------------------------