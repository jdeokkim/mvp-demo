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

#include "shaders/preload_shaders.h"

/* Public Functions ======================================================== */

/* 화살표를 그리는 함수 */
void DrawArrow(Vector3 startPos, Vector3 endPos, Color color) {
    DrawCylinderEx(startPos, endPos, 0.015f, 0.015f, 16, color);

    Vector3 arrowVector = Vector3Subtract(endPos, startPos);

    float length = Vector3Length(arrowVector);

    // 화살표 머리의 시작 지점 (끝 지점은 `endPos`)
    Vector3 midPos = Vector3Add(startPos,
                                Vector3Scale(Vector3Normalize(arrowVector),
                                             0.95f * length));

    DrawCylinderEx(midPos, endPos, 0.11f, 0.015f, 16, color);
}

/* X축, Y축과 Z축을 그리는 함수 */
void DrawAxes(void) {
    DrawAxesEx(Vector3Zero(),
               (Vector3) { .x = 1.0f },
               (Vector3) { .y = 1.0f },
               (Vector3) { .z = 1.0f },
               RED,
               GREEN,
               BLUE);
}

/* 세 축 `axis1`, `axis2`와 `axis3`를 `position`에 그리는 함수 */
void DrawAxesEx(Vector3 position,
                Vector3 axis1,
                Vector3 axis2,
                Vector3 axis3,
                Color color1,
                Color color2,
                Color color3) {
    DrawCylinderEx(
        position, Vector3Add(position, axis1), 0.03f, 0.03f, 32, color1);
    DrawCylinderEx(
        position, Vector3Add(position, axis2), 0.03f, 0.03f, 32, color2);
    DrawCylinderEx(
        position, Vector3Add(position, axis3), 0.03f, 0.03f, 32, color3);

    DrawSphere(position, 0.08f, ColorBrightness(BLACK, 0.15f));
}

/* 게임 세계의 물체를 그리는 함수 */
void DrawGameObject(GameObject *gameObject, MvpRenderMode renderMode) {
    if (gameObject == NULL || renderMode == MVP_RENDER_ALL
        || renderMode == MVP_RENDER_COUNT_)
        return;

    Model *model = &(gameObject->model);

    Matrix tmpMatModel = model->transform;

    // "물체 공간"에서는 모든 물체의 모델 행렬을 초기화
    if (renderMode == MVP_RENDER_LOCAL) model->transform = MatrixIdentity();

    Vector3 virtualCameraEye = GetVirtualCamera()->position;
    Vector3 virtualCameraAt = GetVirtualCamera()->target;

    // "뷰 공간"에서는 카메라가 원점에 오도록 모든 물체의 모델 행렬을 변환
    if (renderMode == MVP_RENDER_VIEW)
        model->transform = MatrixMultiply(model->transform,
                                          GetVirtualCameraViewMat());

    // NOTE: raylib의 `DrawModelEx()` 구현부에서 모델 행렬 계산하는 부분 삭제하고 가져옴
    for (int i = 0; i < model->meshCount; i++) {
        Color color = model->materials[model->meshMaterial[i]]
                          .maps[MATERIAL_MAP_DIFFUSE]
                          .color;

        model->materials[model->meshMaterial[i]]
            .maps[MATERIAL_MAP_DIFFUSE]
            .color = WHITE;

        DrawMesh(model->meshes[i],
                 model->materials[model->meshMaterial[i]],
                 model->transform);

        model->materials[model->meshMaterial[i]]
            .maps[MATERIAL_MAP_DIFFUSE]
            .color = color;
    }

    if (gameObject == GetGameObject(OBJ_TYPE_PLAYER)) {
        // 모델의 정점 좌표 표시하기
        // TODO: ...
    } else if (gameObject == GetGameObject(OBJ_TYPE_CAMERA)) {
        if (renderMode == MVP_RENDER_WORLD) {
            Matrix viewMat = GetVirtualCameraViewMat();

            // 가상 카메라의 U축, V축과 N축 그리기
            DrawAxesEx(virtualCameraEye,
                       (Vector3) {
                           .x = viewMat.m0, .y = viewMat.m4, .z = viewMat.m8 },
                       (Vector3) {
                           .x = viewMat.m1, .y = viewMat.m5, .z = viewMat.m9 },
                       (Vector3) {
                           .x = viewMat.m2, .y = viewMat.m6, .z = viewMat.m10 },
                       ColorBrightness(RED, -0.5f),
                       ColorBrightness(GREEN, -0.5f),
                       ColorBrightness(BLUE, -0.5f));

            // "EYE"에서 "AT"으로 향하는 화살표 그리기
            DrawArrow(virtualCameraEye,
                      virtualCameraAt,
                      ColorBrightness(YELLOW, -0.1f));
        }

        // View Frustum 그리기
        DrawViewFrustum(renderMode, PINK);
    }

    model->transform = tmpMatModel;
}

/* 관찰자 시점 카메라의 입력 잠금 여부를 그리는 함수 */
void DrawHelpText(RenderTexture renderTexture, bool isCameraLocked) {
    const char *cameraLockHelpText = TextFormat("Camera: %s (Press 'ESC')",
                                                (isCameraLocked ? "Locked"
                                                                : "Unlocked"));

    Vector2 cameraLockHelpTextSize = MeasureTextEx(GetGuiDefaultFont(),
                                                   cameraLockHelpText,
                                                   GetGuiDefaultFont().baseSize,
                                                   0.0f);

    DrawTextEx(GetGuiDefaultFont(),
               cameraLockHelpText,
               (Vector2) { .x = renderTexture.texture.width
                                - (cameraLockHelpTextSize.x + 8.0f),
                           .y = renderTexture.texture.height
                                - (cameraLockHelpTextSize.y + 8.0f) },
               (GetGuiDefaultFont().baseSize),
               0.0f,
               ColorBrightness(BLACK, (isCameraLocked ? 0.2f : 0.35f)));
}

/* 공용 셰이더 프로그램으로 XZ 평면에 격자 무늬를 그리는 함수 */
void DrawInfiniteGrid(const Camera *camera) {
    if (camera == NULL) return;

    static int cameraPositionLoc = -1;

    if (cameraPositionLoc < 0) {
        // 셰이더의 Uniform 위치 찾기
        cameraPositionLoc = GetShaderLocation(GetCommonShader(),
                                              "cameraPosition");
    }

    // Y 좌표가 0 이하일 때도 격자 무늬 그리기
    rlDisableBackfaceCulling();

    {
        BeginShaderMode(GetCommonShader());

        // 셰이더의 Uniform 값을 카메라의 "EYE" 좌표로 설정
        SetShaderValue(GetCommonShader(),
                       cameraPositionLoc,
                       &camera->position,
                       SHADER_UNIFORM_VEC3);

        // 격자 무늬는 셰이더가 알아서 다 그려줌
        DrawRectangleRec((Rectangle) { .x = 0.0f,
                                       .y = 0.0f,
                                       .width = SCREEN_WIDTH,
                                       .height = SCREEN_HEIGHT },
                         WHITE);

        EndShaderMode();
    }

    rlEnableBackfaceCulling();
}

/* 가상 카메라의 View Frustum을 그리는 함수 */
void DrawViewFrustum(MvpRenderMode renderMode, Color color) {
    if (renderMode != MVP_RENDER_VIEW) return;

    Camera virtualCamera = *(GetVirtualCamera());

    Vector3 virtualCameraEye = Vector3Zero();

    // "뷰 공간"에서는 가상 카메라의 U축, V축과 N축이 각각 X축, Y축과 Z축이 됨
    Vector3 xAxis = { .x = 1.0f };
    Vector3 yAxis = { .y = 1.0f };
    Vector3 zAxis = { .z = 1.0f };

    float nearDistance = GetViewFrustumNearDistance();
    float farDistance = GetViewFrustumFarDistance();

    float halfAngleInRadians = (0.5f * virtualCamera.fovy) * DEG2RAD;

    // "Near Plane"의 세로 길이의 절반
    float nearPlaneHalfHeight = nearDistance * tanf(halfAngleInRadians);

    // "Near Plane"의 가로 길이의 절반
    float nearPlaneHalfWidth = nearPlaneHalfHeight * GetViewFrustumAspect();

    // "Near Plane"의 중심점
    Vector3 nearPlaneCenter = Vector3Add(virtualCameraEye,
                                         Vector3Scale(Vector3Negate(zAxis),
                                                      nearDistance));

    // "Far Plane"의 세로 길이의 절반
    float farPlaneHalfHeight = farDistance * tanf(halfAngleInRadians);

    // "Far Plane"의 가로 길이의 절반
    float farPlaneHalfWidth = farPlaneHalfHeight * GetViewFrustumAspect();

    // "Far Plane"의 중심점
    Vector3 farPlaneCenter = Vector3Add(virtualCameraEye,
                                        Vector3Scale(Vector3Negate(zAxis),
                                                     farDistance));

    {
        rlDisableBackfaceCulling();

        Vector3 nearPlaneLRTB[] = {
            Vector3Add(nearPlaneCenter,
                       Vector3Scale(xAxis, -nearPlaneHalfWidth)),
            Vector3Add(nearPlaneCenter,
                       Vector3Scale(xAxis, nearPlaneHalfWidth)),
            Vector3Add(nearPlaneCenter,
                       Vector3Scale(yAxis, nearPlaneHalfHeight)),
            Vector3Add(nearPlaneCenter,
                       Vector3Scale(yAxis, -nearPlaneHalfHeight))
        };

        Vector3 farPlaneLRTB[] = {
            Vector3Add(farPlaneCenter, Vector3Scale(xAxis, -farPlaneHalfWidth)),
            Vector3Add(farPlaneCenter, Vector3Scale(xAxis, farPlaneHalfWidth)),
            Vector3Add(farPlaneCenter, Vector3Scale(yAxis, farPlaneHalfHeight)),
            Vector3Add(farPlaneCenter, Vector3Scale(yAxis, -farPlaneHalfHeight))
        };

        /* "Near Plane" 그리기 */

        // "Near Plane"의 정점 좌표
        Vector3 nearPlaneVertices[] = {
            Vector3Add(nearPlaneLRTB[0], nearPlaneLRTB[2]),  // LT
            Vector3Add(nearPlaneLRTB[0], nearPlaneLRTB[3]),  // LB
            Vector3Add(nearPlaneLRTB[1], nearPlaneLRTB[3]),  // RB
            Vector3Add(nearPlaneLRTB[1], nearPlaneLRTB[2])   // RT
        };

        int nearPlaneVertexCount = sizeof nearPlaneVertices
                                   / sizeof *nearPlaneVertices;

        for (int i = nearPlaneVertexCount - 1, j = 0; j < nearPlaneVertexCount;
             i = j, j++)
            DrawLine3D(nearPlaneVertices[i], nearPlaneVertices[j], color);

        /* "Far Plane" 그리기 */

        // "Far Plane"의 정점 좌표
        Vector3 farPlaneVertices[] = {
            Vector3Add(farPlaneLRTB[0], farPlaneLRTB[2]),  // LT
            Vector3Add(farPlaneLRTB[0], farPlaneLRTB[3]),  // LB
            Vector3Add(farPlaneLRTB[1], farPlaneLRTB[3]),  // RB
            Vector3Add(farPlaneLRTB[1], farPlaneLRTB[2])   // RT
        };

        int farPlaneVertexCount = sizeof farPlaneVertices
                                  / sizeof *farPlaneVertices;

        for (int i = farPlaneVertexCount - 1, j = 0; j < farPlaneVertexCount;
             i = j, j++)
            DrawLine3D(farPlaneVertices[i], farPlaneVertices[j], color);

        /* "Near Plane"과 "Far Plane"을 잇는 선분 그리기 */
        for (int i = 0; i < nearPlaneVertexCount; i++)
            DrawLine3D(nearPlaneVertices[i], farPlaneVertices[i], color);

        DrawArrow(virtualCameraEye,
                      farPlaneCenter,
                      ColorBrightness(YELLOW, -0.1f));

        rlEnableBackfaceCulling();
    }
}

/* 공용 셰이더 프로그램을 반환하는 함수 */
Shader LoadCommonShader(void) {
    // 셰이더 소스 파일을 컴파일 및 링크하여 셰이더 프로그램 생성
    return LoadShaderFromMemory(resShadersCommonVsVert, resShadersCommonFsFrag);
}