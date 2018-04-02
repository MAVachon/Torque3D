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
#include "labBoxCtrl.h"
#include "gui/core/guiDefaultControlRender.h"
#include "gui/containers/guiPanel.h"
#include "console/consoleTypes.h"
#include "console/engineAPI.h"
#include "gfx/gfxDrawUtil.h"
#include "gui/core/guiCanvas.h"

IMPLEMENT_CONOBJECT(LabBoxCtrl);

ConsoleDocClass(LabBoxCtrl,
	"@brief Brief Desc.\n\n"

	"@tsexample\n"
	"// Comment:\n"
	"%okButton = new ClassObject()\n"
	"instantiation\n"
	"@endtsexample\n\n"

	"@ingroup GuiContainers"
	);


IMPLEMENT_CALLBACK(LabBoxCtrl, onResize, void, (), (),
	"Called when the control is resized.");
IMPLEMENT_CALLBACK(LabBoxCtrl, onDropped, void, (), (),
	"Called when the window is collapsed by clicking its title bar.");
LabBoxCtrl::LabBoxCtrl()
{
	mResizing = false;
	mText = "";
	mPadding.set(4, 4);	
	mIsContainer = true;
	mCanMove = false;
	mMouseMovingWin = false;
}

//-----------------------------------------------------------------------------

LabBoxCtrl::~LabBoxCtrl()
{

}

//-----------------------------------------------------------------------------

void LabBoxCtrl::initPersistFields()
{
	addGroup("Window");

	addField("text", TypeRealString, Offset(mText, LabBoxCtrl),
		"Text label to display in titlebar.");
	addField("canMove", TypeBool, Offset(mCanMove, LabBoxCtrl),
		"Whether the window can be moved by dragging its titlebar.");

	endGroup("Window");
	Parent::initPersistFields();
}

//-----------------------------------------------------------------------------
bool LabBoxCtrl::onWake()
{
	if (!Parent::onWake())
		return false;

	return true;
}

void LabBoxCtrl::freeze(bool _shouldfreeze)
{
	mResizing = _shouldfreeze;
}


//-----------------------------------------------------------------------------

bool LabBoxCtrl::resize(const Point2I &newPosition, const Point2I &newExtent)
{
	if (!Parent::resize(newPosition, newExtent))
		return false;

	onResize_callback();
	//if (mStackChilds)
		//updatePanes();

	// CodeReview This logic should be updated to correctly return true/false
	//  based on whether it sized it's children. [7/1/2007 justind]
	return true;
}


//-----------------------------------------------------------------------------

void LabBoxCtrl::parentResized(const RectI &oldParentRect, const RectI &newParentRect)
{
	//if(!mCanResize)
	//	return;

	// If it's a control that specifies invalid docking, we'll just treat it as an old GuiControl
	if (getDocking() & Docking::dockInvalid || getDocking() & Docking::dockNone)
		return Parent::parentResized(oldParentRect, newParentRect);

	S32 deltaX = newParentRect.extent.x - oldParentRect.extent.x;
	S32 deltaY = newParentRect.extent.y - oldParentRect.extent.y;

	// Update Self
	RectI oldThisRect = getBounds();
	anchorControl(this, Point2I(deltaX, deltaY));
	RectI newThisRect = getBounds();

	// Update Deltas to pass on to children
	deltaX = newThisRect.extent.x - oldThisRect.extent.x;
	deltaY = newThisRect.extent.y - oldThisRect.extent.y;

	// Iterate over all children and update their anchors
	iterator nI = begin();
	for (; nI != end(); nI++)
	{
		// Sanity
		GuiControl *control = dynamic_cast<GuiControl*>((*nI));
		if (control)
			control->parentResized(oldThisRect, newThisRect);
	}
}

//-----------------------------------------------------------------------------
void LabBoxCtrl::onMouseDown(const GuiEvent &event)
{
	setUpdate();

	mOrigBounds = getBounds();

	mMouseDownPosition = event.mousePoint;
	Point2I localPoint = globalToLocalCoord(event.mousePoint);

	mMouseMovingWin = false;

	S32 docking = getDocking();
	if (docking == Docking::dockInvalid || docking == Docking::dockNone)
		mMouseMovingWin = mCanMove;

	if (mMouseMovingWin)
	{
		mouseLock();
	}
	else
	{
		GuiControl *ctrl = findHitControl(localPoint);
		if (ctrl && ctrl != this)
			ctrl->onMouseDown(event);
	}
	execConsoleCallback();
}

//-----------------------------------------------------------------------------

void LabBoxCtrl::onMouseUp(const GuiEvent &event)
{
	TORQUE_UNUSED(event);
	mouseUnlock();
	if (mMouseMovingWin)
		onDropped_callback();
	mMouseMovingWin = false;


}

//-----------------------------------------------------------------------------

void LabBoxCtrl::onMouseDragged(const GuiEvent &event)
{
	GuiControl *parent = getParent();
	GuiCanvas *root = getRoot();
	if (!root)
		return;

	mMousePosition = globalToLocalCoord(event.mousePoint);
	Point2I deltaMousePosition = event.mousePoint - mMouseDownPosition;

	Point2I newPosition = getPosition();
	Point2I newExtent = getExtent();
	bool resizeX = false;
	bool resizeY = false;

	if (mMouseMovingWin && parent)
	{
		if (parent != root)
		{
			newPosition.x = mOrigBounds.point.x + deltaMousePosition.x;
			newPosition.y = getMax(0, mOrigBounds.point.y + deltaMousePosition.y);
		}
		else
		{
			newPosition.x = getMax(0, getMin(parent->getWidth() - getWidth(), mOrigBounds.point.x + deltaMousePosition.x));
			newPosition.y = getMax(0, getMin(parent->getHeight() - getHeight(), mOrigBounds.point.y + deltaMousePosition.y));
		}

		newPosition.x = mClamp(newPosition.x, 0, parent->getWidth() - getWidth());
		newPosition.y = mClamp(newPosition.y, 0, parent->getHeight() - getHeight());		
	}

	// Resize this
	Point2I pos = parent->localToGlobalCoord(getPosition());
	root->addUpdateRegion(pos, getExtent());
	resize(newPosition, newExtent);
}

//-----------------------------------------------------------------------------

void LabBoxCtrl::onPreRender()
{
	Parent::onPreRender();
}

//-----------------------------------------------------------------------------

const RectI LabBoxCtrl::getClientRect()
{
	return Parent::getClientRect();
}

//-----------------------------------------------------------------------------

void LabBoxCtrl::onRender(Point2I offset, const RectI &updateRect)
{	
	RectI ctrlRect(offset, getExtent());

	//---------------------------------------------------------------
	//Render the Area - Start with Fill then check for bitmapArray with renderBox

	//if opaque, fill the update rect with the fill color
	if (mProfile->mOpaque)
		GFX->getDrawUtil()->drawRectFill(ctrlRect, mProfile->mFillColor);

	//if there's a border, draw the border
	if (!mProfile->mUseBitmapArray){
		if (mProfile->mBorder)
		{
			RectI rect(offset.x, offset.y, getExtent().x, getExtent().y);
			GFX->getDrawUtil()->drawRect(rect, mProfile->mBorderColor);
		}
	}
	else
		renderBox(ctrlRect, mProfile);

	//---------------------------------------------------------------
	//Render the Title Text
	S32 textWidth = mProfile->mFont->getStrWidth((const UTF8 *)mText);
	Point2I start(0, 0);

	// Align the horizontal
	if (mProfile->mAlignment == GuiControlProfile::RightJustify)
		start.set(updateRect.extent.x - textWidth, 0);
	else if (mProfile->mAlignment == GuiControlProfile::CenterJustify)
		start.set((updateRect.extent.x - textWidth) / 2, 0);
	else // GuiControlProfile::LeftJustify or garbage... ;)
		start.set(0, 0);
	// If the text is longer then the box size, (it'll get clipped) so force Left Justify
	if (textWidth > updateRect.extent.x) start.set(0, 0);


	GFX->getDrawUtil()->drawText(mProfile->mFont, start + offset + mProfile->mTextOffset, mText);
	//---------------------------------------------------------------

	// Render Children
	renderChildControls(offset, updateRect);
}
DefineEngineMethod(LabBoxCtrl, freeze, void, (bool freeze), ,
	"Prevents control from restacking - useful when adding or removing child controls\n"
	"@param freeze True to freeze the control, false to unfreeze it\n\n"
	"@tsexample\n"
	"%stackCtrl.freeze(true);\n"
	"// add controls to stack\n"
	"%stackCtrl.freeze(false);\n"
	"@endtsexample\n")
{
	object->freeze(freeze);
}