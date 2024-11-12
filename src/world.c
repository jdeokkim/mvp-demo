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

/* 게임 세계 밖의 관찰자 시점을 위한 카메라 */
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

/* 게임 세계에 존재하는 가상 카메라 */
static Camera3D virtualCamera = {
    /* "FOV" */
    .fovy = 60.0f,
    /* "PROJECTION" */
    .projection = CAMERA_PERSPECTIVE 
};

/* clang-format on */

/* 관찰자 시점 카메라의 입력 잠금 여부 */
static bool isCameraLocked = false;

/* Private Function Prototypes ============================================= */

/* 관찰자 시점 카메라의 입력 잠금 여부를 그리는 함수 */
static void DrawHelpText(RenderTexture renderTexture);

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

            DrawAxes();

            for (int i = 0; i < GAME_OBJECT_COUNT; i++)
                DrawGameObject(GetGameObject(i), MVP_RENDER_WORLD);

            DrawInfiniteGrid(&camera);

            EndMode3D();
        }

        // NOTE: 알파 값이 높은 (불투명한) 물체일수록 먼저 그려야 함
        DrawRectangleRec((Rectangle) { .width = renderTexture.texture.width,
                                       .height = renderTexture.texture.height },
                         ColorAlpha(ORANGE, 0.1f));

        DrawHelpText(renderTexture);

        DrawFPS(8, 8);
    }

    // 기본 프레임버퍼 상태로 되돌아가기
    EndTextureMode();
}

/* "세계 공간"에 필요한 메모리 공간을 해제하는 함수 */
void DeinitWorldSpace(void) {
    // TODO: ...
}

/* ========================================================================= */

/* "세계 공간"의 가상 카메라를 반환하는 함수 */
Camera *GetVirtualCamera(void) {
    return &virtualCamera;
}

/* 가상 카메라의 모델 행렬을 반환하는 함수 */
Matrix GetVirtualCameraModelMat(void) {
    return MatrixMultiply(MatrixIdentity(),
                          MatrixTranslate(virtualCamera.position.x,
                                          virtualCamera.position.y,
                                          virtualCamera.position.z));
}

/* 가상 카메라로 만들어지는 뷰 행렬을 반환하는 함수 */
Matrix GetVirtualCameraViewMat(void) {
    return GetCameraMatrix(virtualCamera);
}

/* Private Functions ======================================================= */

/* 관찰자 시점 카메라의 입력 잠금 여부를 그리는 함수 */
static void DrawHelpText(RenderTexture renderTexture) {
    if (GetMvpRenderMode() != MVP_RENDER_WORLD) return;
    
    const char *cameraLockHelpText = TextFormat("Camera: %s (Press 'ESC')",
                                                (isCameraLocked ? "Locked"
                                                                : "Unlocked"));

    Vector2 cameraLockHelpTextSize = MeasureTextEx(GetFontDefault(),
                                                   cameraLockHelpText,
                                                   (GetFontDefault().baseSize
                                                    << 1),
                                                   1.0f);

    DrawTextEx(GetFontDefault(),
               cameraLockHelpText,
               (Vector2) { .x = 8.0f,
                           .y = renderTexture.texture.height
                                - (cameraLockHelpTextSize.y + 8.0f) },
               (GetFontDefault().baseSize << 1),
               1.0f,
               ColorBrightness(ORANGE, (isCameraLocked ? -0.15f : 0.15f)));
}

/* 마우스 및 키보드 입력을 처리하는 함수 */
static void HandleInputEvents(void) {
    if (GetMvpRenderMode() != MVP_RENDER_WORLD) return;

    if (IsKeyPressed(KEY_ESCAPE)) isCameraLocked = !isCameraLocked;

    if (!isCameraLocked) UpdateCamera(&camera, CAMERA_THIRD_PERSON);
}