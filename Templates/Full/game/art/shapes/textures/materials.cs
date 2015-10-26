
singleton Material(PBRTEST_PBRMetal)
{
   mapTo = "PBRMetal";
   diffuseColor[0] = "1 1 1 1";
   roughness[0] = "1";
   metalness[0] = "50";
   translucentBlendOp = "None";
   diffuseMap[0] = "art/shapes/textures/Metal_A.png";
   normalMap[0] = "art/shapes/textures/Metal_N.png";
   materialTag0 = "Miscellaneous";
   metalChan[0] = "3";
   specularMap[0] = "art/shapes/textures/Metal_S.png";
   roughness0 = "1";
   pixelSpecular0 = "0";
};

singleton Material(PBRTEST_PBRWood)
{
   mapTo = "PBRWood";
   diffuseColor[0] = "1 1 1 1";
   roughness[0] = "1";
   metalness[0] = "50";
   translucentBlendOp = "None";
   diffuseMap[0] = "art/shapes/textures/Wood1_A.dds";
   normalMap[0] = "art/shapes/textures/Wood1_N.png";
   materialTag0 = "Miscellaneous";
   roughMap[0] = "art/shapes/textures/Wood1_S.dds";
   metalMap[0] = "art/shapes/textures/Wood1_S.dds";
   metalChan[0] = "3";
   roughness0 = "1";
   pixelSpecular0 = "0";
};

singleton Material(PBRTest2Mat)
{
   mapTo = "PBRTest2Mat";
   diffuseColor[0] = "1 1 1 1";
   roughness[0] = "1";
   metalness[0] = "1";
   translucentBlendOp = "None";
   diffuseMap[0] = "art/shapes/textures/T3D_Metal_6_basecolor.dds";
   normalMap[0] = "art/shapes/textures/T3D_Metal_6_normal.png";
   materialTag0 = "Miscellaneous";
   metalChan[0] = "2";
   roughness0 = "1";
   pixelSpecular0 = "0";
   smoothness[0] = "1";
   specularMap[0] = "art/shapes/textures/T3D_Metal_6_any.dds";
};

singleton Material(PBRTest2Mat)
{
   mapTo = "PBRTest2Mat";
   diffuseColor[0] = "1 1 1 1";
   smoothness[0] = "1";
   metalness[0] = "1";
   translucentBlendOp = "None";
   diffuseMap[0] = "art/shapes/textures/T3D_Metal_6_basecolor.png";
   normalMap[0] = "art/shapes/textures/T3D_Metal_6_normal.png";
   pixelSpecular0 = "0";
   materialTag0 = "Miscellaneous";
   specularMap[0] = "art/shapes/textures/T3D_Metal_6_any.png";
};

singleton Material(PBRTEST_PBRstone)
{
   mapTo = "PBRstone";
   diffuseColor[0] = "0.64 0.64 0.64 1";
   smoothness[0] = "1";
   metalness[0] = "1";
   translucentBlendOp = "None";
   diffuseMap[0] = "art/shapes/textures/Stone_A.dds";
   normalMap[0] = "art/shapes/textures/Stone_N.png";
   roughMap[0] = "art/shapes/textures/Stone_S.dds";
   metalMap[0] = "art/shapes/textures/Stone_S.dds";
   roughness0 = "1";
   materialTag0 = "Miscellaneous";
   pixelSpecular0 = "0";
   metalChan[0] = "3";
};