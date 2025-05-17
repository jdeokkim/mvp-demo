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

/* Macro Constants ========================================================= */

// clang-format off

#define GUI_CAMERA_LOCK_HINT_TEXT           "Camera: %s (Press 'Esc')"

#define GUI_CAMERA_LOCKED_TEXT              "Locked"
#define GUI_CAMERA_UNLOCKED_TEXT            "Unlocked"

// clang-format on

/* Constants =============================================================== */

/* 가상 카메라에 대한 View Frustum의 선 두께 */
static const float viewFrustumLineThick = 0.015f;

/* Public Functions ======================================================== */

/* 화살표를 그리는 함수 */
void DrawArrow(Vector3 startPos, Vector3 endPos, Color color) {
    DrawCylinderEx(startPos, endPos, 0.015f, 0.015f, 16, color);

    Vector3 arrowVector = Vector3Subtract(endPos, startPos);

    float length = Vector3Length(arrowVector);

    // 화살표 머리의 시작 지점
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

/* 관찰자 시점 카메라의 입력 잠금 여부를 그리는 함수 */
void DrawCameraHintText(RenderTexture renderTexture, bool isCameraLocked) {
    if (GetMvpRenderMode() < MVP_RENDER_LOCAL
        || GetMvpRenderMode() > MVP_RENDER_VIEW)
        return;

    const char *cameraLockHintText = TextFormat(
        GUI_CAMERA_LOCK_HINT_TEXT,
        (isCameraLocked ? GUI_CAMERA_LOCKED_TEXT : GUI_CAMERA_UNLOCKED_TEXT));

    Vector2 cameraLockHintTextSize = MeasureTextEx(GetGuiDefaultFont(),
                                                   cameraLockHintText,
                                                   GetGuiDefaultFont().baseSize,
                                                   0.0f);

    DrawTextEx(GetGuiDefaultFont(),
               cameraLockHintText,
               (Vector2) { .x = renderTexture.texture.width
                                - (cameraLockHintTextSize.x + 8.0f),
                           .y = renderTexture.texture.height
                                - (cameraLockHintTextSize.y + 8.0f) },
               (GetGuiDefaultFont().baseSize),
               0.0f,
               ColorBrightness(SKYBLUE, (isCameraLocked ? 0.05f : 0.6f)));
}

/* 게임 세계의 물체를 그리는 함수 */
void DrawGameObject(GameObject *gameObject,
                    RenderTexture renderTexture,
                    MvpRenderMode renderMode) {
    if (gameObject == NULL || renderMode == MVP_RENDER_ALL
        || renderMode == MVP_RENDER_COUNT_)
        return;

    Model *model = &(gameObject->model);

    Matrix tmpMatModel = model->transform;

    Vector3 virtualCameraEye = GetVirtualCamera()->position;
    Vector3 virtualCameraAt = GetVirtualCamera()->target;

    Matrix virtualCameraViewMat = GetVirtualCameraViewMat(false);
    Matrix virtualCameraProjMat = GetVirtualCameraProjMat(false);

    if (renderMode == MVP_RENDER_LOCAL) {
        // "물체 공간"에서는 모든 물체의 "모델 행렬"을 초기화
        model->transform = MatrixIdentity();
    } else if (renderMode == MVP_RENDER_VIEW) {
        // 모든 물체의 "세계 공간" 위치를 "뷰 공간" 위치로 변환
        model->transform = MatrixMultiply(model->transform,
                                          virtualCameraViewMat);
    }

    // NOTE: raylib의 `DrawModelEx()`에서 "모델 행렬" 계산하는 부분 삭제하고 가져옴
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
        if (IsVertexVisibilityModeEnabled()) {
            const Camera *camera = GetLocalObserverCamera();

            Matrix txMatrix = MatrixIdentity();

            // "클립 공간"에서는 `BeginMode3D()`를 통해 세계 공간의 좌표를 변환
            if (renderMode == MVP_RENDER_WORLD)
                camera = GetWorldObserverCamera(), txMatrix = tmpMatModel;
            else if (renderMode == MVP_RENDER_VIEW)
                camera = GetViewObserverCamera(),
                txMatrix = MatrixMultiply(tmpMatModel, virtualCameraViewMat);
            else if (renderMode == MVP_RENDER_CLIP)
                camera = GetVirtualCamera(), txMatrix = tmpMatModel;

            for (int i = 0; i < PLAYER_MODEL_VERTEX_COUNT; i++) {
                Vector3 vertexPosition = Vector3Transform(
                    gameObject->vertexData.vertices[i], txMatrix);

                // 모델의 정점 위치 그리기
                DrawSphereEx(vertexPosition,
                             0.05f,
                             8,
                             8,
                             gameObject->vertexData.colors[i]);
            }
        }
    } else if (gameObject == GetGameObject(OBJ_TYPE_CAMERA)) {
        if (renderMode == MVP_RENDER_WORLD) {
            // 가상 카메라의 U축, V축과 N축 그리기
            DrawAxesEx(virtualCameraEye,
                       GetVirtualCameraUAxis(),
                       GetVirtualCameraVAxis(),
                       GetVirtualCameraNAxis(),
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

/* 공용 셰이더 프로그램으로 XZ 평면에 격자 무늬를 그리는 함수 */
void DrawInfiniteGrid(const Camera *camera) {
    if (camera == NULL) return;

    Shader shaderProgram = GetCommonShader();

    static int cameraPositionLoc = -1;

    if (cameraPositionLoc < 0) {
        // 셰이더의 Uniform 위치 찾기
        cameraPositionLoc = GetShaderLocation(shaderProgram, "cameraPosition");
    }

    // Y 좌표가 0 이하일 때도 격자 무늬 그리기
    rlDisableBackfaceCulling();

    {
        BeginShaderMode(shaderProgram);

        // 셰이더의 Uniform 값을 카메라의 "EYE" 좌표로 설정
        SetShaderValue(shaderProgram,
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

    Matrix virtualCameraViewMat = GetVirtualCameraViewMat(false);
    Matrix virtualCameraProjMat = GetVirtualCameraProjMat(false);

    Vector3 virtualCameraEye = Vector3Transform(virtualCamera.position,
                                                virtualCameraViewMat);

    Vector3 xAxis = Vector3Transform(Vector3Add(virtualCamera.position,
                                                GetVirtualCameraUAxis()),
                                     virtualCameraViewMat);
    Vector3 yAxis = Vector3Transform(Vector3Add(virtualCamera.position,
                                                GetVirtualCameraVAxis()),
                                     virtualCameraViewMat);
    Vector3 zAxis = Vector3Transform(Vector3Add(virtualCamera.position,
                                                GetVirtualCameraNAxis()),
                                     virtualCameraViewMat);

    {
        rlDisableBackfaceCulling();

        Matrix virtualCameraInverseProjMat = MatrixInvert(virtualCameraProjMat);

        /* "Near Plane" 그리기 */

        // "Near Plane"의 정점에 대한 동차 좌표
        Vector4 nearPlaneCoords[] = {
            QuaternionTransform(
                (Vector4) { .x = -1.0f, .y = 1.0f, .z = -1.0f, .w = 1.0f },
                virtualCameraInverseProjMat),
            QuaternionTransform(
                (Vector4) { .x = -1.0f, .y = -1.0f, .z = -1.0f, .w = 1.0f },
                virtualCameraInverseProjMat),
            QuaternionTransform(
                (Vector4) { .x = 1.0f, .y = -1.0f, .z = -1.0f, .w = 1.0f },
                virtualCameraInverseProjMat),
            QuaternionTransform(
                (Vector4) { .x = 1.0f, .y = 1.0f, .z = -1.0f, .w = 1.0f },
                virtualCameraInverseProjMat)
        };

        Vector3
            nearPlaneVertices[sizeof nearPlaneCoords / sizeof *nearPlaneCoords];

        int nearPlaneVertexCount = sizeof nearPlaneVertices
                                   / sizeof *nearPlaneVertices;

        for (int i = 0; i < nearPlaneVertexCount; i++)
            nearPlaneVertices[i] =
                Vector3Scale((Vector3) { .x = nearPlaneCoords[i].x,
                                         .y = nearPlaneCoords[i].y,
                                         .z = nearPlaneCoords[i].z },
                             1.0f / nearPlaneCoords[i].w);

        for (int i = nearPlaneVertexCount - 1, j = 0; j < nearPlaneVertexCount;
             i = j, j++)
            DrawCylinderEx(nearPlaneVertices[i],
                           nearPlaneVertices[j],
                           viewFrustumLineThick,
                           viewFrustumLineThick,
                           16,
                           color);

        /* "Far Plane" 그리기 */

        // "Far Plane"의 정점에 대한 동차 좌표
        Vector4 farPlaneCoords[] = {
            QuaternionTransform(
                (Vector4) { .x = -1.0f, .y = 1.0f, .z = 1.0f, .w = 1.0f },
                virtualCameraInverseProjMat),
            QuaternionTransform(
                (Vector4) { .x = -1.0f, .y = -1.0f, .z = 1.0f, .w = 1.0f },
                virtualCameraInverseProjMat),
            QuaternionTransform(
                (Vector4) { .x = 1.0f, .y = -1.0f, .z = 1.0f, .w = 1.0f },
                virtualCameraInverseProjMat),
            QuaternionTransform(
                (Vector4) { .x = 1.0f, .y = 1.0f, .z = 1.0f, .w = 1.0f },
                virtualCameraInverseProjMat)
        };

        Vector3
            farPlaneVertices[sizeof farPlaneCoords / sizeof *farPlaneCoords];

        int farPlaneVertexCount = sizeof farPlaneVertices
                                  / sizeof *farPlaneVertices;

        for (int i = 0; i < farPlaneVertexCount; i++)
            farPlaneVertices[i] =
                Vector3Scale((Vector3) { .x = farPlaneCoords[i].x,
                                         .y = farPlaneCoords[i].y,
                                         .z = farPlaneCoords[i].z },
                             (1.0f / farPlaneCoords[i].w) * 0.9f);

        for (int i = farPlaneVertexCount - 1, j = 0; j < farPlaneVertexCount;
             i = j, j++)
            DrawCylinderEx(farPlaneVertices[i],
                           farPlaneVertices[j],
                           viewFrustumLineThick,
                           viewFrustumLineThick,
                           16,
                           color);

        /* "Near Plane"과 "Far Plane"을 잇는 선분 그리기 */
        for (int i = 0; i < nearPlaneVertexCount; i++)
            DrawCylinderEx(nearPlaneVertices[i],
                           farPlaneVertices[i],
                           viewFrustumLineThick,
                           viewFrustumLineThick,
                           16,
                           color);

        rlEnableBackfaceCulling();
    }
}

/* 공용 셰이더 프로그램을 반환하는 함수 */
Shader LoadCommonShader(void) {
    // 셰이더 소스 파일을 컴파일 및 링크하여 셰이더 프로그램 생성
    Shader shaderProgram = LoadShaderFromMemory(resShadersCommonVsVert,
                                                resShadersCommonFsFrag);

    float gridSlicesValue = GRID_SLICES_VALUE;
    float gridSpacingValue = GRID_SPACING_VALUE;
    float gridThickValue = GRID_THICK_VALUE;

    int slicesLoc = GetShaderLocation(shaderProgram, "slices");
    int spacingLoc = GetShaderLocation(shaderProgram, "spacing");
    int thickLoc = GetShaderLocation(shaderProgram, "thick");

    SetShaderValue(shaderProgram,
                   slicesLoc,
                   &gridSlicesValue,
                   SHADER_UNIFORM_FLOAT);

    SetShaderValue(shaderProgram,
                   spacingLoc,
                   &gridSpacingValue,
                   SHADER_UNIFORM_FLOAT);

    SetShaderValue(shaderProgram,
                   thickLoc,
                   &gridThickValue,
                   SHADER_UNIFORM_FLOAT);

    return shaderProgram;
}
