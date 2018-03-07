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

#include "console/engineAPI.h"
#include "platform/platform.h"
#include "gui/lab/world/terrainLabActions.h"

#include "gui/core/guiCanvas.h"

//------------------------------------------------------------------------------

void LabSelectAction::process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type)
{
   if(sel == mTerrainLabEditor->getCurrentSel())
      return;

   if(type == Process)
      return;

   if(selChanged)
   {
      if(event.modifier & SI_MULTISELECT)
      {
         for(U32 i = 0; i < sel->size(); i++)
            mTerrainLabEditor->getCurrentSel()->remove((*sel)[i]);
      }
      else
      {
         for(U32 i = 0; i < sel->size(); i++)
         {
            GridInfo gInfo;
            if(mTerrainLabEditor->getCurrentSel()->getInfo((*sel)[i].mGridPoint.gridPos, gInfo))
            {
               if(!gInfo.mPrimarySelect)
                  gInfo.mPrimarySelect = (*sel)[i].mPrimarySelect;

               if(gInfo.mWeight < (*sel)[i].mWeight)
                  gInfo.mWeight = (*sel)[i].mWeight;

               mTerrainLabEditor->getCurrentSel()->setInfo(gInfo);
            }
            else
               mTerrainLabEditor->getCurrentSel()->add((*sel)[i]);
         }
      }
   }
}

void LabDeselectAction::process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type)
{
   if(sel == mTerrainLabEditor->getCurrentSel())
      return;

   if(type == Process)
      return;

   if(selChanged)
   {
      for(U32 i = 0; i < sel->size(); i++)
         mTerrainLabEditor->getCurrentSel()->remove((*sel)[i]);
   }
}

//------------------------------------------------------------------------------

void LabSoftSelectAction::process(Selection * sel, const Gui3DMouseEvent &, bool selChanged, Type type)
{
   TerrainBlock *terrBlock = mTerrainLabEditor->getActiveTerrain();
   if ( !terrBlock )
      return;
      
   // allow process of current selection
   Selection tmpSel;
   if(sel == mTerrainLabEditor->getCurrentSel())
   {
      tmpSel = *sel;
      sel = &tmpSel;
   }

   if(type == Begin || type == Process)
      mFilter.set(1, &mTerrainLabEditor->mSoftSelectFilter);

   //
   if(selChanged)
   {
      F32 radius = mTerrainLabEditor->mSoftSelectRadius;
      if(radius == 0.f)
         return;

      S32 squareSize = terrBlock->getSquareSize();
      U32 offset = U32(radius / F32(squareSize)) + 1;

      for(U32 i = 0; i < sel->size(); i++)
      {
         GridInfo & info = (*sel)[i];

         info.mPrimarySelect = true;
         info.mWeight = mFilter.getValue(0);

         if(!mTerrainLabEditor->getCurrentSel()->add(info))
            mTerrainLabEditor->getCurrentSel()->setInfo(info);

         Point2F infoPos((F32)info.mGridPoint.gridPos.x, (F32)info.mGridPoint.gridPos.y);

         //
         for(S32 x = info.mGridPoint.gridPos.x - offset; x < info.mGridPoint.gridPos.x + (offset << 1); x++)
            for(S32 y = info.mGridPoint.gridPos.y - offset; y < info.mGridPoint.gridPos.y + (offset << 1); y++)
            {
               //
               Point2F pos((F32)x, (F32)y);

               F32 dist = Point2F(pos - infoPos).len() * F32(squareSize);

               if(dist > radius)
                  continue;

               F32 weight = mFilter.getValue(dist / radius);

               //
               GridInfo gInfo;
               GridPoint gridPoint = info.mGridPoint;
               gridPoint.gridPos.set(x, y);

               if(mTerrainLabEditor->getCurrentSel()->getInfo(Point2I(x, y), gInfo))
               {
                  if(gInfo.mPrimarySelect)
                     continue;

                  if(gInfo.mWeight < weight)
                  {
                     gInfo.mWeight = weight;
                     mTerrainLabEditor->getCurrentSel()->setInfo(gInfo);
                  }
               }
               else
               {
                  Vector<GridInfo> gInfos;
                  mTerrainLabEditor->getGridInfos(gridPoint, gInfos);

                  for (U32 z = 0; z < gInfos.size(); z++)
                  {
                     gInfos[z].mWeight = weight;
                     gInfos[z].mPrimarySelect = false;
                     mTerrainLabEditor->getCurrentSel()->add(gInfos[z]);
                  }
               }
            }
      }
   }
}

//------------------------------------------------------------------------------

void LabOutlineSelectAction::process(Selection * sel, const Gui3DMouseEvent & event, bool, Type type)
{
   TORQUE_UNUSED(sel); TORQUE_UNUSED(event); TORQUE_UNUSED(type);
   switch(type)
   {
      case Begin:
         if(event.modifier & SI_SHIFT)
            break;

         mTerrainLabEditor->getCurrentSel()->reset();
         break;

      case End:
      case Update:

      default:
         return;
   }

   mLastEvent = event;
}

//------------------------------------------------------------------------------

void LabPaintMaterialAction::process(Selection * sel, const Gui3DMouseEvent &, bool selChanged, Type)
{
   S32 mat = mTerrainLabEditor->getPaintMaterialIndex();
   if ( !selChanged || mat < 0 )
      return;

   const bool slopeLimit = mTerrainLabEditor->mSlopeMinAngle > 0.0f || mTerrainLabEditor->mSlopeMaxAngle < 90.0f;
   const F32 minSlope = mSin( mDegToRad( 90.0f - mTerrainLabEditor->mSlopeMinAngle ) );
   const F32 maxSlope = mSin( mDegToRad( 90.0f - mTerrainLabEditor->mSlopeMaxAngle ) );

   const TerrainBlock *terrain = mTerrainLabEditor->getActiveTerrain();
   const F32 squareSize = terrain->getSquareSize();

   Point2F p;
   Point3F norm;


   for( U32 i = 0; i < sel->size(); i++ )
   {
      GridInfo &inf = (*sel)[i];

      if ( slopeLimit )
      {
         p.x = inf.mGridPoint.gridPos.x * squareSize;
         p.y = inf.mGridPoint.gridPos.y * squareSize;
         if ( !terrain->getNormal( p, &norm, true ) )
            continue;

         if (  norm.z > minSlope ||
               norm.z < maxSlope )
            continue;  
      }

      // If grid is already set to our material, or it is an
      // empty grid spot, then skip painting.
      if ( inf.mMaterial == mat || inf.mMaterial == U8_MAX )
         continue;

      if ( mRandF() > mTerrainLabEditor->getBrushPressure() )
         continue;

      inf.mMaterialChanged = true;
      mTerrainLabEditor->getUndoSel()->add(inf);

      // Painting is really simple now... set the one mat index.
      inf.mMaterial = mat;
      mTerrainLabEditor->setGridInfo(inf, true);
   }

   mTerrainLabEditor->scheduleMaterialUpdate();
}

//------------------------------------------------------------------------------

void LabClearMaterialsAction::process(Selection * sel, const Gui3DMouseEvent &, bool selChanged, Type)
{
   if(selChanged)
   {
      for(U32 i = 0; i < sel->size(); i++)
      {
         GridInfo &inf = (*sel)[i];

         mTerrainLabEditor->getUndoSel()->add(inf);
         inf.mMaterialChanged = true;

         // Reset to the first texture layer.
         inf.mMaterial = 0; 
         mTerrainLabEditor->setGridInfo(inf);
      }
      mTerrainLabEditor->scheduleMaterialUpdate();
   }
}

//------------------------------------------------------------------------------

void LabRaiseHeightAction::process( Selection *sel, const Gui3DMouseEvent &evt, bool selChanged, Type type )
{
   // ok the raise height action is our "dirt pour" action
   // only works on brushes...

   Brush *brush = dynamic_cast<Brush*>(sel);
   if ( !brush )
      return;

   if ( type == End )   
      return;

   Point2I brushPos = brush->getPosition();
   GridPoint brushGridPoint = brush->getGridPoint();

   Vector<GridInfo> cur; // the height at the brush position
   mTerrainLabEditor->getGridInfos(brushGridPoint, cur);

   if ( cur.size() == 0 )
      return;

   // we get 30 process actions per second (at least)
   F32 heightAdjust = mTerrainLabEditor->mAdjustHeightVal / 30;
   // nothing can get higher than the current brush pos adjusted height

   F32 maxHeight = cur[0].mHeight + heightAdjust;

   for ( U32 i = 0; i < sel->size(); i++ )
   {
      mTerrainLabEditor->getUndoSel()->add((*sel)[i]);
      if ( (*sel)[i].mHeight < maxHeight )
      {
         (*sel)[i].mHeight += heightAdjust * (*sel)[i].mWeight;
         if ( (*sel)[i].mHeight > maxHeight )
            (*sel)[i].mHeight = maxHeight;
      }
      mTerrainLabEditor->setGridInfo((*sel)[i]);
   }   

   mTerrainLabEditor->scheduleGridUpdate();  
}

//------------------------------------------------------------------------------

void LabLowerHeightAction::process(Selection * sel, const Gui3DMouseEvent &, bool selChanged, Type type)
{
   // ok the lower height action is our "dirt dig" action
   // only works on brushes...

   Brush *brush = dynamic_cast<Brush *>(sel);
   if(!brush)
      return;

   if ( type == End )   
      return;

   Point2I brushPos = brush->getPosition();
   GridPoint brushGridPoint = brush->getGridPoint();

   Vector<GridInfo> cur; // the height at the brush position
   mTerrainLabEditor->getGridInfos(brushGridPoint, cur);

   if (cur.size() == 0)
      return;

   // we get 30 process actions per second (at least)
   F32 heightAdjust = -mTerrainLabEditor->mAdjustHeightVal / 30;
   // nothing can get higher than the current brush pos adjusted height

   F32 maxHeight = cur[0].mHeight + heightAdjust;
   if(maxHeight < 0)
      maxHeight = 0;

   for(U32 i = 0; i < sel->size(); i++)
   {
      mTerrainLabEditor->getUndoSel()->add((*sel)[i]);
      if((*sel)[i].mHeight > maxHeight)
      {
         (*sel)[i].mHeight += heightAdjust * (*sel)[i].mWeight;
         if((*sel)[i].mHeight < maxHeight)
            (*sel)[i].mHeight = maxHeight;
      }
      mTerrainLabEditor->setGridInfo((*sel)[i]);
   }

   mTerrainLabEditor->scheduleGridUpdate();   
}

//------------------------------------------------------------------------------

void LabSetHeightAction::process(Selection * sel, const Gui3DMouseEvent &, bool selChanged, Type)
{
   if(selChanged)
   {
      for(U32 i = 0; i < sel->size(); i++)
      {
         mTerrainLabEditor->getUndoSel()->add((*sel)[i]);
         (*sel)[i].mHeight = mTerrainLabEditor->mSetHeightVal;
         mTerrainLabEditor->setGridInfo((*sel)[i]);
      }
      mTerrainLabEditor->scheduleGridUpdate();
   }
}

//------------------------------------------------------------------------------

void LabSetEmptyAction::process(Selection * sel, const Gui3DMouseEvent &, bool selChanged, Type)
{
   if ( !selChanged )
      return;

   mTerrainLabEditor->setMissionDirty();

   for ( U32 i = 0; i < sel->size(); i++ )
   {
      GridInfo &inf = (*sel)[i];

      // Skip already empty blocks.
      if ( inf.mMaterial == U8_MAX )
         continue;

      // The change flag needs to be set on the undo
      // so that it knows to restore materials.
      inf.mMaterialChanged = true;
      mTerrainLabEditor->getUndoSel()->add( inf );

      // Set the material to empty.
      inf.mMaterial = -1;
      mTerrainLabEditor->setGridInfo( inf );
   }

   mTerrainLabEditor->scheduleGridUpdate();
}

//------------------------------------------------------------------------------

void LabClearEmptyAction::process(Selection * sel, const Gui3DMouseEvent &, bool selChanged, Type)
{
   if ( !selChanged )
      return;

   mTerrainLabEditor->setMissionDirty();

   for ( U32 i = 0; i < sel->size(); i++ )
   {
      GridInfo &inf = (*sel)[i];

      // Skip if not empty.
      if ( inf.mMaterial != U8_MAX )
         continue;

      // The change flag needs to be set on the undo
      // so that it knows to restore materials.
      inf.mMaterialChanged = true;
      mTerrainLabEditor->getUndoSel()->add( inf );

      // Set the material
      inf.mMaterial = 0;
      mTerrainLabEditor->setGridInfo( inf );
   }

   mTerrainLabEditor->scheduleGridUpdate();
}

//------------------------------------------------------------------------------

void LabScaleHeightAction::process(Selection * sel, const Gui3DMouseEvent &, bool selChanged, Type)
{
   if(selChanged)
   {
      for(U32 i = 0; i < sel->size(); i++)
      {
         mTerrainLabEditor->getUndoSel()->add((*sel)[i]);
         (*sel)[i].mHeight *= mTerrainLabEditor->mScaleVal;
         mTerrainLabEditor->setGridInfo((*sel)[i]);
      }
      mTerrainLabEditor->scheduleGridUpdate();
   }
}

void LabBrushAdjustHeightAction::process(Selection * sel, const Gui3DMouseEvent & event, bool, Type type)
{
   if(type == Process)
      return;

   TerrainBlock *terrBlock = mTerrainLabEditor->getActiveTerrain();
   if ( !terrBlock )
      return;

   if(type == Begin)
   {
      mTerrainLabEditor->lockSelection(true);
      mTerrainLabEditor->getRoot()->mouseLock(mTerrainLabEditor);

      // the way this works is:
      // construct a plane that goes through the collision point
      // with one axis up the terrain Z, and horizontally parallel to the
      // plane of projection

      // the cross of the camera ffdv and the terrain up vector produces
      // the cross plane vector.

      // all subsequent mouse actions are collided against the plane and the deltaZ
      // from the previous position is used to delta the selection up and down.
      Point3F cameraDir;

      EditLabTSCtrl::smCamMatrix.getColumn(1, &cameraDir);
      terrBlock->getTransform().getColumn(2, &mTerrainUpVector);

      // ok, get the cross vector for the plane:
      Point3F planeCross;
      mCross(cameraDir, mTerrainUpVector, &planeCross);

      planeCross.normalize();
      Point3F planeNormal;

      Point3F intersectPoint;
      mTerrainLabEditor->collide(event, intersectPoint);

      mCross(mTerrainUpVector, planeCross, &planeNormal);
      mIntersectionPlane.set(intersectPoint, planeNormal);

      // ok, we have the intersection point...
      // project the collision point onto the up vector of the terrain

      mPreviousZ = mDot(mTerrainUpVector, intersectPoint);

      // add to undo
      // and record the starting heights
      for(U32 i = 0; i < sel->size(); i++)
      {
         mTerrainLabEditor->getUndoSel()->add((*sel)[i]);
         (*sel)[i].mStartHeight = (*sel)[i].mHeight;
      }
   }
   else if(type == Update)
   {
      // ok, collide the ray from the event with the intersection plane:

      Point3F intersectPoint;
      Point3F start = event.pos;
      Point3F end = start + event.vec * 1000;

      F32 t = mIntersectionPlane.intersect(start, end);

      m_point3F_interpolate( start, end, t, intersectPoint);
      F32 currentZ = mDot(mTerrainUpVector, intersectPoint);

      F32 diff = currentZ - mPreviousZ;

      for(U32 i = 0; i < sel->size(); i++)
      {
         (*sel)[i].mHeight = (*sel)[i].mStartHeight + diff * (*sel)[i].mWeight;

         // clamp it
         if((*sel)[i].mHeight < 0.f)
            (*sel)[i].mHeight = 0.f;
         if((*sel)[i].mHeight > 2047.f)
            (*sel)[i].mHeight = 2047.f;

         mTerrainLabEditor->setGridInfoHeight((*sel)[i]);
      }
      mTerrainLabEditor->scheduleGridUpdate();
   }
   else if(type == End)
   {
      mTerrainLabEditor->getRoot()->mouseUnlock(mTerrainLabEditor);
   }
}

//------------------------------------------------------------------------------

LabAdjustHeightAction::LabAdjustHeightAction(TerrainLabEditor * editor) :
LabBrushAdjustHeightAction(editor)
{
   mCursor = 0;
}

void LabAdjustHeightAction::process(Selection *sel, const Gui3DMouseEvent & event, bool b, Type type)
{
   Selection * curSel = mTerrainLabEditor->getCurrentSel();
   LabBrushAdjustHeightAction::process(curSel, event, b, type);
}

//------------------------------------------------------------------------------
// flatten the primary selection then blend in the rest...

void LabFlattenHeightAction::process(Selection * sel, const Gui3DMouseEvent &, bool selChanged, Type)
{
   if(!sel->size())
      return;

   if(selChanged)
   {
      F32 average = 0.f;

      // get the average height
      U32 cPrimary = 0;
      for(U32 k = 0; k < sel->size(); k++)
         if((*sel)[k].mPrimarySelect)
         {
            cPrimary++;
            average += (*sel)[k].mHeight;
         }

      average /= cPrimary;

      // set it
      for(U32 i = 0; i < sel->size(); i++)
      {
         mTerrainLabEditor->getUndoSel()->add((*sel)[i]);

         //
         if((*sel)[i].mPrimarySelect)
            (*sel)[i].mHeight = average;
         else
         {
            F32 h = average - (*sel)[i].mHeight;
            (*sel)[i].mHeight += (h * (*sel)[i].mWeight);
         }

         mTerrainLabEditor->setGridInfo((*sel)[i]);
      }
      mTerrainLabEditor->scheduleGridUpdate();
   }
}

//------------------------------------------------------------------------------

void LabSmoothHeightAction::process(Selection * sel, const Gui3DMouseEvent &, bool selChanged, Type)
{
   if(!sel->size())
      return;

   if(selChanged)
   {
      F32 avgHeight = 0.f;
      for(U32 k = 0; k < sel->size(); k++)
      {
         mTerrainLabEditor->getUndoSel()->add((*sel)[k]);
         avgHeight += (*sel)[k].mHeight;
      }

      avgHeight /= sel->size();

      // clamp the terrain smooth factor...
      if(mTerrainLabEditor->mSmoothFactor < 0.f)
         mTerrainLabEditor->mSmoothFactor = 0.f;
      if(mTerrainLabEditor->mSmoothFactor > 1.f)
         mTerrainLabEditor->mSmoothFactor = 1.f;

      // linear
      for(U32 i = 0; i < sel->size(); i++)
      {
         (*sel)[i].mHeight += (avgHeight - (*sel)[i].mHeight) * mTerrainLabEditor->mSmoothFactor * (*sel)[i].mWeight;
         mTerrainLabEditor->setGridInfo((*sel)[i]);
      }
      mTerrainLabEditor->scheduleGridUpdate();
   }
}

void LabSmoothSlopeAction::process(Selection * sel, const Gui3DMouseEvent &, bool selChanged, Type)  
{  
   if(!sel->size())  
      return;  
  
   if(selChanged)  
   {  
      // Perform simple 2d linear regression on x&z and y&z:  
      // b = (Avg(xz) - Avg(x)Avg(z))/(Avg(x^2) - Avg(x)^2)  
      Point2F prod(0.f, 0.f);   // mean of product for covar  
      Point2F avgSqr(0.f, 0.f); // mean sqr of x, y for var  
      Point2F avgPos(0.f, 0.f);  
      F32 avgHeight = 0.f;  
      F32 z;  
      Point2F pos;  
      for(U32 k = 0; k < sel->size(); k++)  
      {  
         mTerrainLabEditor->getUndoSel()->add((*sel)[k]);  
         pos = Point2F((*sel)[k].mGridPoint.gridPos.x, (*sel)[k].mGridPoint.gridPos.y);  
         z = (*sel)[k].mHeight;  
  
         prod += pos * z;  
         avgSqr += pos * pos;  
         avgPos += pos;  
         avgHeight += z;  
      }  
  
      prod /= sel->size();  
      avgSqr /= sel->size();  
      avgPos /= sel->size();  
      avgHeight /= sel->size();  
  
      Point2F avgSlope = (prod - avgPos*avgHeight)/(avgSqr - avgPos*avgPos);  
  
      F32 goalHeight;  
      for(U32 i = 0; i < sel->size(); i++)  
      {  
         goalHeight = avgHeight + ((*sel)[i].mGridPoint.gridPos.x - avgPos.x)*avgSlope.x +  
            ((*sel)[i].mGridPoint.gridPos.y - avgPos.y)*avgSlope.y;  
         (*sel)[i].mHeight += (goalHeight - (*sel)[i].mHeight) * (*sel)[i].mWeight;  
         mTerrainLabEditor->setGridInfo((*sel)[i]);  
      }  
      mTerrainLabEditor->scheduleGridUpdate();  
   }  
}  

void LabPaintNoiseAction::process(Selection * sel, const Gui3DMouseEvent &, bool selChanged, Type type)
{
   // If this is the ending
   // mouse down event, then
   // update the noise values.
   if ( type == Begin )
   {
      mNoise.setSeed( Sim::getCurrentTime() );
      mNoise.fBm( &mNoiseData, mNoiseSize, 12, 1.0f, 5.0f );
      mNoise.getMinMax( &mNoiseData, &mMinMaxNoise.x, &mMinMaxNoise.y, mNoiseSize );
    
      mScale = 1.5f / ( mMinMaxNoise.x - mMinMaxNoise.y);
   }

   if( selChanged )
   {
      for( U32 i = 0; i < sel->size(); i++ )
      {
         mTerrainLabEditor->getUndoSel()->add((*sel)[i]);

         const Point2I &gridPos = (*sel)[i].mGridPoint.gridPos;

         const F32 noiseVal = mNoiseData[ ( gridPos.x % mNoiseSize ) + 
                                          ( ( gridPos.y % mNoiseSize ) * mNoiseSize ) ];

         (*sel)[i].mHeight += (noiseVal - mMinMaxNoise.y * mScale) * (*sel)[i].mWeight * mTerrainLabEditor->mNoiseFactor;

         mTerrainLabEditor->setGridInfo((*sel)[i]);
      }

      mTerrainLabEditor->scheduleGridUpdate();
   }
}
/*
void LabThermalErosionAction::process(Selection * sel, const Gui3DMouseEvent &, bool selChanged, Type)
{
   if( selChanged )
   {
      TerrainBlock *tblock = mTerrainLabEditor->getActiveTerrain();
      if ( !tblock )
         return;
      
      F32 height = 0;
      F32 maxHeight = 0;
      U32 shift = getBinLog2( TerrainBlock::BlockSize );

      for ( U32 x = 0; x < TerrainBlock::BlockSize; x++ )
      {
         for ( U32 y = 0; y < TerrainBlock::BlockSize; y++ )
         {
            height = fixedToFloat( tblock->getHeight( x, y ) );
            mTerrainHeights[ x + (y << 8)] = height;

            if ( height > maxHeight )
               maxHeight = height;
         }
      }

      //mNoise.erodeThermal( &mTerrainHeights, &mNoiseData, 30.0f, 5.0f, 5, TerrainBlock::BlockSize, tblock->getSquareSize(), maxHeight );
         
      mNoise.erodeHydraulic( &mTerrainHeights, &mNoiseData, 1, TerrainBlock::BlockSize );

      F32 heightDiff = 0;

      for( U32 i = 0; i < sel->size(); i++ )
      {
         mTerrainLabEditor->getUndoSel()->add((*sel)[i]);

         const Point2I &gridPos = (*sel)[i].mGridPoint.gridPos;
         
         // Need to get the height difference
         // between the current height and the
         // erosion height to properly apply the
         // softness and pressure settings of the brush
         // for this selection.
         heightDiff = (*sel)[i].mHeight - mNoiseData[ gridPos.x + (gridPos.y << shift)];

         (*sel)[i].mHeight -= (heightDiff * (*sel)[i].mWeight);

         mTerrainLabEditor->setGridInfo((*sel)[i]);
      }

      mTerrainLabEditor->gridUpdateComplete();
   }
}
*/


IMPLEMENT_CONOBJECT( LabTerrainSmoothAction );

ConsoleDocClass( LabTerrainSmoothAction,
   "@brief Terrain action used for leveling varying terrain heights smoothly.\n\n"
   "Editor use only.\n\n"
   "@internal"
);

LabTerrainSmoothAction::LabTerrainSmoothAction()
   :  UndoAction( "Terrain Smoothing" )
{
}

void LabTerrainSmoothAction::initPersistFields()
{
   Parent::initPersistFields();
}

void LabTerrainSmoothAction::smooth( TerrainBlock *terrain, F32 factor, U32 steps )
{
   AssertFatal( terrain, "TerrainSmoothAction::smooth() - Got null object!" );

   // Store our input parameters.
   mTerrainId = terrain->getId();
   mSteps = steps;
   mFactor = factor;

   // The redo can do the rest.
   redo();
}

DefineConsoleMethod( LabTerrainSmoothAction, smooth, void, ( TerrainBlock *terrain, F32 factor, U32 steps ), , "( TerrainBlock obj, F32 factor, U32 steps )")
{
	if (terrain)
   	object->smooth( terrain, factor, mClamp( steps, 1, 13 ) );
}

void LabTerrainSmoothAction::undo()
{
   // First find the terrain from the id.
   TerrainBlock *terrain;
   if ( !Sim::findObject( mTerrainId, terrain ) || !terrain )
      return;

   // Get the terrain file.
   TerrainFile *terrFile = terrain->getFile();

   // Copy our stored heightmap to the file.
   terrFile->setHeightMap( mUnsmoothedHeights, false );

   // Tell the terrain to update itself.
   terrain->updateGrid( Point2I::Zero, Point2I::Max, true );
}

void LabTerrainSmoothAction::redo()
{
   // First find the terrain from the id.
   TerrainBlock *terrain;
   if ( !Sim::findObject( mTerrainId, terrain ) || !terrain )
      return;

   // Get the terrain file.
   TerrainFile *terrFile = terrain->getFile();

   // First copy the heightmap state.
   mUnsmoothedHeights = terrFile->getHeightMap();

   // Do the smooth.
   terrFile->smooth( mFactor, mSteps, false );

   // Tell the terrain to update itself.
   terrain->updateGrid( Point2I::Zero, Point2I::Max, true );
}