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

#ifndef _LAB_TREEVIEWCTRL_H
#define _LAB_TREEVIEWCTRL_H

#include "core/bitSet.h"
#include "math/mRect.h"
#include "gfx/gFont.h"
#include "gui/core/guiControl.h"
#include "gui/core/guiArrayCtrl.h"
#ifndef _GUI_TREEVIEWCTRL_H
#include "gui/controls/guiTreeViewCtrl.h"
#endif

class GuiTextEditCtrl;

//------------------------------------------------------------------------------

class LabTreeViewCtrl : public GuiTreeViewCtrl
{
   private:
      typedef GuiTreeViewCtrl Parent;

   

   public:
      LabTreeViewCtrl();
      virtual ~LabTreeViewCtrl();


      DECLARE_CONOBJECT(LabTreeViewCtrl);
      DECLARE_CATEGORY( "Gui Lists" );
      DECLARE_DESCRIPTION( "Hierarchical list of text items with optional icons.\nCan also be used to inspect SimObject hierarchies." );
};

#endif
