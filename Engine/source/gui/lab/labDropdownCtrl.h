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

#ifndef _LABBOXROLLOUTCTRL_H_
#define _LABBOXROLLOUTCTRL_H_

#ifndef _GUIPOPUPCTRLEX_H_
#include "gui/controls/guiPopUpCtrlEx.h"
#endif

/// Base class for controls that act as containers to other controls.
///
/// @addtogroup gui_container_group Containers
///
/// @ingroup gui_group Gui System
/// @{
class LabDropdownCtrl;
class LabDropdownTextListCtrl;

class LabDropdownBackgroundCtrl : public GuiPopUpBackgroundCtrlEx
{
protected:
	LabDropdownCtrl *mPopUpCtrl;
	LabDropdownTextListCtrl *mTextList;
public:
	LabDropdownBackgroundCtrl(LabDropdownCtrl *ctrl, LabDropdownTextListCtrl* textList);
	void onMouseDown(const GuiEvent &event);
};

class LabDropdownTextListCtrl : public GuiPopupTextListCtrlEx
{
private:
	typedef GuiPopupTextListCtrlEx Parent;

	

protected:
	LabDropdownCtrl *mPopUpCtrl;

public:
	LabDropdownTextListCtrl(); // for inheritance
	LabDropdownTextListCtrl(LabDropdownCtrl *ctrl);

};

class  LabDropdownCtrl : public GuiPopUpMenuCtrlEx
{
public:
	typedef GuiPopUpMenuCtrlEx Parent;

protected:

	/// Window title string.	
	bool  mExpanded;


public:

	DECLARE_CONOBJECT(LabDropdownCtrl);
	DECLARE_CATEGORY("Gui Containers");

	LabDropdownCtrl();
	virtual ~LabDropdownCtrl();


	static void initPersistFields();

};
/// @}




#endif
