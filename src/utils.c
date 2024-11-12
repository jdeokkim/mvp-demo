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

/* Constants =============================================================== */

/* 정점 셰이더의 파일 경로 */
static const char *commonVsFileName = "../res/shaders/common-vs.vert";

/* 프래그먼트 셰이더의 파일 경로 */
static const char *commonFsFileName = "../res/shaders/common-fs.frag";

/* Public Functions ======================================================== */

/* 화살표를 그리는 함수 */
void DrawArrow(Vector3 startPos, Vector3 endPos, Color color) {
    DrawCylinderEx(startPos, endPos, 0.015f, 0.015f, 32, color);

    Vector3 headStartPos = Vector3Scale(Vector3Normalize(
                                            Vector3Subtract(startPos, endPos)),
                                        0.25f);
    Vector3 headEndPos = endPos;

    DrawCylinderEx(headStartPos, headEndPos, 0.11f, 0.015f, 32, color);
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

    if (renderMode == MVP_RENDER_LOCAL) model->transform = MatrixIdentity();

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

    if (gameObject == GetGameObject(OBJ_TYPE_CAMERA)) {
        Matrix viewMat = GetVirtualCameraViewMat();

        // 뷰 행렬의 U축, V축과 N축 그리기
        DrawAxesEx(
            GetVirtualCamera()->position,
            (Vector3) { .x = viewMat.m0, .y = viewMat.m4, .z = viewMat.m8 },
            (Vector3) { .x = viewMat.m1, .y = viewMat.m5, .z = viewMat.m9 },
            (Vector3) { .x = viewMat.m2, .y = viewMat.m6, .z = viewMat.m10 },
            ColorBrightness(RED, -0.5f),
            ColorBrightness(GREEN, -0.5f),
            ColorBrightness(BLUE, -0.5f));

        DrawArrow(GetVirtualCamera()->position,
                  GetVirtualCamera()->target,
                  YELLOW);
    }

    model->transform = tmpMatModel;
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

/* 공용 셰이더 프로그램을 반환하는 함수 */
Shader LoadCommonShader(void) {
    const char *vsFileName = commonVsFileName;
    const char *fsFileName = commonFsFileName;

    if (!IsPathFile(vsFileName)) vsFileName += 3;
    if (!IsPathFile(fsFileName)) fsFileName += 3;

    // 셰이더 소스 파일을 컴파일 및 링크하여 셰이더 프로그램 생성
    return LoadShader(vsFileName, fsFileName);
}