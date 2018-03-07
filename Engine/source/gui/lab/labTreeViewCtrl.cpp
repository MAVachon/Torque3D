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
#include "gui/lab/labTreeViewCtrl.h"

#include "core/frameAllocator.h"
#include "core/strings/findMatch.h"
#include "gui/containers/guiScrollCtrl.h"
#include "gui/worldEditor/editorIconRegistry.h"
#include "console/consoleTypes.h"
#include "console/console.h"
#include "gui/core/guiTypes.h"
#include "gfx/gfxDrawUtil.h"
#include "gui/controls/guiTextEditCtrl.h"
#ifdef TORQUE_TOOLS
#include "gui/editor/editorFunctions.h"
#endif
#include "console/engineAPI.h"
#ifdef TORQUE_EXPERIMENTAL_EC
#include "T3D/entity.h"
#endif

IMPLEMENT_CONOBJECT(LabTreeViewCtrl);

ConsoleDocClass(LabTreeViewCtrl,
	"@brief Hierarchical list of text items with optional icons.\n\n"

	"Can also be used to inspect SimObject hierarchies, primarily within editors.\n\n"

	"LabTreeViewCtrls can either display arbitrary user-defined trees or can be used to display SimObject hierarchies where "
	"each parent node in the tree is a SimSet or SimGroup and each leaf node is a SimObject.\n\n"

	"Each item in the tree has a text and a value.  For trees that display SimObject hierarchies, the text for each item "
	"is automatically derived from objects while the value for each item is the ID of the respective SimObject.  For trees "
	"that are not tied to SimObjects, both text and value of each item are set by the user.\n\n"

	"Additionally, items in the tree can have icons.\n\n"

	"Each item in the tree has a distinct numeric ID that is unique within its tree.  The ID of the root item, which is always "
	"present on a tree, is 0.\n\n"

	"@tsexample\n"
	"new LabTreeViewCtrl(DatablockEditorTree)\n"
	"{\n"
	"	tabSize = \"16\";\n"
	"	textOffset = \"2\";\n"
	"	fullRowSelect = \"0\";\n"
	"	itemHeight = \"21\";\n"
	"	destroyTreeOnSleep = \"0\";\n"
	"	MouseDragging = \"0\";\n"
	"	MultipleSelections = \"1\";\n"
	"	DeleteObjectAllowed = \"1\";\n"
	"	DragToItemAllowed = \"0\";\n"
	"	ClearAllOnSingleSelection = \"1\";\n"
	"	showRoot = \"1\";\n"
	"	internalNamesOnly = \"0\";\n"
	"	objectNamesOnly = \"0\";\n"
	"	compareToObjectID = \"0\";\n"
	"	Profile = \"LabTreeViewProfile\";\n"
	"	tooltipprofile = \"GuiToolTipProfile\";\n"
	"	hovertime = \"1000\";\n"
	"};\n"
	"@endtsexample\n\n"

	"@ingroup GuiContainers\n");


//=============================================================================
//    LabTreeViewCtrl.
//=============================================================================
// MARK: ---- LabTreeViewCtrl ----

//-----------------------------------------------------------------------------

LabTreeViewCtrl::LabTreeViewCtrl()
{
	VECTOR_SET_ASSOCIATION(mItems);
	VECTOR_SET_ASSOCIATION(mVisibleItems);
	VECTOR_SET_ASSOCIATION(mSelectedItems);
	VECTOR_SET_ASSOCIATION(mSelected);

	mItemFreeList = NULL;
	mRoot = NULL;
	mItemCount = 0;
	mSelectedItem = 0;
	mStart = 0;
	mPossibleRenameItem = NULL;
	mRenamingItem = NULL;
	mTempItem = NULL;
	mRenameCtrl = NULL;

	mDraggedToItem = 0;
	mCurrentDragCell = 0;
	mPreviousDragCell = 0;
	mDragMidPoint = NomDragMidPoint;
	mMouseDragged = false;
	mDebug = false;

	// persist info..
	mTabSize = 16;
	mTextOffset = 2;
	mFullRowSelect = false;
	mItemHeight = 20;

	//
	setSize(Point2I(1, 0));

	// Set up default state
	mFlags.set(ShowTreeLines);
	mFlags.set(IsEditable, false);

	mDestroyOnSleep = true;
	mSupportMouseDragging = true;
	mMultipleSelections = true;
	mDeleteObjectAllowed = true;
	mDragToItemAllowed = true;
	mShowRoot = true;
	mUseInspectorTooltips = false;
	mTooltipOnWidthOnly = false;
	mCompareToObjectID = true;
	mShowObjectIds = true;
	mShowClassNames = true;
	mShowObjectNames = true;
	mShowInternalNames = true;
	mShowClassNameForUnnamedObjects = false;
	mFlags.set(RebuildVisible);

	mCanRenameObjects = true;
	mRenameInternal = false;

	mClearAllOnSingleSelection = true;

	mBitmapBase = StringTable->insert("");
	mTexRollover = NULL;
	mTexSelected = NULL;

	mRenderTooltipDelegate.bind(this, &LabTreeViewCtrl::renderTooltip);
}

//-----------------------------------------------------------------------------

LabTreeViewCtrl::~LabTreeViewCtrl()
{
	_destroyTree();
}