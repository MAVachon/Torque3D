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

#ifndef _TERRAINLABACTIONS_H_
#define _TERRAINLABACTIONS_H_

#ifndef _TERRAINLABEDITOR_H_
#include "gui/lab/world/terrainLabEditor.h"
#endif
#ifndef _GUIFILTERCTRL_H_
#include "gui/editor/guiFilterCtrl.h"
#endif
#ifndef _UNDO_H_
#include "util/undo.h"
#endif
#ifndef _NOISE2D_H_
#include "util/noise2d.h"
#endif

class TerrainLabAction
{
   protected:
      TerrainLabEditor *         mTerrainLabEditor;

   public:

      virtual ~TerrainLabAction(){};
      TerrainLabAction(TerrainLabEditor * editor) : mTerrainLabEditor(editor){}

      virtual StringTableEntry getName() = 0;

      enum Type {
         Begin = 0,
         Update,
         End,
         Process
      };

      //
      virtual void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type) = 0;
      virtual bool useMouseBrush() { return(true); }
};

//------------------------------------------------------------------------------

class LabSelectAction : public TerrainLabAction
{
   public:
	   LabSelectAction(TerrainLabEditor * editor) : TerrainLabAction(editor){};
      StringTableEntry getName(){return("select");}

      void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type);
};

class LabDeselectAction : public TerrainLabAction
{
   public:
	   LabDeselectAction(TerrainLabEditor * editor) : TerrainLabAction(editor){};
      StringTableEntry getName(){return("deselect");}

      void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type);
};

class LabClearAction : public TerrainLabAction
{
   public:
	   LabClearAction(TerrainLabEditor * editor) : TerrainLabAction(editor){};
      StringTableEntry getName(){return("clear");}

      void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type) {};
      bool useMouseBrush() { mTerrainLabEditor->getCurrentSel()->reset(); return true; }
};


class LabSoftSelectAction : public TerrainLabAction
{
   public:
	   LabSoftSelectAction(TerrainLabEditor * editor) : TerrainLabAction(editor){};
      StringTableEntry getName(){return("softSelect");}

      void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type);

      Filter   mFilter;
};

//------------------------------------------------------------------------------

class LabOutlineSelectAction : public TerrainLabAction
{
   public:
	   LabOutlineSelectAction(TerrainLabEditor * editor) : TerrainLabAction(editor){};
      StringTableEntry getName(){return("outlineSelect");}

      void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type);
      bool useMouseBrush() { return(false); }

   private:

      Gui3DMouseEvent   mLastEvent;
};

//------------------------------------------------------------------------------

class LabPaintMaterialAction : public TerrainLabAction
{
   public:
	   LabPaintMaterialAction(TerrainLabEditor * editor) : TerrainLabAction(editor){}
      StringTableEntry getName(){return("paintMaterial");}

      void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type);
};

//------------------------------------------------------------------------------

class LabClearMaterialsAction : public TerrainLabAction
{
public:
	LabClearMaterialsAction(TerrainLabEditor * editor) : TerrainLabAction(editor){}
   StringTableEntry getName(){return("clearMaterials");}

   void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type);
};

//------------------------------------------------------------------------------

class LabRaiseHeightAction : public TerrainLabAction
{
   public:
	   LabRaiseHeightAction(TerrainLabEditor * editor) : TerrainLabAction(editor){}
      StringTableEntry getName(){return("raiseHeight");}

      void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type);
};

//------------------------------------------------------------------------------

class LabLowerHeightAction : public TerrainLabAction
{
   public:
	   LabLowerHeightAction(TerrainLabEditor * editor) : TerrainLabAction(editor){}
      StringTableEntry getName(){return("lowerHeight");}

      void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type);
};

//------------------------------------------------------------------------------

class LabSetHeightAction : public TerrainLabAction
{
   public:
	   LabSetHeightAction(TerrainLabEditor * editor) : TerrainLabAction(editor){}
      StringTableEntry getName(){return("setHeight");}

      void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type);
};

//------------------------------------------------------------------------------

class LabSetEmptyAction : public TerrainLabAction
{
   public:
	   LabSetEmptyAction(TerrainLabEditor * editor) : TerrainLabAction(editor){}
      StringTableEntry getName(){return("setEmpty");}

      void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type);
};

//------------------------------------------------------------------------------

class LabClearEmptyAction : public TerrainLabAction
{
   public:
	   LabClearEmptyAction(TerrainLabEditor * editor) : TerrainLabAction(editor){}
      StringTableEntry getName(){return("clearEmpty");}

      void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type);
};

//------------------------------------------------------------------------------

class LabScaleHeightAction : public TerrainLabAction
{
   public:
	   LabScaleHeightAction(TerrainLabEditor * editor) : TerrainLabAction(editor){}
      StringTableEntry getName(){return("scaleHeight");}

      void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type);
};

//------------------------------------------------------------------------------

class LabBrushAdjustHeightAction : public TerrainLabAction
{
   public:
	   LabBrushAdjustHeightAction(TerrainLabEditor * editor) : TerrainLabAction(editor){}
      StringTableEntry getName(){return("brushAdjustHeight");}

      void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type);

   private:
      PlaneF mIntersectionPlane;
      Point3F mTerrainUpVector;
      F32 mPreviousZ;
};

class LabAdjustHeightAction : public LabBrushAdjustHeightAction
{
   public:
	   LabAdjustHeightAction(TerrainLabEditor * editor);
      StringTableEntry getName(){return("adjustHeight");}

      void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type);
      bool useMouseBrush() { return(false); }

   private:
      //
      Point3F                    mHitPos;
      Point3F                    mLastPos;
      SimObjectPtr<GuiCursor>    mCursor;
};

//------------------------------------------------------------------------------

class LabFlattenHeightAction : public TerrainLabAction
{
   public:
	   LabFlattenHeightAction(TerrainLabEditor * editor) : TerrainLabAction(editor){}
      StringTableEntry getName(){return("flattenHeight");}

      void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type);
};

class LabSmoothHeightAction : public TerrainLabAction
{
   public:
	   LabSmoothHeightAction(TerrainLabEditor * editor) : TerrainLabAction(editor){}
      StringTableEntry getName(){return("smoothHeight");}

      void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type);
};

class LabSmoothSlopeAction : public TerrainLabAction
{  
   public:  
	   LabSmoothSlopeAction(TerrainLabEditor * editor) : TerrainLabAction(editor){}
      StringTableEntry getName(){return("smoothSlope");}  
  
      void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type);  
};  

class LabPaintNoiseAction : public TerrainLabAction
{
   public:

	   LabPaintNoiseAction(TerrainLabEditor *editor)
         :  TerrainLabAction( editor ),
            mNoiseSize( 256 )
      {
         mNoise.setSeed( 5342219 );
         mNoiseData.setSize( mNoiseSize * mNoiseSize );
         mNoise.fBm( &mNoiseData, mNoiseSize, 12, 1.0f, 5.0f );
         //Vector<F32> scratch = mNoiseData;
         //mNoise.rigidMultiFractal( &mNoiseData, &scratch, TerrainBlock::BlockSize, 12, 1.0f, 5.0f );
         mNoise.getMinMax( &mNoiseData, &mMinMaxNoise.x, &mMinMaxNoise.y, mNoiseSize );
       
         mScale = 1.5f / ( mMinMaxNoise.x - mMinMaxNoise.y);
      }
      
      StringTableEntry getName() { return "paintNoise"; }

      void process( Selection *sel, const Gui3DMouseEvent &event, bool selChanged, Type type );

   protected:

      const U32 mNoiseSize;

      Noise2D mNoise;

      Vector<F32> mNoiseData;

      Point2F mMinMaxNoise;

      F32 mScale;
};

/*
class ThermalErosionAction : public TerrainLabAction
{
   public:
      ThermalErosionAction(TerrainLabEditor * editor) 
      : TerrainLabAction(editor)
      {
         mNoise.setSeed( 1 );//Sim::getCurrentTime() );
         mNoiseData.setSize( TerrainBlock::BlockSize * TerrainBlock::BlockSize );
         mTerrainHeights.setSize( TerrainBlock::BlockSize * TerrainBlock::BlockSize );
      }
      
      StringTableEntry getName(){return("thermalErode");}

      void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type);

      Noise2D mNoise;
      Vector<F32> mNoiseData;
      Vector<F32> mTerrainHeights;
};
*/

/// An undo action used to perform terrain wide smoothing.
class LabTerrainSmoothAction : public UndoAction
{
   typedef UndoAction Parent;

protected:

   SimObjectId mTerrainId;

   U32 mSteps;

   F32 mFactor;

   Vector<U16> mUnsmoothedHeights;

public:

	LabTerrainSmoothAction();

   // SimObject
	DECLARE_CONOBJECT(LabTerrainSmoothAction);
   static void initPersistFields();

   // UndoAction
   virtual void undo();
   virtual void redo();

   /// Performs the initial smoothing and stores
   /// the heighfield state for later undo.
   void smooth( TerrainBlock *terrain, F32 factor, U32 steps );
};


#endif // _TERRAINLABACTIONS_H_
