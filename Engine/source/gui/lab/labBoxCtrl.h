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

#ifndef _LABBOXCTRL_H_
#define _LABBOXCTRL_H_

#ifndef _GUICONTAINER_H_
#include "gui/containers/guiContainer.h"
#endif


/// Base class for controls that act as containers to other controls.
///
/// @addtogroup gui_container_group Containers
///
/// @ingroup gui_group Gui System
/// @{
class  LabBoxCtrl : public GuiContainer
{
   public:   
	   typedef GuiContainer Parent;   

   protected:

	   /// Window title string.
	   String mText;
	   bool  mResizing;
	   S32   mSpacing;
	   Point2I mPadding;

	   /// Allow moving window.
	   bool mCanMove;
	   bool mMouseMovingWin;

	   Point2I mMousePosition;
	   Point2I mMouseDownPosition;
	   RectI mOrigBounds;
	   RectI mStandardBounds;
	   /// @name Callbacks
	   /// @{
	   DECLARE_CALLBACK(void, onResize, ());
		DECLARE_CALLBACK(void, onDropped, ());
	   /// @}
   public:
   
      DECLARE_CONOBJECT(LabBoxCtrl);
      DECLARE_CATEGORY( "Gui Containers" );

      LabBoxCtrl();
      virtual ~LabBoxCtrl();

      static void initPersistFields();

      /// @name Container Sizing
      /// @{

	  bool onWake();
      /// Docking Accessors
	  bool resize(const Point2I &newPosition, const Point2I &newExtent);
	
	  bool isFrozen() { return mResizing; };
	  /// prevent resizing. useful when adding many items.
	  void freeze(bool);
      /// @}   

      /// @name Sizing Constraints
      /// @{
      virtual const RectI getClientRect();
      /// @}

	  virtual void onMouseDown(const GuiEvent &event);
	  virtual void onMouseDragged(const GuiEvent &event);
	  virtual void onMouseUp(const GuiEvent &event);
     virtual void onPreRender();      
     
	  void onRender(Point2I offset, const RectI &updateRect);
	  /// GuiContainer deals with parentResized calls differently than GuiControl.  It will
	  /// update the layout for all of it's non-docked child controls.  parentResized calls
	  /// on the child controls will be handled by their default functions, but for our
	  /// purposes we want at least our immediate children to use the anchors that they have
	  /// set on themselves. - JDD [9/20/2006]
	  virtual void parentResized(const RectI &oldParentRect, const RectI &newParentRect);
      /// @}
};
/// @}
#endif
