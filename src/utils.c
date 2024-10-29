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

    Vector3 basisX = (Vector3) { .x = 1.0f };
    Vector3 basisY = (Vector3) { .y = 1.0f };
    Vector3 basisZ = (Vector3) { .z = 1.0f };

    /* X축, Y축과 Z축 막대 */
    DrawCylinderEx(Vector3Zero(), basisX, 0.02f, 0.02f, 32, RED);
    DrawCylinderEx(Vector3Zero(), basisY, 0.02f, 0.02f, 32, GREEN);
    DrawCylinderEx(Vector3Zero(), basisZ, 0.02f, 0.02f, 32, BLUE);

    /* 원점 구 */
    DrawSphere(Vector3Zero(), 0.05f, ColorAlpha(BLACK, 0.85f));
}

/* X축, Y축과 Z축을 나타내는 텍스트를 그리는 함수 */
void DrawAxisNames(const Camera *camera, RenderTexture renderTexture) {
    if (camera == NULL) return;

    Vector3 positionForXAxis = (Vector3) { .x = 0.95f, .y = 0.25f, .z = 0.0f };
    Vector3 positionForYAxis = (Vector3) { .x = 0.1f, .y = 1.0f, .z = 0.1f };
    Vector3 positionForZAxis = (Vector3) { .x = 0.0f, .y = 0.25f, .z = 0.95f };

    // 카메라 "EYE"의 Y 좌표가 원점과 멀어질수록 텍스트 크기를 줄임
    float baseSize = (GetFontDefault().baseSize << 3) / camera->position.y;

    // X축 텍스트
    DrawTextEx(
        GetFontDefault(), 
        "X", 
        GetWorldToScreenEx(
            positionForXAxis, 
            *camera, 
            renderTexture.texture.width, 
            renderTexture.texture.height
        ), 
        baseSize,
        1.0f, 
        RED
    );

    // Y축 텍스트
    DrawTextEx(
        GetFontDefault(), 
        "Y", 
        GetWorldToScreenEx(
            positionForYAxis, 
            *camera, 
            renderTexture.texture.width, 
            renderTexture.texture.height
        ), 
        baseSize,
        1.0f, 
        GREEN
    );

    // Z축 텍스트
    DrawTextEx(
        GetFontDefault(), 
        "Z", 
        GetWorldToScreenEx(
            positionForZAxis, 
            *camera, 
            renderTexture.texture.width, 
            renderTexture.texture.height
        ), 
        baseSize,
        1.0f, 
        BLUE
    );
}

/* 공용 셰이더 프로그램으로 XZ 평면에 격자 무늬를 그리는 함수 */
void DrawInfiniteGrid(const Camera *camera) {
    if (camera == NULL) return;

    static int cameraPositionLoc = -1;

    if (cameraPositionLoc < 0) {
        // 셰이더의 Uniform 위치 찾기
        cameraPositionLoc = GetShaderLocation(
            GetCommonShader(), "cameraPosition"
        );
    }

    {
        BeginShaderMode(GetCommonShader());

        // 셰이더의 Uniform 값을 카메라의 "EYE" 좌표로 설정
        SetShaderValue(
            GetCommonShader(),
            cameraPositionLoc, 
            &camera->position, 
            SHADER_UNIFORM_VEC3
        );

        // 셰이더가 격자 무늬 알아서 다 그려줌
        DrawRectangleRec(
            (Rectangle) {
                .x = 0.0f,
                .y = 0.0f,
                .width = SCREEN_WIDTH,
                .height = SCREEN_HEIGHT
            },
            WHITE
        );

        EndShaderMode();
    }

    DrawAxes(camera);
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