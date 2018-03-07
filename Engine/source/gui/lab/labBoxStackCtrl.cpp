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
#include "labBoxStackCtrl.h"
#include "gui/core/guiDefaultControlRender.h"
#include "gui/containers/guiPanel.h"
#include "console/consoleTypes.h"
#include "console/engineAPI.h"
#include "gfx/gfxDrawUtil.h"
#include "gui/core/guiCanvas.h"
IMPLEMENT_CONOBJECT(LabBoxStackCtrl);

ConsoleDocClass(LabBoxStackCtrl,
	"@brief Brief Desc.\n\n"

	"@tsexample\n"
	"// Comment:\n"
	"%okButton = new ClassObject()\n"
	"instantiation\n"
	"@endtsexample\n\n"

	"@ingroup GuiContainers"
	);

ImplementEnumType(BoxStackingType,
	"Stacking method used to position child controls.\n\n"
	"@ingroup GuiContainers")
{
	LabBoxStackCtrl::StackBoxVert, "Vertical", "Stack children vertically by setting their Y position"
},
{ LabBoxStackCtrl::StackBoxHoriz,"Horizontal", "Stack children horizontall by setting their X position" },
{ LabBoxStackCtrl::StackBoxDyn,"Dynamic", "Automatically switch between "
"Vertical and Horizontal stacking. Vertical stacking is chosen when the "
"stack control is taller than it is wide, horizontal stacking is chosen "
"when the stack control is wider than it is tall." }
	EndImplementEnumType;
//-----------------------------------------------------------------------------


	LabBoxStackCtrl::LabBoxStackCtrl()
	{		
		mStackingType = StackBoxVert;
		mStackMargin.set(0, 0, 0, 0);
		mStackSpacing = 0;
		mIsContainer = true;
		mStackChilds = false;
		mMinStackHeight = 12;
	}

	//-----------------------------------------------------------------------------

	LabBoxStackCtrl::~LabBoxStackCtrl()
	{

	}

	//-----------------------------------------------------------------------------

	void LabBoxStackCtrl::initPersistFields()
	{
		addGroup("Window");		
		addField("stackingType", TYPEID< StackingType >(), Offset(mStackingType, LabBoxStackCtrl),
			"Determines the method used to position the child controls.\n\n");
		addField("stackChilds", TypeBool, Offset(mStackChilds, LabBoxStackCtrl),
			"If true, the bitmap is tiled inside the control rather than stretched to fit.");
		addField("stackSpacing", TypeS32, Offset(mStackSpacing, LabBoxStackCtrl),
			"If true, the bitmap is tiled inside the control rather than stretched to fit.");
		addField("stackMargin", TypeRectSpacingI, Offset(mStackMargin, LabBoxStackCtrl));
		addField("minStackHeight", TypeS32, Offset(mMinStackHeight, LabBoxStackCtrl),
			"If true, the bitmap is tiled inside the control rather than stretched to fit.");		
		endGroup("Window");
		Parent::initPersistFields();
	}
	DefineEngineMethod(LabBoxStackCtrl, updateStack, void, (), ,
		"Restack the child controls.\n")
	{
		object->updatePanes();
	}

	//-----------------------------------------------------------------------------

	bool LabBoxStackCtrl::onWake()
	{
		if (!Parent::onWake())
			return false;

		if (mStackChilds)
			updatePanes();

		return true;
	}

	//-----------------------------------------------------------------------------

	void LabBoxStackCtrl::updatePanes()
	{
		// Prevent recursion
		if (mResizing || !mStackChilds)
			return;

		// Set Resizing.
		mResizing = true;

		Point2I extent = getExtent();		
		stackVertical(true);

		// Clear Sizing Flag.
		mResizing = false;
	}

	//-----------------------------------------------------------------------------

	void LabBoxStackCtrl::freeze(bool _shouldfreeze)
	{
		mResizing = _shouldfreeze;
	}

	//-----------------------------------------------------------------------------

	void LabBoxStackCtrl::stackVertical(bool fromTop)
	{
		if (empty() || !mStackChilds)
			return;

		S32 begin, end, step;
		if (fromTop)
		{
			// Stack from Child0 at top to ChildN at bottom
			begin = 0;
			end = size();
			step = 1;
		}
		else
		{
			// Stack from ChildN at top to Child0 at bottom
			begin = size() - 1;
			end = -1;
			step = -1;
		}

		// Place each visible child control
		S32 maxWidth = 0;
		Point2I curPos(0, 0);
		curPos = mPadding;
		S32 top, bottom, left, right;
		top = mSizingOptions.mInternalPadding.top;
		bottom = mSizingOptions.mInternalPadding.bottom;
		left = mSizingOptions.mInternalPadding.left;
		right = mSizingOptions.mInternalPadding.right;

		curPos.set(left, top);
		for (S32 i = begin; i != end; i += step)
		{
			GuiControl * gc = dynamic_cast<GuiControl*>(at(i));
			if (gc && gc->isVisible())
			{
				// Add padding between controls
				if (curPos.y > 0)
					curPos.y += mStackSpacing;

				Point2I childPos = curPos;
				//childPos.x = gc->getLeft();
				//if (!mChangeChildPosition)
				//childPos.x = gc->getLeft();

				Point2I childSize(gc->getExtent());
				//if (mChangeChildSizeToFit)
				childSize.x = getWidth() - (left + right);

				if (childSize.y < mMinStackHeight)
					childSize.y = mMinStackHeight;

				gc->resize(childPos, childSize);

				curPos.y += gc->getHeight();
				maxWidth = getMax(maxWidth, childPos.x + childSize.x + right); //Remove both size padding and add size
			}
		}
		//curPos.x += mPadding.x;
		//curPos.y += mPadding.y;
		//if (mDynamicSize)
		//{
		// Conform our size to the sum of the child sizes.
		Point2I newPos(getPosition());
		Point2I newSize(getWidth(), curPos.y + bottom);

		newSize.setMax(getMinExtent());

		// Grow the stack up instead of down?
		//if (mDynamicPos)
		//newPos.y -= (newSize.y - getHeight());

		resize(newPos, newSize);
		//}
	}
	void LabBoxStackCtrl::stackHorizontal(bool fromLeft)
	{
		if (empty())
			return;

		S32 begin, end, step;
		if (fromLeft)
		{
			// Stack from Child0 at left to ChildN at right
			begin = 0;
			end = size();
			step = 1;
		}
		else
		{
			// Stack from ChildN at left to Child0 at right
			begin = size() - 1;
			end = -1;
			step = -1;
		}

		// Place each visible child control
		S32 maxHeight = 0;
		Point2I curPos(0, 0);
		for (S32 i = begin; i != end; i += step)
		{
			GuiControl * gc = dynamic_cast<GuiControl*>(at(i));
			if (gc && gc->isVisible())
			{
				// Add padding between controls
				if (curPos.x > 0)
					curPos.x += mStackSpacing;

				Point2I childPos = curPos;
				//if (!mChangeChildPosition)
					//childPos.y = gc->getTop();

				Point2I childSize(gc->getExtent());
				//if (mChangeChildSizeToFit)
				childSize.y = getHeight();

				gc->resize(childPos, childSize);

				curPos.x += gc->getWidth();
				maxHeight = getMax(maxHeight, childPos.y + childSize.y);
			}
		}

		//if (mDynamicSize)
		//{
			// Conform our size to the sum of the child sizes.
			Point2I newPos(getPosition());
			//Point2I newSize(curPos.x, mDynamicNonStackExtent ? maxHeight : getHeight());
			Point2I newSize(curPos.x, getHeight());
			newSize.setMax(getMinExtent());

			// Grow the stack left instead of right?
			//if (mDynamicPos)
				//newPos.x -= (newSize.x - getWidth());

			resize(newPos, newSize);
		//}
	}
	//-----------------------------------------------------------------------------

	//-----------------------------------------------------------------------------

	bool LabBoxStackCtrl::resize(const Point2I &newPosition, const Point2I &newExtent)
	{
		if (!Parent::resize(newPosition, newExtent))
			return false;

		if (mStackChilds)
			updatePanes();

		// CodeReview This logic should be updated to correctly return true/false
		//  based on whether it sized it's children. [7/1/2007 justind]
		return true;
	}

	//-----------------------------------------------------------------------------

	void LabBoxStackCtrl::addObject(SimObject *obj)
	{
		Parent::addObject(obj);

		updatePanes();
	}

	//-----------------------------------------------------------------------------

	void LabBoxStackCtrl::removeObject(SimObject *obj)
	{
		Parent::removeObject(obj);

		updatePanes();
	}

	//-----------------------------------------------------------------------------

	bool LabBoxStackCtrl::reOrder(SimObject* obj, SimObject* target)
	{
		bool ret = Parent::reOrder(obj, target);
		if (ret)
			updatePanes();

		return ret;
	}

	//-----------------------------------------------------------------------------

	void LabBoxStackCtrl::childResized(GuiControl *child)
	{
		updatePanes();
	}
	
	//-----------------------------------------------------------------------------

	
