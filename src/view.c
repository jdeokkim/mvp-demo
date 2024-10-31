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

/* 관찰자 시점을 위한 카메라 */
static Camera3D camera = {
    /* "EYE" */
    .position = { 
        .x = 1.5f, 
        .y = 2.0f, 
        .z = 5.5f 
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

/* "카메라 (뷰) 공간"을 초기화하는 함수 */
void InitViewSpace(void) {
    // TODO: ...
}

/* 프레임버퍼에 "카메라 (뷰) 공간"을 그리는 함수 */
void UpdateViewSpace(RenderTexture renderTexture) {
    /* TODO: ... */

    // 렌더 텍스처 (프레임버퍼) 초기화
    BeginTextureMode(renderTexture);

    {
        ClearBackground(WHITE);

        {
            BeginMode3D(camera);

            DrawInfiniteGrid(&camera);

            DrawAxes(&camera);

            // TODO: ...

            EndMode3D();
        }

        DrawRectangleRec(
            (Rectangle) {
                .width = renderTexture.texture.width,
                .height = renderTexture.texture.height
            },
            ColorAlpha(GREEN, 0.05f)
        );

        DrawFPS(8, 8);
    }

    // 기본 프레임버퍼 상태로 되돌아가기
    EndTextureMode();
}

/* "카메라 (뷰) 공간"에 필요한 메모리 공간을 해제하는 함수 */
void DeinitViewSpace(void) {
    // TODO: ...
}