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
#include "labDropdownCtrl.h"
#include "gui/core/guiDefaultControlRender.h"
#include "console/consoleTypes.h"
#include "console/engineAPI.h"
#include "gfx/gfxDrawUtil.h"
#include "gui/core/guiCanvas.h"
#include "gfx/primBuilder.h"


IMPLEMENT_CONOBJECT(LabDropdownCtrl);

ConsoleDocClass(LabDropdownCtrl,
	"@brief Brief Desc.\n\n"

	"@tsexample\n"
	"// Comment:\n"
	"%okButton = new ClassObject()\n"
	"instantiation\n"
	"@endtsexample\n\n"

	"@ingroup GuiContainers"
	);


//-----------------------------------------------------------------------------


LabDropdownCtrl::LabDropdownCtrl()
{
	
}

//-----------------------------------------------------------------------------

LabDropdownCtrl::~LabDropdownCtrl()
{

}

//-----------------------------------------------------------------------------

void LabDropdownCtrl::initPersistFields()
{
	
	Parent::initPersistFields();
}


//-----------------------------------------------------------------------------

