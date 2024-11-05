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

/* Constants =============================================================== */

/* 정점 셰이더의 파일 경로 */
static const char *commonVsFileName = "../res/shaders/common-vs.vert";

/* 프래그먼트 셰이더의 파일 경로 */
static const char *commonFsFileName = "../res/shaders/common-fs.frag";

/* Public Functions ======================================================== */

/* X축, Y축과 Z축을 그리는 함수 */
void DrawAxes(const Camera *camera) {
    if (camera == NULL) return;

    Vector3 basisX = { .x = 1.0f };
    Vector3 basisY = { .y = 1.0f };
    Vector3 basisZ = { .z = 1.0f };

    /* X축, Y축과 Z축 막대 */
    DrawCylinderEx(Vector3Zero(), basisX, 0.03f, 0.03f, 32, RED);
    DrawCylinderEx(Vector3Zero(), basisY, 0.03f, 0.03f, 32, GREEN);
    DrawCylinderEx(Vector3Zero(), basisZ, 0.03f, 0.03f, 32, BLUE);

    /* 원점 구 */
    DrawSphere(Vector3Zero(), 0.08f, ColorAlpha(BLACK, 0.85f));
}

/* 게임 세계의 물체를 그리는 함수 */
void DrawGameObject(GameObject *gameObject, MvpRenderMode renderMode) {
    if (gameObject == NULL || renderMode == MVP_RENDER_ALL
        || renderMode == MVP_RENDER_COUNT_)
        return;

    Model *model = &(gameObject->model);

    Matrix tmpMatModel = model->transform;

    if (renderMode == MVP_RENDER_LOCAL) model->transform = MatrixIdentity();

    // NOTE: `DrawModelEx()` 구현부에서 모델 행렬 계산하는 부분 삭제하고 가져옴
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