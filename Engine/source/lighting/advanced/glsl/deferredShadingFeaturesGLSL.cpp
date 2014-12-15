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
#include "lighting/advanced/glsl/deferredShadingFeaturesGLSL.h"

#include "lighting/advanced/advancedLightBinManager.h"
#include "shaderGen/langElement.h"
#include "shaderGen/shaderOp.h"
#include "shaderGen/conditionerFeature.h"
#include "renderInstance/renderPrePassMgr.h"
#include "materials/processedMaterial.h"
#include "materials/materialFeatureTypes.h"


//****************************************************************************
// Deferred Shading Features
//****************************************************************************

// Specular Map -> Blue of Material Buffer ( greyscaled )
void DeferredSpecMapGLSL::processPix( Vector<ShaderComponent*> &componentList, const MaterialFeatureData &fd )
{
   // Get the texture coord.
   Var *texCoord = getInTexCoord( "texCoord", "vec2", true, componentList );

   // search for color var
   Var *material = (Var*) LangElement::find( getOutputTargetVarName(ShaderFeature::RenderTarget2) );
   MultiLine * meta = new MultiLine;
   if ( !material )
   {
      // create color var
      material = new Var;
      material->setType( "vec4" );
      material->setName( getOutputTargetVarName(ShaderFeature::RenderTarget2) );
      material->setStructName("OUT");
   }

   // create texture var
   Var *specularMap = new Var;
   specularMap->setType( "sampler2D" );
   specularMap->setName( "specularMap" );
   specularMap->uniform = true;
   specularMap->sampler = true;
   specularMap->constNum = Var::getTexUnitNum();
   LangElement *texOp = new GenOp( "tex2D(@, @)", specularMap, texCoord );
   meta->addStatement(new GenOp("   @.b = dot(tex2D(@, @).rgb, vec3(0.3, 0.59, 0.11));\r\n", material, specularMap, texCoord));
   output = meta;
}

ShaderFeature::Resources DeferredSpecMapGLSL::getResources( const MaterialFeatureData &fd )
{
   Resources res; 
   res.numTex = 1;
   res.numTexReg = 1;

   return res;
}

void DeferredSpecMapGLSL::setTexData(   Material::StageData &stageDat,
                                       const MaterialFeatureData &fd,
                                       RenderPassData &passData,
                                       U32 &texIndex )
{
   GFXTextureObject *tex = stageDat.getTex( MFT_SpecularMap );
   if ( tex )
   {
      passData.mTexType[ texIndex ] = Material::Standard;
      passData.mSamplerNames[ texIndex ] = "specularMap";
      passData.mTexSlot[ texIndex++ ].texObject = tex;
   }
}

void DeferredSpecMapGLSL::processVert( Vector<ShaderComponent*> &componentList, 
                                       const MaterialFeatureData &fd )
{
   MultiLine *meta = new MultiLine;
   getOutTexCoord(   "texCoord", 
                     "vec2", 
                     true, 
                     fd.features[MFT_TexAnim], 
                     meta, 
                     componentList );
   output = meta;
}

// Specular Color -> Blue of Material Buffer ( greyscaled )
void DeferredSpecColorGLSL::processPix( Vector<ShaderComponent*> &componentList, const MaterialFeatureData &fd )
{
   Var *specularColor = (Var*)LangElement::find( "specularColor" );
   if ( !specularColor )
   {
      specularColor  = new Var( "specularColor", "vec4" );
      specularColor->uniform = true;
      specularColor->constSortPos = cspPotentialPrimitive;
   }

   MultiLine *meta = new MultiLine;

   Var *material = (Var*)LangElement::find(getOutputTargetVarName(ShaderFeature::RenderTarget2));
   if (!material)
   {
	   // create color var
	   material = new Var;
	   material->setType("vec4");
	   material->setName(getOutputTargetVarName(ShaderFeature::RenderTarget2));
       material->setStructName("OUT");
   }
   
   meta->addStatement(new GenOp("   @.b = dot(@.rgb, vec3(0.3, 0.59, 0.11));\r\n", material, specularColor));
   output = meta;
}

// Gloss Map (Alpha Channel of Specular Map) -> Alpha ( Spec Power ) of Material Info Buffer.
void DeferredGlossMapGLSL::processPix( Vector<ShaderComponent*> &componentList, const MaterialFeatureData &fd )
{
   // Get the texture coord.
   Var *texCoord = getInTexCoord( "texCoord", "vec2", true, componentList );

   MultiLine *meta = new MultiLine;

   // search for color var
   Var *color = (Var*) LangElement::find( getOutputTargetVarName(ShaderFeature::RenderTarget2) );
   if ( !color )
   {
      // create color var
      color = new Var;
      color->setType( "vec4" );
      color->setName( getOutputTargetVarName(ShaderFeature::RenderTarget2) );
	  meta->addStatement(new GenOp("   @;\r\n", new DecOp(color)));
   }

   // create texture var
   Var *specularMap = (Var*)LangElement::find( "specularMap" );
   if (!specularMap)
   {
       specularMap->setType( "sampler2D" );
       specularMap->setName( "specularMap" );
       specularMap->uniform = true;
       specularMap->sampler = true;
       specularMap->constNum = Var::getTexUnitNum();
   }
   LangElement *texOp = new GenOp( "tex2D(@, @)", specularMap, texCoord );

   meta->addStatement(new GenOp( "   @.a = @.a;\r\n", color, texOp ));
   output = meta;
}

// Material Info Flags -> Red ( Flags ) of Material Info Buffer.
void DeferredMatInfoFlagsGLSL::processPix( Vector<ShaderComponent*> &componentList, const MaterialFeatureData &fd )
{
	MultiLine *meta = new MultiLine;

   // search for material var
   Var *material = (Var*) LangElement::find( getOutputTargetVarName(ShaderFeature::RenderTarget2) );
   if ( !material )
   {
      // create material var
      material = new Var;
      material->setType( "vec4" );
      material->setName( getOutputTargetVarName(ShaderFeature::RenderTarget2) );
      material->setStructName("OUT");
   }

   Var *matInfoFlags = new Var;
   matInfoFlags->setType( "float" );
   matInfoFlags->setName( "matInfoFlags" );
   matInfoFlags->uniform = true;
   matInfoFlags->constSortPos = cspPotentialPrimitive;

   meta->addStatement(output = new GenOp("   @.r = @;\r\n", material, matInfoFlags));
   output = meta;
}

// Spec Strength -> Blue Channel of Material Info Buffer.
void DeferredSpecStrengthGLSL::processPix( Vector<ShaderComponent*> &componentList, const MaterialFeatureData &fd )
{
	MultiLine *meta = new MultiLine;

   // search for material var
   Var *material = (Var*) LangElement::find( getOutputTargetVarName(ShaderFeature::RenderTarget2) );
   if ( !material )
   {
      // create material var
      material = new Var;
      material->setType( "vec4" );
      material->setName( getOutputTargetVarName(ShaderFeature::RenderTarget2) );
      material->setStructName("OUT");
   }

   Var *specStrength = new Var;
   specStrength->setType( "float" );
   specStrength->setName( "specularStrength" );
   specStrength->uniform = true;
   specStrength->constSortPos = cspPotentialPrimitive;
   
   meta->addStatement(new GenOp("   @.b = @/128;\r\n", material, specStrength));
   output = meta;
}

// Spec Power -> Alpha Channel ( of Material Info Buffer.
void DeferredSpecPowerGLSL::processPix( Vector<ShaderComponent*> &componentList, const MaterialFeatureData &fd )
{
	MultiLine *meta = new MultiLine;

   // search for material var
   Var *material = (Var*) LangElement::find( getOutputTargetVarName(ShaderFeature::RenderTarget2) );
   if ( !material )
   {
      // create material var
      material = new Var;
      material->setType( "vec4" );
      material->setName( getOutputTargetVarName(ShaderFeature::RenderTarget2) );
      material->setStructName("OUT");
   }

   Var *specPower = new Var;
   specPower->setType( "float" );
   specPower->setName( "specularPower" );
   specPower->uniform = true;
   specPower->constSortPos = cspPotentialPrimitive;

   meta->addStatement(new GenOp("   @.a = @/5;\r\n", material, specPower ) );
   output = meta;
}

// Black -> Blue and Alpha of Color Buffer (representing no specular)
void DeferredEmptySpecGLSL::processPix( Vector<ShaderComponent*> &componentList, const MaterialFeatureData &fd )
{
   // Get the texture coord.
   Var *texCoord = getInTexCoord( "texCoord", "vec2", true, componentList );

	MultiLine *meta = new MultiLine;

   // search for material var
   Var *material = (Var*) LangElement::find( getOutputTargetVarName(ShaderFeature::RenderTarget2) );
   if ( !material )
   {
      // create material var
      material = new Var;
      material->setType( "vec4" );
      material->setName( getOutputTargetVarName(ShaderFeature::RenderTarget2) );
      material->setStructName("OUT");
   }
   
   meta->addStatement(new GenOp( "   @.ba = vec2(0.0);\r\n", material ));
   output = meta;
}

// Tranlucency -> Green of Material Info Buffer.
void DeferredTranslucencyMapGLSL::processPix( Vector<ShaderComponent*> &componentList, const MaterialFeatureData &fd )
{
	MultiLine *meta = new MultiLine;

   // Get the texture coord.
   Var *texCoord = getInTexCoord( "texCoord", "vec2", true, componentList );

   // search for color var
   Var *material = (Var*) LangElement::find( getOutputTargetVarName(ShaderFeature::RenderTarget2) );
   if ( !material )
   {
      // create color var
      material = new Var;
      material->setType( "vec4" );
      material->setName( getOutputTargetVarName(ShaderFeature::RenderTarget2) );
      material->setStructName("OUT");
   }

   // create texture var
   Var *translucencyMap = new Var;
   translucencyMap->setType( "sampler2D" );
   translucencyMap->setName( "translucencyMap" );
   translucencyMap->uniform = true;
   translucencyMap->sampler = true;
   translucencyMap->constNum = Var::getTexUnitNum();

   meta->addStatement(new GenOp("   @.g = dot(tex2D(@, @).rgb, vec3(0.3, 0.59, 0.11));\r\n", material, translucencyMap, texCoord));
   output = meta;
   
}

ShaderFeature::Resources DeferredTranslucencyMapGLSL::getResources( const MaterialFeatureData &fd )
{
   Resources res; 
   res.numTex = 1;
   res.numTexReg = 1;

   return res;
}

void DeferredTranslucencyMapGLSL::setTexData(   Material::StageData &stageDat,
                                       const MaterialFeatureData &fd,
                                       RenderPassData &passData,
                                       U32 &texIndex )
{
   GFXTextureObject *tex = stageDat.getTex( MFT_TranslucencyMap );
   if ( tex )
   {
      passData.mTexType[ texIndex ] = Material::Standard;
      passData.mSamplerNames[ texIndex ] = "translucencyMap";
      passData.mTexSlot[ texIndex++ ].texObject = tex;
   }
}

// Tranlucency -> Green of Material Info Buffer.
void DeferredTranslucencyEmptyGLSL::processPix( Vector<ShaderComponent*> &componentList, const MaterialFeatureData &fd )
{
   // search for material var
   Var *material = (Var*) LangElement::find( getOutputTargetVarName(ShaderFeature::RenderTarget2) );
   if ( !material )
   {
      // create color var
      material = new Var;
      material->setType( "vec4" );
      material->setName( getOutputTargetVarName(ShaderFeature::RenderTarget2) );
   }
   output = new GenOp( "   @.g = 0.0;\r\n", material );
   
}