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

#ifndef _LABHUDCTRL_H_
#define _LABHUDCTRL_H_

#ifndef _GUITSCONTROL_H_
#include "gui/3d/guiTSControl.h"
#endif



struct ObjectRenderInst;
class SceneRenderState;
class BaseMatInstance;


class LabHudCtrl : public GuiTSCtrl
{
      typedef GuiTSCtrl Parent;
		void makeScriptCall(const char *func, const GuiEvent &evt) const;
   protected:


		virtual void onRightMouseDown(const GuiEvent &event);
		virtual void onMiddleMouseDown(const GuiEvent &event);


		virtual void onRightMouseUp(const GuiEvent &event);
		virtual void onMiddleMouseUp(const GuiEvent &event);
      virtual void onMouseUp(const GuiEvent & event);
      virtual void onMouseDown(const GuiEvent & event);
      virtual void onMouseMove(const GuiEvent & event);
      virtual void onMouseDragged(const GuiEvent & event);
      /*
			virtual void onMouseEnter(const GuiEvent & event);
      virtual void onMouseLeave(const GuiEvent & event);
      virtual void onRightMouseDown(const GuiEvent & event);
      virtual void onRightMouseUp(const GuiEvent & event);
      virtual void onRightMouseDragged(const GuiEvent & event);
      virtual void onMiddleMouseDown(const GuiEvent & event);
      virtual void onMiddleMouseUp(const GuiEvent & event);
      virtual void onMiddleMouseDragged(const GuiEvent & event);
      virtual bool onInputEvent(const InputEventInfo & event);
      virtual bool onMouseWheelUp(const GuiEvent &event);
      virtual bool onMouseWheelDown(const GuiEvent &event);
		*/
public:
	ColorI            mDragRectColor;
private:
		bool                       mMouseDown;
		
		bool                       mDragSelect;
		RectI                      mDragRect;
		Point2I                    mDragStart;
   protected: 

      Gui3DMouseEvent   mLastEvent;
      bool              mLeftMouseDown;
      bool              mRightMouseDown;
      bool              mMiddleMouseDown;
      bool              mMiddleMouseTriggered;
      bool              mMouseLeft;
		Point2I                    mLastMousePos;
     

   public:

      LabHudCtrl();
      ~LabHudCtrl();

   

      static void initPersistFields();
		
      // guiControl
      virtual void onRender(Point2I offset, const RectI &updateRect);  
     

      virtual bool isMiddleMouseDown() {return mMiddleMouseDown;}

  

      /// Set flags or other Gizmo state appropriate for the current situation.
      /// For example derived classes may override this to disable certain
      /// axes of modes of manipulation.
    
      DECLARE_CONOBJECT(LabHudCtrl);
      DECLARE_CATEGORY( "Gui Editor" );
};

#endif // _LABHUDCTRL_H_
