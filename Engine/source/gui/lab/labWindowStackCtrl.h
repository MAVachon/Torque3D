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

#ifndef _LABWINDOWSTACKCTRL_H_
#define _LABWINDOWSTACKCTRL_H_

#ifndef _LABBOXSTACKCTRL_H_
#include "labBoxStackCtrl.h"
#endif

/// Base class for controls that act as containers to other controls.
///
/// @addtogroup gui_container_group Containers
///
/// @ingroup gui_group Gui System
/// @{
class  LabWindowStackCtrl : public LabBoxStackCtrl
{
public:
	typedef LabBoxStackCtrl Parent;

protected:

	/// Window title string.	
	bool  mStackChilds;
	S32   mStackSpacing;
	S32   mMinStackHeight;
	RectSpacingI mStackMargin; // Margin surrounding the stack inside the control

	/// Allow moving window.


	
public:

	DECLARE_CONOBJECT(LabWindowStackCtrl);
	DECLARE_CATEGORY("Gui Containers");

	LabWindowStackCtrl();
	virtual ~LabWindowStackCtrl();


	static void initPersistFields();

	/// @name Container Sizing
	/// @{

	bool onWake();
	/// Docking Accessors
	bool resize(const Point2I &newPosition, const Point2I &newExtent);
	void childResized(GuiControl *child);
	bool isFrozen() { return mResizing; };
	/// prevent resizing. useful when adding many items.
	void freeze(bool);

	void updatePanes();

	virtual void stackVertical(bool fromTop);
	
	S32 getCount() { return size(); }; /// Returns the number of children in the stack

	void addObject(SimObject *obj);
	void removeObject(SimObject *obj);

	bool reOrder(SimObject* obj, SimObject* target = 0);


};
/// @}




#endif
