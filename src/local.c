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

/* Includes ================================================================ */

#include "mvp-demo.h"

/* Private Variables ======================================================= */

/* clang-format off */

static Camera3D camera = {
    /* "EYE" */
    .position = { 
        .x = 1.5f, 
        .y = 2.0f, 
        .z = 2.5f 
    },
    /* "AT" */
    .target = { 
        .x = 0.0f, 
        .y = 0.0f, 
        .z = 0.0f 
    },
    /* "UP" */
    .up = { 
        .x = 0.0f, 
        .y = 1.0f, 
        .z = 0.0f 
    },
    /* "FOV" */
    .fovy = 45.0f,
    /* "PROJECTION" */
    .projection = CAMERA_PERSPECTIVE
};

/* clang-format on */

/* Public Functions ======================================================== */

/* "물체 공간"을 초기화하는 함수 */
void InitLocalSpace(void) {
    // TODO: ...
}

/* 프레임버퍼에 "물체 공간"을 그리는 함수 */
void UpdateLocalSpace(RenderTexture renderTexture) {
    { UpdateCamera(&camera, CAMERA_ORBITAL); }

    BeginTextureMode(renderTexture);

    {
        ClearBackground(ColorAlpha(RED, 0.05f));

        {
            BeginMode3D(camera);

            DrawAxes();

            /* TODO: ... */

            EndMode3D();
        }

        DrawFPS(8, 8);
    }

    EndTextureMode();
}

/* "물체 공간"에 필요한 메모리 공간을 해제하는 함수 */
void DeinitLocalSpace(void) {
    // TODO: ...
}