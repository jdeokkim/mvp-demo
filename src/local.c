/*
    Copyright (c) 2024 Jaedeok Kim <jdeokkim@protonmail.com>
    Copyright (c) 2024 Minhu Lee <fghkk12d@gmail.com>

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
        .x = -0.5f, 
        .y = 3.25f, 
        .z = 3.5f 
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

    // 렌더 텍스처 (프레임버퍼) 초기화
    BeginTextureMode(renderTexture);

    {
        ClearBackground(BLACK);

        {
            BeginMode3D(camera);

            DrawAxes();

            DrawGameObject(GetGameObject(OBJ_TYPE_PLAYER), MVP_RENDER_LOCAL);

            DrawInfiniteGrid(&camera);

            EndMode3D();
        }

        // NOTE: 알파 값이 높은 (불투명한) 물체일수록 먼저 그려야 함
        DrawRectangleRec((Rectangle) { .x = 0.0f,
                                       .y = 0.0f,
                                       .width = renderTexture.texture.width,
                                       .height = renderTexture.texture.height },
                         ColorAlpha(RED, 0.07f));

        DrawFPS(8, 8);
    }

    // 기본 프레임버퍼 상태로 되돌아가기
    EndTextureMode();
}

/* "물체 공간"에 필요한 메모리 공간을 해제하는 함수 */
void DeinitLocalSpace(void) {
    // TODO: ...
}