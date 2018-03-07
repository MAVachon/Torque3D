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
#include "gui/lab/labHudCtrl.h"

#include "console/consoleTypes.h"
#include "console/engineAPI.h"
#include "T3D/gameBase/gameConnection.h"
#include "gui/core/guiCanvas.h"
#include "gfx/primBuilder.h"
#include "gfx/gfxDrawUtil.h"
#include "gfx/gfxTransformSaver.h"
#include "gfx/gfxDebugEvent.h"
#include "scene/sceneManager.h"
#include "scene/sceneRenderState.h"
#include "renderInstance/renderBinManager.h"


IMPLEMENT_CONOBJECT(LabHudCtrl);
ConsoleDocClass( LabHudCtrl,
   "@brief Work as a GameTSCtrl child overlay, it will call mouse action on the parent GameTSCtrl.\n\n"

   "By using it, the GuiControl between them won't interfer with mouse calls\n\n"

   "@ingroup Game\n"

   "@internal"
);



LabHudCtrl::LabHudCtrl()
{


  
   mLastMousePos.set(0, 0);

   mLeftMouseDown = false;
   mRightMouseDown = false;
   mMiddleMouseDown = false;
   mMiddleMouseTriggered = false;
   mMouseLeft = false;
	mDragRectColor.set(255, 255, 0);


}

LabHudCtrl::~LabHudCtrl()
{

}

//------------------------------------------------------------------------------



void LabHudCtrl::onRender(Point2I offset, const RectI &updateRect)
{
  
   Parent::onRender(offset, updateRect);
	if (mDragSelect && mDragRect.extent.x > 1 && mDragRect.extent.y > 1)
		GFX->getDrawUtil()->drawRect(mDragRect, mDragRectColor);
}

//------------------------------------------------------------------------------

void LabHudCtrl::initPersistFields()
{
  
   
   Parent::initPersistFields();
}


//------------------------------------------------------------------------------
void LabHudCtrl::makeScriptCall(const char *func, const GuiEvent &evt) const
{
	// write screen position
	char *sp = Con::getArgBuffer(32);
	dSprintf(sp, 32, "%d %d", evt.mousePoint.x, evt.mousePoint.y);

	// write world position
	char *wp = Con::getArgBuffer(32);
	Point3F camPos;
	mLastCameraQuery.cameraMatrix.getColumn(3, &camPos);
	dSprintf(wp, 32, "%g %g %g", camPos.x, camPos.y, camPos.z);

	// write click vector
	char *vec = Con::getArgBuffer(32);
	Point3F fp(evt.mousePoint.x, evt.mousePoint.y, 1.0);
	Point3F ray;
	unproject(fp, &ray);
	ray -= camPos;
	ray.normalizeSafe();
	dSprintf(vec, 32, "%g %g %g", ray.x, ray.y, ray.z);

	Con::executef((SimObject*)this, func, sp, wp, vec);
}
void LabHudCtrl::onRightMouseDown(const GuiEvent &event)
{
	//If the parentGroup have a method call it (Used for parent GameTSControl)
	GuiControl *parent = getParent();
	if (parent && parent->isMethod("onRightMouseDown"))
	{
		parent->onRightMouseDown(event);
		return;
	}
	Parent::onRightMouseDown(event);
	if (isMethod("onRightMouseDown"))
		makeScriptCall("onRightMouseDown", event);
}



void LabHudCtrl::onRightMouseUp(const GuiEvent &event)
{
	//If the parentGroup have a method call it (Used for parent GameTSControl)
	GuiControl *parent = getParent();
	if (parent && parent->isMethod("onRightMouseUp"))
	{
		parent->onRightMouseUp(event);
		return;
	}
	Parent::onRightMouseUp(event);
	if (isMethod("onRightMouseUp"))
		makeScriptCall("onRightMouseUp", event);
}
void LabHudCtrl::onMiddleMouseDown(const GuiEvent &event)
{
	//If the parentGroup have a method call it (Used for parent GameTSControl)
	GuiControl *parent = getParent();
	if (parent && parent->isMethod("onMiddleMouseDown"))
	{
		parent->onMiddleMouseDown(event);
		return;
	}
	Parent::onMiddleMouseDown(event);
	if (isMethod("onMiddleMouseDown"))
		makeScriptCall("onMiddleMouseDown", event);
}
void LabHudCtrl::onMiddleMouseUp(const GuiEvent &event)
{
	//If the parentGroup have a method call it (Used for parent GameTSControl)
	GuiControl *parent = getParent();
	if (parent && parent->isMethod("onMiddleMouseUp"))
	{
		parent->onMiddleMouseUp(event);
		return;
	}
	Parent::onMiddleMouseUp(event);
	if (isMethod("onMiddleMouseUp"))
		makeScriptCall("onMiddleMouseUp", event);
}
void LabHudCtrl::onMouseUp(const GuiEvent & event)
{
	//If the parentGroup have a method call it (Used for parent GameTSControl)
	GuiControl *parent = getParent();
	if (parent && parent->isMethod("onMouseUp"))
	{
		parent->onMouseUp(event);
		return;
	}

	Parent::onMouseUp(event);
	if (isMethod("onMouseUp"))
		makeScriptCall("onMouseUp", event);

	mLeftMouseDown = false;

	if (mDragSelect)
	{
		if (isMethod("onDragRectDone"))
			Con::executef(this, "onDragRectDone", mDragRect.point, mDragRect.extent);
		mDragSelect = false;

	}

}

void LabHudCtrl::onMouseDown(const GuiEvent & event)
{
	//If the parentGroup have a method call it (Used for parent GameTSControl)
	GuiControl *parent = getParent();	
	if (parent && parent->isMethod("onMouseDown"))
	{
		parent->onMouseDown(event);
		return;
	}
		
	
	Parent::onMouseDown(event);
	if (isMethod("onMouseDown"))
		makeScriptCall("onMouseDown", event);

	mLeftMouseDown = true;

	mDragSelect = true;
	mDragRect.set(Point2I(event.mousePoint), Point2I(0, 0));
	mDragStart = event.mousePoint;

	setFirstResponder();
}

void LabHudCtrl::onMouseMove(const GuiEvent & event)
{
	//If the parentGroup have a method call it (Used for parent GameTSControl)
	GuiControl *parent = getParent();
	if (parent && parent->isMethod("onMouseMove"))
	{
		parent->onMouseMove(event);
		//return;
	}

	if (mDragSelect)	
		mDragSelect = false;

	mLastMousePos = event.mousePoint;
}

void LabHudCtrl::onMouseDragged(const GuiEvent & event)
{
	//If the parentGroup have a method call it (Used for parent GameTSControl)
	GuiControl *parent = getParent();
	if (parent && parent->isMethod("onMouseDragged"))
	{
		parent->onMouseDragged(event);
		//return;
	}

	// update the drag selection
	if (mDragSelect)
	{
		// build the drag selection on the renderScene method - make sure no neg extent!
		mDragRect.point.x = (event.mousePoint.x < mDragStart.x) ? event.mousePoint.x : mDragStart.x;
		mDragRect.extent.x = (event.mousePoint.x > mDragStart.x) ? event.mousePoint.x - mDragStart.x : mDragStart.x - event.mousePoint.x;
		mDragRect.point.y = (event.mousePoint.y < mDragStart.y) ? event.mousePoint.y : mDragStart.y;
		mDragRect.extent.y = (event.mousePoint.y > mDragStart.y) ? event.mousePoint.y - mDragStart.y : mDragStart.y - event.mousePoint.y;
		return;
	}
}



//------------------------------------------------------------------------------

