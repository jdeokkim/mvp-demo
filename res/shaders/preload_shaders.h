/*
    Copyright (c) 2024 Jaedeok Kim <jdeokkim@protonmail.com>

    Permission is hereby granted, free of charge, to any person obtaining a 
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation 
    the rights to use, copy, modify, merge, publish, distribute, sublicense, 
    and/or sell copies of the Software, and to permit persons to whom the 
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included 
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
    DEALINGS IN THE SOFTWARE.
*/

/* REF: https://dev.to/javiersalcedopuyo/simple-infinite-grid-shader-5fah */

/* 정점 셰이더 코드 */
const char resShadersCommonVsVert[] = ""

#ifdef PLATFORM_WEB
"#version 300 es\n"
"\n"
"precision mediump float;\n"
"precision mediump int;\n"
#else
"#version 330\n"
#endif

"\n"
"/* Input Variables ===================================================== */\n"
"\n"
"/* <===================================== [variables assigned by raylib] */\n"
"\n"
"in vec3 vertexPosition;\n"
"in vec2 vertexTexCoord;\n"
"in vec3 vertexNormal;\n"
"in vec4 vertexColor;\n"
"\n"
"/* [variables assigned by raylib] =====================================> */\n"
"\n"
"/* Uniforms (Global Variables) ========================================= */\n"
"\n"
"/* <===================================== [variables assigned by raylib] */\n"
"\n"
"uniform mat4 matModel;\n"
"uniform mat4 matView;\n"
"uniform mat4 matProjection;\n"
"\n"
"uniform mat4 mvp;\n"
"\n"
"/* [variables assigned by raylib] =====================================> */\n"
"\n"
"uniform vec3 cameraPosition;\n"
"\n"
"uniform float spacing;\n"
"uniform float thick;\n"
"\n"
"uniform mediump int slices;\n"
"\n"
"/* Output Variables ==================================================== */\n"
"\n"
"/* <===================================== [variables assigned by raylib] */\n"
"\n"
"out vec3 fragPosition;\n"
"out vec2 fragTexCoord;\n"
"out vec4 fragColor;\n"
"\n"
"/* [variables assigned by raylib] =====================================> */\n"
"\n"
"/* Private Variables ==================================================> */\n"
"\n"
"vec3 planeVertexPositions[] = vec3[](\n"
"    vec3(-0.5,  0.0,  0.5),\n"
"    vec3( 0.5,  0.0,  0.5),\n"
"    vec3( 0.5,  0.0, -0.5),\n"
"    vec3(-0.5,  0.0, -0.5)\n"
");\n"
"\n"
"/* GLSL Functions ====================================================== */\n"
"\n"
"void main() {\n"
"    vec3 planeVertexPosition = planeVertexPositions[gl_VertexID];\n"
"    vec3 newVertexPosition = (planeVertexPosition * spacing)"
"       * float(slices);\n"
"\n"
"    newVertexPosition.xz += cameraPosition.xz;\n"
"\n"
"    fragPosition = newVertexPosition;\n"
"    // fragTexCoord = vertexTexCoord;\n"
"    // fragColor = vertexColor;\n"
"\n"
"    gl_Position = mvp * vec4(newVertexPosition, 1.0);\n"
"}\n";

/* 프래그먼트 셰이더 코드 */
const char resShadersCommonFsFrag[] = ""

#ifdef PLATFORM_WEB
"#version 300 es\n"
"\n"
"precision mediump float;\n"
"precision mediump int;\n"
#else
"#version 330\n"
#endif

"\n"
"/* Input Variables ===================================================== */\n"
"\n"
"/* <===================================== [variables assigned by raylib] */\n"
"\n"
"in vec3 fragPosition;\n"
"in vec2 fragTexCoord;\n"
"in vec4 fragColor;\n"
"\n"
"/* [variables assigned by raylib] ====================================> */ \n"
"\n"
"/* Uniforms (Global Variables) ======================================== */ \n"
"\n"
"/* <===================================== [variables assigned by raylib] */\n"
"\n"
"uniform sampler2D texture0;\n"
"uniform vec4 colDiffuse;\n"
"\n"
"/* [variables assigned by raylib] =====================================> */\n"
"\n"
"uniform vec3 cameraPosition;\n"
"\n"
"uniform float spacing;\n"
"uniform float thick;\n"
"\n"
"uniform int slices;\n"
"\n"
"/* Output Variables ==================================================== */\n"
"\n"
"/* <===================================== [variables assigned by raylib] */\n"
"\n"
"out vec4 finalColor;\n"
"\n"
"/* [variables assigned by raylib] =====================================> */\n"
"\n"
"/* Constants ==========================================================> */\n"
"\n"
"const vec3 innerColor = vec3(0.22, 0.22, 0.22);\n"
"const vec3 outerColor = vec3(0.38, 0.38, 0.38);\n"
"\n"
"const vec3 redColor = vec3(0.90, 0.16, 0.21);\n"
"const vec3 blueColor = vec3(0.0, 0.47, 0.94);\n"
"\n"
"const float heightToFadeDistanceRatio = 36.0;\n"
"\n"
"/* GLSL Functions ====================================================== */\n"
"\n"
"float ComputeAlpha() {\n"
"    float cameraDistance = distance(cameraPosition.xz, fragPosition.xz);\n"
"    float fadeDistance = heightToFadeDistanceRatio * abs(cameraPosition.y);\n"
"\n"
"    float minFadeDistance = 0.01 * float(slices);\n"
"    float maxFadeDistance = 0.32 * float(slices);\n"
"\n"
"    fadeDistance = clamp(fadeDistance, minFadeDistance, maxFadeDistance);\n"
"\n"
"    return 0.9 - smoothstep(0.0, 0.9, cameraDistance / fadeDistance);\n"
"}\n"
"\n"
"void main() {\n"
"    vec4 texelColor = texture(texture0, fragTexCoord);\n"
"\n"
"    float inverseSpacing = 1.0 / spacing;\n"
"\n"
"    float cellX = fract(fragPosition.x * inverseSpacing);\n"
"    float cellZ = fract(fragPosition.z * inverseSpacing);\n"
"\n"
"    float halfThick = 0.5 * thick;\n"
"\n"
"    finalColor.xyz = innerColor;\n"
"\n"
"   if ((fragPosition.x < halfThick) && (fragPosition.x > -halfThick))\n"
"       finalColor.xyz = blueColor;\n"
"   else if ((fragPosition.z < halfThick) && (fragPosition.z > -halfThick))\n"
"       finalColor.xyz = redColor;\n"
"   else if (cellX < halfThick || cellX > (spacing - halfThick)\n"
"       || cellZ < halfThick || cellZ > (spacing - halfThick))\n"
"       finalColor.xyz = outerColor;\n"
"\n"
"   finalColor.w = ComputeAlpha();\n"
"}\n";
