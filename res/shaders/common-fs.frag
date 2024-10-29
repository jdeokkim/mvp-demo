/*
    Copyright (c) 2024 Jaedeok Kim <jdeokkim@protonmail.com>

    Copyright (c) 2024 Hyungki Kim <diskhkme@gmail.com>

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

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec4 fragColor;

/* [variables assigned by raylib] =========================================> */

/* Uniforms (Global Variables) ============================================= */

/* <========================================= [variables assigned by raylib] */

uniform sampler2D texture0;
uniform vec4 colDiffuse;

/* [variables assigned by raylib] =========================================> */

uniform vec3 cameraPosition;

uniform float spacing;
uniform float thick;

uniform int slices;

/* Output Variables ======================================================== */

/* <========================================= [variables assigned by raylib] */

out vec4 finalColor;

/* [variables assigned by raylib] =========================================> */

/* Constants ==============================================================> */

const vec4 innerColor = vec4(0.22, 0.22, 0.22, 1.0);
const vec4 outerColor = vec4(0.38, 0.38, 0.38, 1.0);

const vec4 redColor = vec4(0.90, 0.16, 0.21, 1.0);
const vec4 blueColor = vec4(0.0, 0.47, 0.94, 1.0);

/* Private Variables ======================================================> */

float halfThick = 0.5 * thick;

float minFadeDistance = 0.01 * slices;
float maxFadeDistance = 0.35 * slices;

float heightToFadeDistanceRatio = 35.0;

/* GLSL Functions ========================================================== */

float ComputeAlpha() {
    float cameraDistance = length(cameraPosition.xz - fragPosition.xz);
    float fadeDistance = heightToFadeDistanceRatio * abs(cameraPosition.y);

    fadeDistance = clamp(fadeDistance, minFadeDistance, maxFadeDistance);

    return smoothstep(1.0, 0.0, cameraDistance / fadeDistance);
}

void main() {
    /* REF: https://dev.to/javiersalcedopuyo/simple-infinite-grid-shader-5fah */

    vec4 texelColor = texture(texture0, fragTexCoord);

    float cellX = fract(fragPosition.x / spacing);
    float cellZ = fract(fragPosition.z / spacing);

    finalColor = innerColor;

    if ((fragPosition.x < halfThick) && (fragPosition.x > -halfThick))
        finalColor = blueColor;
    else if ((fragPosition.z < halfThick) && (fragPosition.z > -halfThick))
        finalColor = redColor;
    else if (cellX < halfThick || cellX > (spacing - halfThick)
        || cellZ < halfThick || cellZ > (spacing - halfThick))
        finalColor = outerColor;

    finalColor.w = ComputeAlpha();
}
