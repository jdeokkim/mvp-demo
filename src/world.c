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
        .y = 3.0f, 
        .z = 8.5f 
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

/* 플레이어의 이동 속도 배수 */
static float speedMultiplier = 1.0f;

/* Private Function Prototypes ============================================= */

/* 마우스 및 키보드 입력을 처리하는 함수 */
static void HandleInputEvents(void);

/* Public Functions ======================================================== */

/* "세계 공간"을 초기화하는 함수 */
void InitWorldSpace(void) {
    // TODO: ...
}

/* 프레임버퍼에 "세계 공간"을 그리는 함수 */
void UpdateWorldSpace(RenderTexture renderTexture) {
    HandleInputEvents();

    // 렌더 텍스처 (프레임버퍼) 초기화
    BeginTextureMode(renderTexture);

    {
        ClearBackground(WHITE);

        {
            BeginMode3D(camera);

            DrawInfiniteGrid(&camera);

            DrawAxes(&camera);

            for (int i = 0; i < GAME_OBJECT_COUNT; i++)
                DrawGameObject(GetGameObject(i), MVP_RENDER_WORLD);

            EndMode3D();
        }

        // NOTE: 알파 값이 높은 (불투명한) 물체일수록 먼저 그려야 함
        DrawRectangleRec((Rectangle) { .width = renderTexture.texture.width,
                                       .height = renderTexture.texture.height },
                         ColorAlpha(ORANGE, 0.06f));

        DrawFPS(8, 8);
    }

    // 기본 프레임버퍼 상태로 되돌아가기
    EndTextureMode();
}

/* "세계 공간"에 필요한 메모리 공간을 해제하는 함수 */
void DeinitWorldSpace(void) {
    // TODO: ...
}

/* Private Functions ======================================================= */

/* 마우스 및 키보드 입력을 처리하는 함수 */
static void HandleInputEvents(void) {
    if (GetMvpRenderMode() != MVP_RENDER_WORLD) return;

    float speed = speedMultiplier * GetFrameTime();

    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT)) {
        GetGameObject(OBJ_TYPE_PLAYER)->model.transform = MatrixMultiply(
            GetGameObject(OBJ_TYPE_PLAYER)->model.transform,
            MatrixTranslate(IsKeyDown(KEY_LEFT) ? -speed : speed, 0.0f, 0.0f)
        );

        UpdateModelMatrix(false);
    }

    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_DOWN)) {
        GetGameObject(OBJ_TYPE_PLAYER)->model.transform = MatrixMultiply(
            GetGameObject(OBJ_TYPE_PLAYER)->model.transform,
            MatrixTranslate(0.0f, 0.0f, IsKeyDown(KEY_UP) ? -speed : speed)
        );

        UpdateModelMatrix(false);
    }

    if (IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_LEFT_SHIFT)) {
        GetGameObject(OBJ_TYPE_PLAYER)->model.transform = MatrixMultiply(
            GetGameObject(OBJ_TYPE_PLAYER)->model.transform,
            MatrixTranslate(0.0f, IsKeyDown(KEY_LEFT_SHIFT) ? -speed : speed, 0.0f)
        );

        UpdateModelMatrix(false);
    }
}