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
    /* "PROJECTION" */
    .projection = CAMERA_PERSPECTIVE 
};

/* clang-format on */

/* 관찰자 시점 카메라의 입력 잠금 여부 */
static bool isCameraLocked = true;

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

            DrawAxes();

            for (int i = 0; i < GAME_OBJECT_COUNT; i++)
                DrawGameObject(GetGameObject(i),
                               renderTexture,
                               MVP_RENDER_WORLD);

            DrawInfiniteGrid(&camera);

            EndMode3D();
        }

        DrawVertexPositionText(renderTexture, MVP_RENDER_WORLD);

        // NOTE: 알파 값이 높은 (불투명한) 물체일수록 먼저 그려야 함
        DrawRectangleRec((Rectangle) { .width = renderTexture.texture.width,
                                       .height = renderTexture.texture.height },
                         ColorAlpha(ORANGE, 0.1f));

        if (GetMvpRenderMode() == MVP_RENDER_WORLD)
            DrawCameraHelpText(renderTexture, isCameraLocked);

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

/* "세계 공간"의 관찰자 시점 카메라를 반환하는 함수 */
Camera *GetWorldObserverCamera(void) {
    return &camera;
}

/* ========================================================================= */

/* "세계 공간"의 가상 카메라를 반환하는 함수 */
Camera *GetVirtualCamera(void) {
    return &virtualCamera;
}

/* 가상 카메라의 모델 행렬을 반환하는 함수 */
Matrix GetVirtualCameraModelMat(bool readOnly) {
    // 가상 카메라 "뷰 행렬"의 U축을 기준으로 가상 카메라의 모델 회전
    Vector3 axis = GetVirtualCameraUAxis();

    float angle = Vector3Angle((Vector3) { .x = 0.0f, .y = -1.0f, .z = 0.0f },
                               Vector3Subtract(virtualCamera.target,
                                               virtualCamera.position));

    return MatrixMultiply(MatrixRotate(axis, angle),
                          MatrixTranslate(virtualCamera.position.x,
                                          virtualCamera.position.y,
                                          virtualCamera.position.z));
}

/* 가상 카메라에 대한 "뷰 행렬"을 반환하는 함수 */
Matrix GetVirtualCameraViewMat(bool readOnly) {
    return MatrixLookAt(virtualCamera.position,
                        virtualCamera.target,
                        virtualCamera.up);
}

/* 가상 카메라에 대한 "뷰 행렬"의 U축을 반환하는 함수 */
Vector3 GetVirtualCameraUAxis(void) {
    Matrix virtualCameraViewMat = GetVirtualCameraViewMat(true);

    return (Vector3) { .x = virtualCameraViewMat.m0,
                       .y = virtualCameraViewMat.m4,
                       .z = virtualCameraViewMat.m8 };
}

/* 가상 카메라에 대한 "뷰 행렬"의 V축을 반환하는 함수 */
Vector3 GetVirtualCameraVAxis(void) {
    Matrix virtualCameraViewMat = GetVirtualCameraViewMat(true);

    return (Vector3) { .x = virtualCameraViewMat.m1,
                       .y = virtualCameraViewMat.m5,
                       .z = virtualCameraViewMat.m9 };
}

/* 가상 카메라에 대한 "뷰 행렬"의 N축을 반환하는 함수 */
Vector3 GetVirtualCameraNAxis(void) {
    Matrix virtualCameraViewMat = GetVirtualCameraViewMat(true);

    return (Vector3) { .x = virtualCameraViewMat.m2,
                       .y = virtualCameraViewMat.m6,
                       .z = virtualCameraViewMat.m10 };
}

/* Private Functions ======================================================= */

/* 마우스 및 키보드 입력을 처리하는 함수 */
static void HandleInputEvents(void) {
    if (GetMvpRenderMode() != MVP_RENDER_WORLD) return;

    if (IsKeyPressed(KEY_ESCAPE)) isCameraLocked = !isCameraLocked;

    if (!isCameraLocked) UpdateCamera(&camera, CAMERA_THIRD_PERSON);
}