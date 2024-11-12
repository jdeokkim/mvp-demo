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

/* Compilation Settings ==================================================== */

#version 330

/* Input Variables ========================================================= */

/* <========================================= [variables assigned by raylib] */

in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;

/* [variables assigned by raylib] =========================================> */

/* Uniforms (Global Variables) ============================================= */

/* <========================================= [variables assigned by raylib] */

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;

uniform mat4 mvp;

/* [variables assigned by raylib] =========================================> */

uniform vec3 cameraPosition;

uniform float spacing = 1.0;
uniform float thick = 0.03;

uniform int slices = 1000;

/* Output Variables ======================================================== */

/* <========================================= [variables assigned by raylib] */

out vec3 fragPosition;
out vec2 fragTexCoord;
out vec4 fragColor;

/* [variables assigned by raylib] =========================================> */

/* Constants ==============================================================> */

vec3 planeVertexPositions[] = vec3[](
    vec3(-0.5,  0.0,  0.5),
    vec3( 0.5,  0.0,  0.5),
    vec3( 0.5,  0.0, -0.5),
    vec3(-0.5,  0.0, -0.5)
);

/* GLSL Functions ========================================================== */

void main() {
    /* REF: https://dev.to/javiersalcedopuyo/simple-infinite-grid-shader-5fah */
    
    vec3 planeVertexPosition = planeVertexPositions[gl_VertexID];
    vec3 newVertexPosition = (planeVertexPosition * spacing) * slices;

    newVertexPosition.xz += cameraPosition.xz;

    fragPosition = newVertexPosition;
    // fragTexCoord = vertexTexCoord;
    // fragColor = vertexColor;

    gl_Position = mvp * vec4(newVertexPosition, 1.0);
}