xof 0302txt 0064
template Header {
 <3D82AB43-62DA-11cf-AB39-0020AF71E433>
 WORD major;
 WORD minor;
 DWORD flags;
}

template Vector {
 <3D82AB5E-62DA-11cf-AB39-0020AF71E433>
 FLOAT x;
 FLOAT y;
 FLOAT z;
}

template Coords2d {
 <F6F23F44-7686-11cf-8F52-0040333594A3>
 FLOAT u;
 FLOAT v;
}

template Matrix4x4 {
 <F6F23F45-7686-11cf-8F52-0040333594A3>
 array FLOAT matrix[16];
}

template ColorRGBA {
 <35FF44E0-6C7C-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
 FLOAT alpha;
}

template ColorRGB {
 <D3E16E81-7835-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
}

template IndexedColor {
 <1630B820-7842-11cf-8F52-0040333594A3>
 DWORD index;
 ColorRGBA indexColor;
}

template Boolean {
 <4885AE61-78E8-11cf-8F52-0040333594A3>
 WORD truefalse;
}

template Boolean2d {
 <4885AE63-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template MaterialWrap {
 <4885AE60-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template TextureFilename {
 <A42790E1-7810-11cf-8F52-0040333594A3>
 STRING filename;
}

template Material {
 <3D82AB4D-62DA-11cf-AB39-0020AF71E433>
 ColorRGBA faceColor;
 FLOAT power;
 ColorRGB specularColor;
 ColorRGB emissiveColor;
 [...]
}

template MeshFace {
 <3D82AB5F-62DA-11cf-AB39-0020AF71E433>
 DWORD nFaceVertexIndices;
 array DWORD faceVertexIndices[nFaceVertexIndices];
}

template MeshFaceWraps {
 <4885AE62-78E8-11cf-8F52-0040333594A3>
 DWORD nFaceWrapValues;
 Boolean2d faceWrapValues;
}

template MeshTextureCoords {
 <F6F23F40-7686-11cf-8F52-0040333594A3>
 DWORD nTextureCoords;
 array Coords2d textureCoords[nTextureCoords];
}

template MeshMaterialList {
 <F6F23F42-7686-11cf-8F52-0040333594A3>
 DWORD nMaterials;
 DWORD nFaceIndexes;
 array DWORD faceIndexes[nFaceIndexes];
 [Material]
}

template MeshNormals {
 <F6F23F43-7686-11cf-8F52-0040333594A3>
 DWORD nNormals;
 array Vector normals[nNormals];
 DWORD nFaceNormals;
 array MeshFace faceNormals[nFaceNormals];
}

template MeshVertexColors {
 <1630B821-7842-11cf-8F52-0040333594A3>
 DWORD nVertexColors;
 array IndexedColor vertexColors[nVertexColors];
}

template Mesh {
 <3D82AB44-62DA-11cf-AB39-0020AF71E433>
 DWORD nVertices;
 array Vector vertices[nVertices];
 DWORD nFaces;
 array MeshFace faces[nFaces];
 [...]
}

Header{
1;
0;
1;
}

Mesh {
 28;
 0.53033;6.00000;-0.53033;,
 0.53033;0.00000;-0.53033;,
 0.00000;0.00000;-0.75000;,
 0.00000;6.00000;-0.75000;,
 0.75000;6.00000;0.00000;,
 0.75000;0.00000;0.00000;,
 0.53033;6.00000;0.53033;,
 0.53033;0.00000;0.53033;,
 0.00000;6.00000;0.75000;,
 0.00000;0.00000;0.75000;,
 -0.53033;6.00000;0.53033;,
 -0.53033;0.00000;0.53033;,
 -0.75000;6.00000;0.00000;,
 -0.75000;0.00000;0.00000;,
 -0.53033;6.00000;-0.53033;,
 -0.53033;0.00000;-0.53033;,
 -0.53033;0.00000;-0.53033;,
 -0.53033;6.00000;-0.53033;,
 0.00000;4.82049;-3.00000;,
 0.00000;10.82049;0.00000;,
 2.12132;4.82049;-2.12132;,
 3.00000;4.82049;0.00000;,
 2.12132;4.82049;2.12132;,
 0.00000;4.82049;3.00000;,
 -2.12132;4.82049;2.12132;,
 -3.00000;4.82049;0.00000;,
 -2.12132;4.82049;-2.12132;,
 0.00000;4.82049;0.00000;;
 
 32;
 3;0,1,2;,
 3;0,2,3;,
 3;4,5,1;,
 3;4,1,0;,
 3;6,7,5;,
 3;6,5,4;,
 3;8,9,7;,
 3;8,7,6;,
 3;10,11,9;,
 3;10,9,8;,
 3;12,13,11;,
 3;12,11,10;,
 3;14,15,13;,
 3;14,13,12;,
 3;3,2,16;,
 3;3,16,17;,
 3;18,19,20;,
 3;20,19,21;,
 3;21,19,22;,
 3;22,19,23;,
 3;23,19,24;,
 3;24,19,25;,
 3;25,19,26;,
 3;26,19,18;,
 3;27,18,20;,
 3;27,20,21;,
 3;27,21,22;,
 3;27,22,23;,
 3;27,23,24;,
 3;27,24,25;,
 3;27,25,26;,
 3;27,26,18;;
 
 MeshMaterialList {
  2;
  32;
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1;;
  Material {
   0.800000;0.800000;0.800000;1.000000;;
   5.000000;
   0.000000;0.000000;0.000000;;
   0.000000;0.000000;0.000000;;
   TextureFilename {
    "textures\\wood.jpg";
   }
  }
  Material {
   0.800000;0.800000;0.800000;1.000000;;
   5.000000;
   0.000000;0.000000;0.000000;;
   0.000000;0.000000;0.000000;;
   TextureFilename {
    "textures\\leaf.png";
   }
  }
 }
 MeshNormals {
  25;
  -0.136773;0.000000;-0.990602;,
  0.603747;0.000000;-0.797176;,
  0.990602;0.000000;-0.136773;,
  0.797176;0.000000;0.603747;,
  0.136773;0.000000;0.990602;,
  -0.603747;0.000000;0.797176;,
  -0.990602;0.000000;0.136773;,
  -0.797176;0.000000;-0.603747;,
  0.136773;0.000000;-0.990602;,
  0.797176;0.000000;-0.603747;,
  0.990602;0.000000;0.136773;,
  0.603747;0.000000;0.797176;,
  -0.136773;0.000000;0.990602;,
  -0.797176;0.000000;0.603747;,
  -0.990602;0.000000;-0.136773;,
  -0.603747;0.000000;-0.797176;,
  0.000000;0.447214;-0.894427;,
  0.632455;0.447214;-0.632455;,
  0.894427;0.447214;0.000000;,
  0.632455;0.447214;0.632455;,
  0.000000;0.447214;0.894427;,
  -0.632455;0.447214;0.632455;,
  -0.894427;0.447214;0.000000;,
  -0.632455;0.447214;-0.632455;,
  0.000000;-1.000000;-0.000000;;
  32;
  3;1,9,8;,
  3;1,8,0;,
  3;2,10,9;,
  3;2,9,1;,
  3;3,11,10;,
  3;3,10,2;,
  3;4,12,11;,
  3;4,11,3;,
  3;5,13,12;,
  3;5,12,4;,
  3;6,14,13;,
  3;6,13,5;,
  3;7,15,14;,
  3;7,14,6;,
  3;0,8,15;,
  3;0,15,7;,
  3;16,17,17;,
  3;17,17,18;,
  3;18,19,19;,
  3;19,19,20;,
  3;20,21,21;,
  3;21,21,22;,
  3;22,23,23;,
  3;23,23,16;,
  3;24,24,24;,
  3;24,24,24;,
  3;24,24,24;,
  3;24,24,24;,
  3;24,24,24;,
  3;24,24,24;,
  3;24,24,24;,
  3;24,24,24;;
 }
 MeshTextureCoords {
  28;
  0.125000;0.000000;,
  0.125000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  0.250000;0.000000;,
  0.250000;1.000000;,
  0.375000;0.000000;,
  0.375000;1.000000;,
  0.500000;0.000000;,
  0.500000;1.000000;,
  0.625000;0.000000;,
  0.625000;1.000000;,
  0.750000;0.000000;,
  0.750000;1.000000;,
  0.875000;0.000000;,
  0.875000;1.000000;,
  -0.125000;1.000000;,
  -0.125000;0.000000;,
  0.500000;-0.056170;,
  0.500000;-10.468496;,
  6.047678;-0.056170;,
  8.345598;-0.056170;,
  6.047678;-0.056170;,
  0.500000;-0.056170;,
  -5.047678;-0.056170;,
  -7.345598;-0.056170;,
  -5.047678;-0.056170;,
  0.500000;-0.056170;;
 }
 MeshVertexColors {
  28;
  0;1.000000;1.000000;1.000000;1.000000;,
  1;1.000000;1.000000;1.000000;1.000000;,
  2;1.000000;1.000000;1.000000;1.000000;,
  3;1.000000;1.000000;1.000000;1.000000;,
  4;1.000000;1.000000;1.000000;1.000000;,
  5;1.000000;1.000000;1.000000;1.000000;,
  6;1.000000;1.000000;1.000000;1.000000;,
  7;1.000000;1.000000;1.000000;1.000000;,
  8;1.000000;1.000000;1.000000;1.000000;,
  9;1.000000;1.000000;1.000000;1.000000;,
  10;1.000000;1.000000;1.000000;1.000000;,
  11;1.000000;1.000000;1.000000;1.000000;,
  12;1.000000;1.000000;1.000000;1.000000;,
  13;1.000000;1.000000;1.000000;1.000000;,
  14;1.000000;1.000000;1.000000;1.000000;,
  15;1.000000;1.000000;1.000000;1.000000;,
  16;1.000000;1.000000;1.000000;1.000000;,
  17;1.000000;1.000000;1.000000;1.000000;,
  18;1.000000;1.000000;1.000000;1.000000;,
  19;1.000000;1.000000;1.000000;1.000000;,
  20;1.000000;1.000000;1.000000;1.000000;,
  21;1.000000;1.000000;1.000000;1.000000;,
  22;1.000000;1.000000;1.000000;1.000000;,
  23;1.000000;1.000000;1.000000;1.000000;,
  24;1.000000;1.000000;1.000000;1.000000;,
  25;1.000000;1.000000;1.000000;1.000000;,
  26;1.000000;1.000000;1.000000;1.000000;,
  27;1.000000;1.000000;1.000000;1.000000;;
 }
}
