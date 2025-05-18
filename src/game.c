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

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "styles/raygui_style_darkr.h"

/* Macro Constants ========================================================= */

// clang-format off

#define GUI_PANEL_TEXT                      "MVP Transform Visualizer Demo"

/* ========================================================================= */

#define GUI_MODEL_MAT_PANEL_TEXT            "Model Matrix"
#define GUI_MODEL_MAT_TOOLTIP_TEXT          NULL

#define GUI_MODEL_MAT_SCALE_LABEL_TEXT      "Scale: "
#define GUI_MODEL_MAT_SCALE_TOOLTIP_TEXT    NULL

#define GUI_MODEL_MAT_TRANS_LABEL_TEXT      "Trans.:"
#define GUI_MODEL_MAT_TRANS_TOOLTIP_TEXT    NULL

#define GUI_MODEL_MAT_ROTATE_LABEL_TEXT     "Rotate:"
#define GUI_MODEL_MAT_ROTATE_TOOLTIP_TEXT   NULL

/* ========================================================================= */

#define GUI_VIEW_MAT_PANEL_TEXT             "View Matrix"

#define GUI_VIEW_MAT_EYE_LABEL_TEXT         "Eye:"
#define GUI_VIEW_MAT_EYE_TOOLTIP_TEXT       NULL

#define GUI_VIEW_MAT_AT_LABEL_TEXT          "At: "
#define GUI_VIEW_MAT_AT_TOOLTIP_TEXT        NULL

#define GUI_VIEW_MAT_UP_LABEL_TEXT          "Up: "
#define GUI_VIEW_MAT_UP_TOOLTIP_TEXT        NULL

/* ========================================================================= */

#define GUI_PROJ_MAT_PANEL_TEXT             "Projection Matrix"

#define GUI_PROJ_MAT_FOV_LABEL_TEXT         "FOV:     "
#define GUI_PROJ_MAT_FOV_TOOLTIP_TEXT       NULL

#define GUI_PROJ_MAT_ASPECT_LABEL_TEXT      "Aspect:  "
#define GUI_PROJ_MAT_ASPECT_TOOLTIP_TEXT    NULL

#define GUI_PROJ_MAT_NEAR_FAR_LABEL_TEXT    "Near/Far:"
#define GUI_PROJ_MAT_NEAR_FAR_TOOLTIP_TEXT  NULL

/* ========================================================================= */

#define GUI_RENDER_MODE_HINT_TEXT           "%s (Press 'Alt' + [0-4])"

#define GUI_RENDER_MODE_00_TEXT             "All Spaces"
#define GUI_RENDER_MODE_01_TEXT             "Local Space"
#define GUI_RENDER_MODE_02_TEXT             "World Space"
#define GUI_RENDER_MODE_03_TEXT             "View Space"
#define GUI_RENDER_MODE_04_TEXT             "Clip Space"

/* ========================================================================= */

#define GUI_VERTEX_VISIBILITY_HINT_TEXT     "Vertices: %s (Press 'V')"

#define GUI_VERTEX_SHOWN_TEXT               "Shown"
#define GUI_VERTEX_HIDDEN_TEXT              "Hidden"

// clang-format on

/* Constants =============================================================== */

/* 게임 화면의 왼쪽 (GUI 패널) 영역 */
static const Rectangle guiArea = { .width = 0.2f * SCREEN_WIDTH,
                                   .height = SCREEN_HEIGHT };

/* 게임 화면의 오른쪽 (MVP 시각화) 영역 */
static const Rectangle mvpArea = { .x = 0.2f * SCREEN_WIDTH,
                                   .y = 0.0f,
                                   .width = 0.8f * SCREEN_WIDTH,
                                   .height = SCREEN_HEIGHT };

/* "<물체 / 세계 / 카메라> 공간"을 초기화하는 함수들 */
static const InitSpaceFunc initSpaceFuncs[MVP_RENDER_COUNT_] = {
    [MVP_RENDER_LOCAL] = InitLocalSpace,
    [MVP_RENDER_WORLD] = InitWorldSpace,
    [MVP_RENDER_VIEW] = InitViewSpace,
    [MVP_RENDER_CLIP] = InitClipSpace
};

/* 프레임버퍼에 "<물체 / 세계 / 카메라> 공간"을 그리는 함수들 */
static const UpdateSpaceFunc updateSpaceFuncs[MVP_RENDER_COUNT_] = {
    [MVP_RENDER_LOCAL] = UpdateLocalSpace,
    [MVP_RENDER_WORLD] = UpdateWorldSpace,
    [MVP_RENDER_VIEW] = UpdateViewSpace,
    [MVP_RENDER_CLIP] = UpdateClipSpace
};

/* "<물체 / 세계 / 카메라> 공간"에 필요한 메모리 공간을 해제하는 함수들 */
static const DeinitSpaceFunc deinitSpaceFuncs[MVP_RENDER_COUNT_] = {
    [MVP_RENDER_LOCAL] = DeinitLocalSpace,
    [MVP_RENDER_WORLD] = DeinitWorldSpace,
    [MVP_RENDER_VIEW] = DeinitViewSpace,
    [MVP_RENDER_CLIP] = DeinitClipSpace
};

/* clang-format off */

static const char magicNumbers[] = { 0x04, 0x08, 0x0f, 0x10, 0x17, 0x2a, 
                                     0x47, 0x69, 0x74, 0x48, 0x75, 0x62,
                                     0x3a, 0x20, 0x6a, 0x64, 0x65, 0x6f,
                                     0x6b, 0x6b, 0x69, 0x6d, 0x2f, 0x6d,
                                     0x76, 0x70, 0x2d, 0x64, 0x65, 0x6d,
                                     0x6f, 0x00, 0x68, 0x74, 0x74, 0x70,
                                     0x73, 0x3a, 0x2f, 0x2f, 0x67, 0x69,
                                     0x74, 0x68, 0x75, 0x62, 0x2e, 0x63,
                                     0x6f, 0x6d, 0x2f, 0x6a, 0x64, 0x65,
                                     0x6f, 0x6b, 0x6b, 0x69, 0x6d, 0x2f,
                                     0x6d, 0x76, 0x70, 0x2d, 0x64, 0x65,
                                     0x6d, 0x6f, 0x00, 0x00, 0x00, 0x00 };

/* clang-format on */

/* Private Variables ======================================================= */

/* GUI 패널의 "모델 행렬" 영역 */
static Rectangle guiModelMatArea;

/* "모델 행렬"의 초기화 버튼 영역*/
static Rectangle guiModelMatResetButtonArea;

/* "모델 행렬"의 각 요소를 그릴 영역 */
static Rectangle guiModelMatEntryArea[16];

/* "모델 행렬"의 각 요소를 나타내는 문자열 */
static char guiModelMatEntryText[16][MATRIX_VALUE_TEXT_LENGTH];

/* ========================================================================= */

/* "모델 행렬"의 크기 변환 영역 */
static Rectangle guiModelMatScaleArea;

/* "모델 행렬"의 크기 변환 레이블 영역 */
static char guiModelMatScaleLabelText[LABEL_TEXT_LENGTH];

/* "모델 행렬"의 크기 변환을 위한 입력 상자 영역 */
static Rectangle guiModelMatScaleValueBoxArea[3];

/* "모델 행렬"의 크기 변환을 위한 입력 상자의 활성화 여부 */
static bool guiModelMatScaleValueBoxEnabled[3];

/* "모델 행렬"의 크기 변환 정보를 나타내는 문자열 */
static char guiModelMatScaleValueText[3][MATRIX_VALUE_TEXT_LENGTH];

/* "모델 행렬"의 크기 변환 정보가 저장될 배열 */
static float guiModelMatScaleValues[3] = { 1.0f, 1.0f, 1.0f };

/* ========================================================================= */

/* "모델 행렬"의 이동 변환 영역 */
static Rectangle guiModelMatTransArea;

/* "모델 행렬"의 이동 변환 레이블 영역 */
static char guiModelMatTransLabelText[LABEL_TEXT_LENGTH];

/* "모델 행렬"의 이동 변환을 위한 입력 상자 영역 */
static Rectangle guiModelMatTransValueBoxArea[3];

/* "모델 행렬"의 이동 변환을 위한 입력 상자의 활성화 여부 */
static bool guiModelMatTransValueBoxEnabled[3];

/* "모델 행렬"의 이동 변환 정보를 나타내는 문자열 */
static char guiModelMatTransValueText[3][MATRIX_VALUE_TEXT_LENGTH];

/* "모델 행렬"의 이동 변환 정보가 저장될 배열 */
static float guiModelMatTransValues[3] = { 2.0f, 0.5f, 2.0f };

/* ========================================================================= */

/* "모델 행렬"의 회전 변환 영역 */
static Rectangle guiModelMatRotateArea;

/* "모델 행렬"의 회전 변환 레이블 영역 */
static char guiModelMatRotateLabelText[LABEL_TEXT_LENGTH];

/* "모델 행렬"의 회전 변환을 위한 입력 상자 영역 */
static Rectangle guiModelMatRotateValueBoxArea[3];

/* "모델 행렬"의 회전 변환을 위한 입력 상자의 활성화 여부 */
static bool guiModelMatRotateValueBoxEnabled[3];

/* "모델 행렬"의 회전 변환 정보를 나타내는 문자열 */
static char guiModelMatRotateValueText[3][MATRIX_VALUE_TEXT_LENGTH];

/* "모델 행렬"의 회전 변환 정보가 저장될 배열 */
static float guiModelMatRotateValues[3] = { 0.0f, 0.0f, 0.0f };

/* ========================================================================= */

/* GUI 패널의 "뷰 행렬" 영역 */
static Rectangle guiViewMatArea;

/* "뷰 행렬"의 각 요소를 그릴 영역 */
static Rectangle guiViewMatEntryArea[16];

/* "뷰 행렬"의 각 요소를 나타내는 문자열 */
static char guiViewMatEntryText[16][MATRIX_VALUE_TEXT_LENGTH];

/* ========================================================================= */

/* "뷰 행렬"의 "EYE" 벡터 영역 */
static Rectangle guiViewMatEyeArea;

/* "뷰 행렬"의 "EYE" 벡터 레이블 영역 */
static char guiViewMatEyeLabelText[LABEL_TEXT_LENGTH];

/* "뷰 행렬"의 "EYE" 벡터를 위한 입력 상자 영역 */
static Rectangle guiViewMatEyeValueBoxArea[3];

/* "뷰 행렬"의 "EYE" 벡터를 위한 입력 상자의 활성화 여부 */
static bool guiViewMatEyeValueBoxEnabled[3];

/* "뷰 행렬"의 "EYE" 벡터를 나타내는 문자열 */
static char guiViewMatEyeValueText[3][MATRIX_VALUE_TEXT_LENGTH];

/* "뷰 행렬"의 "EYE" 벡터 정보가 저장될 배열 */
static float guiViewMatEyeValues[3] = { -3.0f, 2.25f, -0.5f };

/* ========================================================================= */

/* "뷰 행렬"의 "AT" 벡터 영역 */
static Rectangle guiViewMatAtArea;

/* "뷰 행렬"의 "AT" 벡터 레이블 영역 */
static char guiViewMatAtLabelText[LABEL_TEXT_LENGTH];

/* "뷰 행렬"의 "AT" 벡터를 위한 입력 상자 영역 */
static Rectangle guiViewMatAtValueBoxArea[3];

/* "뷰 행렬"의 "AT" 벡터를 위한 입력 상자의 활성화 여부 */
static bool guiViewMatAtValueBoxEnabled[3];

/* "뷰 행렬"의 "AT" 벡터를 나타내는 문자열 */
static char guiViewMatAtValueText[3][MATRIX_VALUE_TEXT_LENGTH];

/* "뷰 행렬"의 "AT" 벡터 정보가 저장될 배열 */
static float guiViewMatAtValues[3] = { 0.0f, 0.0f, 0.0f };

/* ========================================================================= */

/* "뷰 행렬"의 "UP" 벡터 영역 */
static Rectangle guiViewMatUpArea;

/* "뷰 행렬"의 "UP" 벡터 레이블 영역 */
static char guiViewMatUpLabelText[LABEL_TEXT_LENGTH];

/* "뷰 행렬"의 "UP" 벡터를 위한 입력 상자 영역 */
static Rectangle guiViewMatUpValueBoxArea[3];

/* "뷰 행렬"의 "UP" 벡터를 위한 입력 상자의 활성화 여부 */
static bool guiViewMatUpValueBoxEnabled[3];

/* "뷰 행렬"의 "UP" 벡터를 나타내는 문자열 */
static char guiViewMatUpValueText[3][MATRIX_VALUE_TEXT_LENGTH];

/* "뷰 행렬"의 "UP" 벡터 정보가 저장될 배열 */
static float guiViewMatUpValues[3] = { 0.0f, 1.0f, 0.0f };

/* ========================================================================= */

/* GUI 패널의 "투영 행렬" 영역 */
static Rectangle guiProjMatArea;

/* "투영 행렬"의 각 요소를 그릴 영역 */
static Rectangle guiProjMatEntryArea[16];

/* "투영 행렬"의 각 요소를 나타내는 문자열 */
static char guiProjMatEntryText[16][MATRIX_VALUE_TEXT_LENGTH];

/* ========================================================================= */

/* "투영 행렬"의 "FOV" 영역 */
static Rectangle guiProjMatFovArea;

/* "투영 행렬"의 "FOV" 레이블 영역 */
static char guiProjMatFovLabelText[LABEL_TEXT_LENGTH];

/* "투영 행렬"의 "FOV"를 위한 입력 상자 영역 */
static Rectangle guiProjMatFovValueBoxArea[1];

/* "투영 행렬"의 "FOV"를 위한 입력 상자의 활성화 여부 */
static bool guiProjMatFovValueBoxEnabled[1];

/* "투영 행렬"의 "FOV"를 나타내는 문자열 */
static char guiProjMatFovValueText[1][MATRIX_VALUE_TEXT_LENGTH];

/* "투영 행렬"의 "FOV" 정보가 저장될 배열 */
static float guiProjMatFovValues[1] = { 60.0f };

/* ========================================================================= */

/* "투영 행렬"의 "Aspect" 영역 */
static Rectangle guiProjMatAspectArea;

/* "투영 행렬"의 "Aspect" 레이블 영역 */
static char guiProjMatAspectLabelText[LABEL_TEXT_LENGTH];

/* "투영 행렬"의 "Aspect"를 위한 입력 상자 영역 */
static Rectangle guiProjMatAspectValueBoxArea[1];

/* "투영 행렬"의 "Aspect"를 나타내는 문자열 */
static char guiProjMatAspectValueText[1][MATRIX_VALUE_TEXT_LENGTH];

/* "투영 행렬"의 "Aspect" 정보가 저장될 배열 */
static float guiProjMatAspectValues[1];

/* ========================================================================= */

/* "투영 행렬"의 "Near/Far Distance" 영역 */
static Rectangle guiProjMatNearFarArea;

/* "투영 행렬"의 "Near/Far Distance" 레이블 영역 */
static char guiProjMatNearFarLabelText[LABEL_TEXT_LENGTH];

/* "투영 행렬"의 "Near/Far Distance"을 위한 입력 상자 영역 */
static Rectangle guiProjMatNearFarValueBoxArea[2];

/* "투영 행렬"의 "Near/Far Distance"를 위한 입력 상자의 활성화 여부 */
static bool guiProjMatNearFarValueBoxEnabled[2];

/* "투영 행렬"의 "Near/Far Distance"을 나타내는 문자열 */
static char guiProjMatNearFarValueText[2][MATRIX_VALUE_TEXT_LENGTH];

/* "투영 행렬"의 "Near/Far Distance" 정보가 저장될 배열 */
static float guiProjMatNearFarValues[2] = { CULL_DISTANCE_NEAR_MIN_VALUE,
                                            CULL_DISTANCE_FAR_MAX_VALUE };

/* ========================================================================= */

/* GUI 패널에서 예약된 영역 */
static Rectangle guiReservedArea;

/* ========================================================================= */

/* 게임 세계에 존재하는 물체들 */
static GameObject gameObjects[GAME_OBJECT_COUNT];

/* 게임 화면의 오른쪽 영역을 4개로 분할하고, 렌더 텍스처를 그림 */
static RenderTexture renderTextures[MVP_RENDER_COUNT_];

/* MVP 영역에 그릴 화면의 종류 */
static MvpRenderMode renderMode = MVP_RENDER_ALL;

/* 컴파일 및 링크 과정을 거친 공용 셰이더 프로그램 */
static Shader shaderProgram;

/* 모델을 그릴 때 사용할 텍스처 아틀라스 (atlas)의 일부분 */
static Texture cameraTexture, enemyTexture;

/* `renderMode` 텍스트의 애니메이션 길이 */
static float renderModeCounter = 0.0f;

/* 플레이어 모델의 정점 표시 여부 */
static bool showPlayerVertices = false;

/* Private Function Prototypes ============================================= */

/* 게임 화면의 왼쪽 영역을 그리는 함수 */
static void DrawGuiArea(void);

/* MVP 영역에 그릴 화면의 종류를 표시하는 함수 */
static void DrawRenderModeText(void);

/* 게임 화면의 오른쪽 영역을 그리는 함수 */
static void DrawMvpArea(void);

/* 플레이어 모델의 정점 표시 여부를 보여주는 함수 */
static void DrawVertexVisibilityText(void);

/* 카메라 모델을 생성하는 함수 */
static Model GenerateCameraModel(void);

/* 적 모델을 생성하는 함수 */
static Model GenerateEnemyModel(void);

/* 플레이어 모델을 생성하는 함수 */
static Model GeneratePlayerModel(void);

/* 마우스 및 키보드 입력을 처리하는 함수 */
static void HandleInputEvents(void);

/* GUI 패널에 그릴 위젯들의 영역을 정의하는 함수 */
static void InitGuiAreas(void);

/* "모델 행렬"을 단위 행렬로 초기화하는 함수 */
static void ResetModelMatrix(void);

/* 행렬의 각 요소를 나타내는 문자열을 업데이트하는 함수 */
static void UpdateMatrixEntryText(char (*matEntryText)[16], Matrix matrix);

/* Public Functions ======================================================== */

/* 게임 화면을 초기화하는 함수 */
void InitGameScreen(void) {
    {
        GuiLoadStyleDarkr();

        InitGuiAreas();
    }

    shaderProgram = LoadCommonShader();

    {
        /* 게임 세계에 플레이어와 카메라 등의 모델 추가 */

        for (int i = 0; i < GAME_OBJECT_COUNT; i++) {
            if (i == OBJ_TYPE_CAMERA) {
                gameObjects[i].model = GenerateCameraModel();

                UpdateViewMatrix(true);
            } else if (i == OBJ_TYPE_PLAYER) {
                gameObjects[i].model = GeneratePlayerModel();
                
                UpdateModelMatrix(true);
            } else {
                gameObjects[i].model = GenerateEnemyModel();
            }
        }
    }

    UpdateModelMatrix(true), UpdateViewMatrix(true), UpdateProjMatrix(true);

    for (int i = MVP_RENDER_ALL + 1; i < MVP_RENDER_COUNT_; i++) {
        if (initSpaceFuncs[i] == NULL) continue;

        renderTextures[i] = LoadRenderTexture(mvpArea.width, mvpArea.height);

        initSpaceFuncs[i]();
    }
}

/* 게임 화면을 그리고 게임 상태를 업데이트하는 함수 */
void UpdateGameScreen(void) {
    // 마우스 및 키보드 입력 처리
    HandleInputEvents();

    // 프레임버퍼 초기화
    BeginDrawing();

    {
        ClearBackground(RAYWHITE);

        DrawMvpArea(), DrawGuiArea();

        DrawRenderModeText(), DrawVertexVisibilityText();
    }

    // 이중 버퍼링 (double buffering) 기법으로 프레임버퍼 교체
    EndDrawing();
}

/* 게임 화면에 필요한 메모리 공간을 해제하는 함수 */
void DeinitGameScreen(void) {
    UnloadFont(GuiGetFont());

    UnloadShader(shaderProgram);

    for (int i = MVP_RENDER_ALL + 1; i < MVP_RENDER_COUNT_; i++) {
        if (deinitSpaceFuncs[i] == NULL) continue;

        UnloadRenderTexture(renderTextures[i]);

        deinitSpaceFuncs[i]();
    }
}

/* ========================================================================= */

/* 공용 셰이더 프로그램을 반환하는 함수 */
Shader GetCommonShader(void) {
    return shaderProgram;
}

/* 게임 세계의 `index`번째 물체를 반환하는 함수 */
GameObject *GetGameObject(int index) {
    return (index >= 0 && index < GAME_OBJECT_COUNT) ? &gameObjects[index]
                                                     : NULL;
}

/* GUI 영역에 사용되는 글꼴을 반환하는 함수 */
Font GetDefaultFont(void) {
    return GuiGetFont();
}

/* MVP 영역에 그려지고 있는 화면의 종류를 반환하는 함수 */
MvpRenderMode GetMvpRenderMode(void) {
    return renderMode;
}

/* View Frustum의 "Aspect" 값을 반환하는 함수 */
float GetViewFrustumAspect(void) {
    return guiProjMatAspectValues[0];
}

/* View Frustum의 "Near Distance" 값을 반환하는 함수 */
float GetViewFrustumNearDistance(void) {
    return guiProjMatNearFarValues[0];
}

/* View Frustum의 "Far Distance" 값을 반환하는 함수 */
float GetViewFrustumFarDistance(void) {
    return guiProjMatNearFarValues[1];
}

/* 플레이어 모델의 정점 위치 표시 여부를 반환하는 함수 */
bool IsVertexVisibilityModeEnabled(void) {
    return showPlayerVertices;
}

/* "모델 행렬"을 업데이트하는 함수 */
void UpdateModelMatrix(bool fromGUI) {
    if (fromGUI) {
        Matrix scaleMat = MatrixScale(guiModelMatScaleValues[0],
                                      guiModelMatScaleValues[1],
                                      guiModelMatScaleValues[2]);
        Matrix rotationMat = MatrixRotateXYZ(
            (Vector3) { .x = DEG2RAD * guiModelMatRotateValues[0],
                        .y = DEG2RAD * guiModelMatRotateValues[1],
                        .z = DEG2RAD * guiModelMatRotateValues[2] });
        Matrix transMat = MatrixTranslate(guiModelMatTransValues[0],
                                          guiModelMatTransValues[1],
                                          guiModelMatTransValues[2]);

        gameObjects[OBJ_TYPE_PLAYER].model.transform =
            MatrixMultiply(MatrixMultiply(scaleMat, rotationMat), transMat);
    }

    UpdateMatrixEntryText(guiModelMatEntryText,
                          gameObjects[OBJ_TYPE_PLAYER].model.transform);

    for (int i = 0; i < 3; i++) {
        strncpy(guiModelMatScaleValueText[i],
                TextFormat("%.2f", guiModelMatScaleValues[i]),
                MATRIX_VALUE_TEXT_LENGTH);
        strncpy(guiModelMatTransValueText[i],
                TextFormat("%.2f", guiModelMatTransValues[i]),
                MATRIX_VALUE_TEXT_LENGTH);
        strncpy(guiModelMatRotateValueText[i],
                TextFormat("%.1f", guiModelMatRotateValues[i]),
                MATRIX_VALUE_TEXT_LENGTH);
    }
}

/* "뷰 행렬"을 업데이트하는 함수 */
void UpdateViewMatrix(bool fromGUI) {
    Camera *virtualCamera = GetVirtualCamera();

    if (fromGUI) {
        Vector3 eye = { .x = guiViewMatEyeValues[0],
                        .y = guiViewMatEyeValues[1],
                        .z = guiViewMatEyeValues[2] };

        Vector3 at = { .x = guiViewMatAtValues[0],
                       .y = guiViewMatAtValues[1],
                       .z = guiViewMatAtValues[2] };

        Vector3 up = { .x = guiViewMatUpValues[0],
                       .y = guiViewMatUpValues[1],
                       .z = guiViewMatUpValues[2] };

        virtualCamera->position = eye;
        virtualCamera->target = at;
        virtualCamera->up = up;
    }

    gameObjects[OBJ_TYPE_CAMERA].model.transform = GetVirtualCameraModelMat(
        false);

    UpdateMatrixEntryText(guiViewMatEntryText, GetVirtualCameraViewMat(true));

    for (int i = 0; i < 3; i++) {
        strncpy(guiViewMatEyeValueText[i],
                TextFormat("%.2f", guiViewMatEyeValues[i]),
                MATRIX_VALUE_TEXT_LENGTH);
        strncpy(guiViewMatAtValueText[i],
                TextFormat("%.2f", guiViewMatAtValues[i]),
                MATRIX_VALUE_TEXT_LENGTH);
        strncpy(guiViewMatUpValueText[i],
                TextFormat("%.2f", guiViewMatUpValues[i]),
                MATRIX_VALUE_TEXT_LENGTH);
    }
}

/* "투영 행렬"을 업데이트하는 함수 */
void UpdateProjMatrix(bool fromGUI) {
    if (fromGUI) {
        GetVirtualCamera()->fovy = guiProjMatFovValues[0];

        // NOTE: 창 크기가 아닌 `mvpArea` 크기를 이용해 "Aspect" 값을 설정해야 함!
        guiProjMatAspectValues[0] = (float) mvpArea.width
                                    / (float) mvpArea.height;

        rlSetClipPlanes(guiProjMatNearFarValues[0], guiProjMatNearFarValues[1]);
    }

    UpdateMatrixEntryText(guiProjMatEntryText, GetVirtualCameraProjMat(true));

    strncpy(guiProjMatFovValueText[0],
            TextFormat("%.2f", guiProjMatFovValues[0]),
            MATRIX_VALUE_TEXT_LENGTH);

    strncpy(guiProjMatAspectValueText[0],
            TextFormat("%.2f", guiProjMatAspectValues[0]),
            MATRIX_VALUE_TEXT_LENGTH);

    strncpy(guiProjMatNearFarValueText[0],
            TextFormat("%.2f", guiProjMatNearFarValues[0]),
            MATRIX_VALUE_TEXT_LENGTH);

    strncpy(guiProjMatNearFarValueText[1],
            TextFormat("%.2f", guiProjMatNearFarValues[1]),
            MATRIX_VALUE_TEXT_LENGTH);
}

/* Private Functions ======================================================= */

/* 게임 화면의 왼쪽 영역을 그리는 함수 */
static void DrawGuiArea(void) {
    {
        {
            int tmpTextAlignment = GuiGetStyle(STATUSBAR, TEXT_ALIGNMENT);

            // 패널의 상태 표시줄 텍스트를 가운데 정렬
            GuiSetStyle(STATUSBAR, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);

            GuiPanel(guiArea, GUI_PANEL_TEXT);

            GuiSetStyle(STATUSBAR, TEXT_ALIGNMENT, tmpTextAlignment);
        }

        Matrix modelMat = gameObjects[OBJ_TYPE_PLAYER].model.transform;

        {
            GuiPanel(guiModelMatArea, GUI_MODEL_MAT_PANEL_TEXT);

            {
                if (GuiLabelButton(guiModelMatResetButtonArea,
                                   GuiIconText(ICON_UNDO, NULL)))
                    ResetModelMatrix();
            }

            {
                GuiDisable();

                for (int i = 0; i < 16; i++)
                    GuiTextBox(guiModelMatEntryArea[i],
                               guiModelMatEntryText[i],
                               MATRIX_VALUE_TEXT_LENGTH,
                               false);

                GuiEnable();
            }

            GuiLabel(guiModelMatScaleArea, guiModelMatScaleLabelText);

            for (int i = 0; i < 3; i++)
                if (GuiValueBoxFloat(guiModelMatScaleValueBoxArea[i],
                                     NULL,
                                     guiModelMatScaleValueText[i],
                                     &guiModelMatScaleValues[i],
                                     guiModelMatScaleValueBoxEnabled[i])) {
                    if (guiModelMatScaleValueBoxEnabled[i])
                        UpdateModelMatrix(true);

                    guiModelMatScaleValueBoxEnabled[i] =
                        !guiModelMatScaleValueBoxEnabled[i];
                }

            GuiLabel(guiModelMatTransArea, guiModelMatTransLabelText);

            for (int i = 0; i < 3; i++)
                if (GuiValueBoxFloat(guiModelMatTransValueBoxArea[i],
                                     NULL,
                                     guiModelMatTransValueText[i],
                                     &guiModelMatTransValues[i],
                                     guiModelMatTransValueBoxEnabled[i])) {
                    if (guiModelMatTransValueBoxEnabled[i])
                        UpdateModelMatrix(true);

                    guiModelMatTransValueBoxEnabled[i] =
                        !guiModelMatTransValueBoxEnabled[i];
                }

            GuiLabel(guiModelMatRotateArea, guiModelMatRotateLabelText);

            for (int i = 0; i < 3; i++)
                if (GuiValueBoxFloat(guiModelMatRotateValueBoxArea[i],
                                     NULL,
                                     guiModelMatRotateValueText[i],
                                     &guiModelMatRotateValues[i],
                                     guiModelMatRotateValueBoxEnabled[i])) {
                    if (guiModelMatRotateValueBoxEnabled[i])
                        UpdateModelMatrix(true);

                    guiModelMatRotateValueBoxEnabled[i] =
                        !guiModelMatRotateValueBoxEnabled[i];
                }
        }

        {
            GuiPanel(guiViewMatArea, GUI_VIEW_MAT_PANEL_TEXT);

            {
                GuiDisable();

                for (int i = 0; i < 16; i++)
                    GuiTextBox(guiViewMatEntryArea[i],
                               guiViewMatEntryText[i],
                               MATRIX_VALUE_TEXT_LENGTH,
                               false);

                GuiEnable();
            }

            GuiLabel(guiViewMatEyeArea, guiViewMatEyeLabelText);

            for (int i = 0; i < 3; i++)
                if (GuiValueBoxFloat(guiViewMatEyeValueBoxArea[i],
                                     NULL,
                                     guiViewMatEyeValueText[i],
                                     &guiViewMatEyeValues[i],
                                     guiViewMatEyeValueBoxEnabled[i])) {
                    if (guiViewMatEyeValueBoxEnabled[i]) UpdateViewMatrix(true);

                    guiViewMatEyeValueBoxEnabled[i] =
                        !guiViewMatEyeValueBoxEnabled[i];
                }

            GuiLabel(guiViewMatAtArea, guiViewMatAtLabelText);

            for (int i = 0; i < 3; i++)
                if (GuiValueBoxFloat(guiViewMatAtValueBoxArea[i],
                                     NULL,
                                     guiViewMatAtValueText[i],
                                     &guiViewMatAtValues[i],
                                     guiViewMatAtValueBoxEnabled[i])) {
                    if (guiViewMatAtValueBoxEnabled[i]) UpdateViewMatrix(true);

                    guiViewMatAtValueBoxEnabled[i] =
                        !guiViewMatAtValueBoxEnabled[i];
                }

            GuiLabel(guiViewMatUpArea, guiViewMatUpLabelText);

            for (int i = 0; i < 3; i++)
                if (GuiValueBoxFloat(guiViewMatUpValueBoxArea[i],
                                     NULL,
                                     guiViewMatUpValueText[i],
                                     &guiViewMatUpValues[i],
                                     guiViewMatUpValueBoxEnabled[i])) {
                    if (guiViewMatUpValueBoxEnabled[i]) UpdateViewMatrix(true);

                    guiViewMatUpValueBoxEnabled[i] =
                        !guiViewMatUpValueBoxEnabled[i];
                }
        }

        {
            GuiPanel(guiProjMatArea, GUI_PROJ_MAT_PANEL_TEXT);

            {
                GuiDisable();

                for (int i = 0; i < 16; i++)
                    GuiTextBox(guiProjMatEntryArea[i],
                               guiProjMatEntryText[i],
                               MATRIX_VALUE_TEXT_LENGTH,
                               false);

                GuiEnable();
            }

            GuiLabel(guiProjMatFovArea, guiProjMatFovLabelText);

            for (int i = 0; i < 1; i++)
                if (GuiValueBoxFloat(guiProjMatFovValueBoxArea[i],
                                     NULL,
                                     guiProjMatFovValueText[i],
                                     &guiProjMatFovValues[i],
                                     guiProjMatFovValueBoxEnabled[i])) {
                    if (guiProjMatFovValues[i] < CAMERA_FOV_MIN_VALUE)
                        guiProjMatFovValues[i] = CAMERA_FOV_MIN_VALUE;

                    if (guiProjMatFovValues[i] > CAMERA_FOV_MAX_VALUE)
                        guiProjMatFovValues[i] = CAMERA_FOV_MAX_VALUE;

                    if (guiProjMatFovValueBoxEnabled[i]) UpdateProjMatrix(true);

                    guiProjMatFovValueBoxEnabled[i] =
                        !guiProjMatFovValueBoxEnabled[i];
                }

            GuiLabel(guiProjMatAspectArea, guiProjMatAspectLabelText);

            for (int i = 0; i < 1; i++)
                GuiValueBoxFloat(guiProjMatAspectValueBoxArea[i],
                                 NULL,
                                 guiProjMatAspectValueText[i],
                                 &guiProjMatAspectValues[i],
                                 false);

            GuiLabel(guiProjMatNearFarArea, guiProjMatNearFarLabelText);

            for (int i = 0; i < 2; i++)
                if (GuiValueBoxFloat(guiProjMatNearFarValueBoxArea[i],
                                     NULL,
                                     guiProjMatNearFarValueText[i],
                                     &guiProjMatNearFarValues[i],
                                     guiProjMatNearFarValueBoxEnabled[i])) {
                    guiProjMatNearFarValues[0] =
                        Clamp(guiProjMatNearFarValues[0],
                              CULL_DISTANCE_NEAR_MIN_VALUE,
                              CULL_DISTANCE_NEAR_MAX_VALUE);
                    guiProjMatNearFarValues[1] =
                        Clamp(guiProjMatNearFarValues[1],
                              CULL_DISTANCE_FAR_MIN_VALUE,
                              CULL_DISTANCE_FAR_MAX_VALUE);

                    if (guiProjMatNearFarValueBoxEnabled[i])
                        UpdateProjMatrix(true);

                    guiProjMatNearFarValueBoxEnabled[i] =
                        !guiProjMatNearFarValueBoxEnabled[i];
                }
        }

        {
            int tmpTextAlignment = GuiGetStyle(LABEL, TEXT_ALIGNMENT);

            GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);

            GuiLabelButton(guiReservedArea, magicNumbers + 6)
                ? OpenURL(magicNumbers + 32)
                : 0;

            GuiSetStyle(LABEL, TEXT_ALIGNMENT, tmpTextAlignment);
        }
    }
}

/* MVP 영역에 그릴 화면의 종류를 보여주는 함수 */
static void DrawRenderModeText(void) {
    static const char *renderModeTitles[MVP_RENDER_COUNT_] = {
        GUI_RENDER_MODE_00_TEXT,
        GUI_RENDER_MODE_01_TEXT,
        GUI_RENDER_MODE_02_TEXT,
        GUI_RENDER_MODE_03_TEXT,
        GUI_RENDER_MODE_04_TEXT
    };

    const char *renderModeHintText = TextFormat(GUI_RENDER_MODE_HINT_TEXT,
                                                renderModeTitles[renderMode]);

    Vector2 renderModeHintTextSize = MeasureTextEx(GuiGetFont(),
                                                   renderModeHintText,
                                                   (GuiGetFont().baseSize),
                                                   0.0f);

    float renderModeAlpha = 1.0f
                            - (renderModeCounter
                               / RENDER_MODE_ANIMATION_DURATION);

    DrawTextEx(GuiGetFont(),
               renderModeHintText,
               (Vector2) { .x = SCREEN_WIDTH - renderModeHintTextSize.x,
                           .y = 8.0f },
               (GuiGetFont().baseSize),
               0.0f,
               ColorAlpha(PURPLE, renderModeAlpha));
}

/* 게임 화면의 오른쪽 영역을 그리는 함수 */
static void DrawMvpArea(void) {
    {
        // MVP 영역에 그릴 화면의 종류를 잠시 동안 보여주기
        if (renderModeCounter < RENDER_MODE_ANIMATION_DURATION)
            renderModeCounter += GetFrameTime();
    }

    {
        for (int i = MVP_RENDER_ALL + 1; i < MVP_RENDER_COUNT_; i++) {
            if (updateSpaceFuncs[i] == NULL) continue;

            updateSpaceFuncs[i](renderTextures[i]);

            // 텍스처 필터링 (이중 선형 필터링)
            SetTextureFilter(renderTextures[i].texture,
                             TEXTURE_FILTER_BILINEAR);
        }
    }

    // 전부 다 그리기 vs. 하나만 그리기
    if (renderMode == MVP_RENDER_ALL) {
        for (int i = MVP_RENDER_ALL + 1; i < MVP_RENDER_COUNT_; i++) {
            float halfWidth = 0.5f * mvpArea.width;
            float halfHeight = 0.5f * mvpArea.height;

            /*
                첫 번째 행에는 "물체 공간"과 "세계 공간",
                두 번째 행에는 "카메라 (뷰) 공간"과 "클립 공간"
            */
            DrawTexturePro(renderTextures[i].texture,
                           (Rectangle) { .width = mvpArea.width,
                                         .height = -mvpArea.height },
                           (Rectangle) {
                               .x = mvpArea.x + ((!(i & 1)) * halfWidth),
                               .y = mvpArea.y
                                    + ((i > MVP_RENDER_WORLD) * halfHeight),
                               .width = 0.5f * mvpArea.width,
                               .height = 0.5f * mvpArea.height },
                           Vector2Zero(),
                           0.0f,
                           WHITE);
        }
    } else {
        DrawTexturePro(renderTextures[renderMode].texture,
                       (Rectangle) { .width = mvpArea.width,
                                     .height = -mvpArea.height },
                       mvpArea,
                       Vector2Zero(),
                       0.0f,
                       WHITE);
    }

    {
        /* GUI 영역과 MVP 영역 사이의 경계선 */

        DrawLineEx((Vector2) { .x = mvpArea.x, .y = 0.0f * SCREEN_HEIGHT },
                   (Vector2) { .x = mvpArea.x, .y = 1.0f * SCREEN_HEIGHT },
                   1.0f,
                   GRAY);
    }

    if (renderMode == MVP_RENDER_ALL) {
        /* MVP 영역 위에 그려지는 회색 십자선 */

        Vector2 mvpCenter = { .x = mvpArea.x + (0.5f * mvpArea.width),
                              .y = mvpArea.y + (0.5f * mvpArea.height) };

        DrawLineEx((Vector2) { .x = mvpCenter.x, .y = mvpArea.y },
                   (Vector2) { .x = mvpCenter.x, .y = 1.0f * SCREEN_HEIGHT },
                   1.0f,
                   GRAY);

        DrawLineEx((Vector2) { .x = mvpArea.x, .y = mvpCenter.y },
                   (Vector2) { .x = 1.0f * SCREEN_WIDTH, .y = mvpCenter.y },
                   1.0f,
                   GRAY);
    }
}

/* 플레이어 모델의 정점 표시 여부를 보여주는 함수 */
static void DrawVertexVisibilityText(void) {
    if (renderMode != MVP_RENDER_ALL) return;

    const char *vertexVisibilityHintText = TextFormat(
        GUI_VERTEX_VISIBILITY_HINT_TEXT,
        (showPlayerVertices ? GUI_VERTEX_SHOWN_TEXT : GUI_VERTEX_HIDDEN_TEXT));
    
    Font guiFont = GuiGetFont();

    Vector2 vertexVisibilityHintTextSize =
        MeasureTextEx(guiFont,
                      vertexVisibilityHintText,
                      guiFont.baseSize,
                      0.0f);

    DrawTextEx(guiFont,
               vertexVisibilityHintText,
               (Vector2) { .x = SCREEN_WIDTH
                                - (vertexVisibilityHintTextSize.x + 8.0f),
                           .y = SCREEN_HEIGHT
                                - (vertexVisibilityHintTextSize.y + 8.0f) },
               (guiFont.baseSize),
               0.0f,
               ColorBrightness(BLACK, (showPlayerVertices ? 0.1f : 0.25f)));
}

/* 카메라 모델을 생성하는 함수 */
static Model GenerateCameraModel(void) {
    // TODO: ...    
}

/* 적 모델을 생성하는 함수 */
static Model GenerateEnemyModel(void) {
    // TODO: ...    
}

/* 플레이어 모델을 생성하는 함수 */
static Model GeneratePlayerModel(void) {
    // TODO: ...    
}

/* 마우스 및 키보드 입력을 처리하는 함수 */
static void HandleInputEvents(void) {
    {
        /* 마우스 커서의 종류 변경 */

        Vector2 mousePosition = GetMousePosition();

        SetMouseCursor(CheckCollisionPointRec(mousePosition, guiArea)
                           ? MOUSE_CURSOR_DEFAULT
                           : GetMouseCursor());
    }

    {
        /* MVP 영역에 그릴 화면 종류 변경 */

        if (IsKeyDown(KEY_LEFT_ALT)) {
            int keyCode = GetKeyPressed();

            if (keyCode >= '0' + MVP_RENDER_ALL
                && keyCode < '0' + MVP_RENDER_COUNT_)
                renderMode = keyCode - '0', renderModeCounter = 0.0f;
        }

        /* 플레이어 모델의 정점 위치 및 좌표 표시 여부 변경 */

        if (IsKeyPressed(KEY_V)) showPlayerVertices = !showPlayerVertices;
    }
}

/* GUI 패널에 그릴 위젯들의 영역을 정의하는 함수 */
static void InitGuiAreas(void) {
    float guiDefaultPaddingSize = 6.0f;

    float guiMatEntryAreaWidth = 48.0f;
    float guiMatEntryAreaHeight = 24.0f;

    guiModelMatArea = (Rectangle) {
        .x = 0.0f,
        .y = (guiArea.y + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT)
             + guiDefaultPaddingSize,
        .width = guiArea.width,
        .height = ((guiArea.y + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT)
                   + (2.5f * guiDefaultPaddingSize))
                  + ((4.0f * guiMatEntryAreaHeight)
                     + (4.0f * guiDefaultPaddingSize))
    };

    float guiMatEntryOffsetX = 0.5f
                               * (guiModelMatArea.width
                                  - ((4.0f * guiMatEntryAreaWidth)
                                     + (3.0f * guiDefaultPaddingSize)));

    {
        guiModelMatResetButtonArea = (Rectangle) {
            .x = (guiModelMatArea.x + guiModelMatArea.width)
                 - RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT,
            .y = guiModelMatArea.y,
            .width = RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT,
            .height = RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT
        };
    }

    {
        for (int i = 0,
                 j = sizeof guiModelMatEntryArea / sizeof *guiModelMatEntryArea;
             i < j;
             i++) {
            float guiMatEntryPaddingWidth = ((i / 4)
                                             * (guiMatEntryAreaWidth
                                                + guiDefaultPaddingSize));

            float guiMatEntryPaddingHeight = ((i % 4)
                                              * (guiMatEntryAreaHeight
                                                 + guiDefaultPaddingSize));

            guiModelMatEntryArea[i] = (Rectangle) {
                .x = (guiModelMatArea.x + guiMatEntryOffsetX)
                     + guiMatEntryPaddingWidth,
                .y = (guiModelMatArea.y
                      + (RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT
                         + (1.25f * guiDefaultPaddingSize)))
                     + guiMatEntryPaddingHeight,
                .width = guiMatEntryAreaWidth,
                .height = guiMatEntryAreaHeight
            };
        }
    }

    {
        guiModelMatScaleArea = (Rectangle) {
            .x = guiModelMatArea.x + guiMatEntryOffsetX,
            .y = (guiModelMatArea.y
                  + (RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT
                     + (1.5f * guiDefaultPaddingSize)))
                 + 4.0f * (guiMatEntryAreaHeight + guiDefaultPaddingSize),
            .width = ((4.0f * guiMatEntryAreaWidth)
                      + (3.0f * guiDefaultPaddingSize)),
            .height = guiMatEntryAreaHeight
        };

        strncpy(guiModelMatScaleLabelText,
                GuiIconText(ICON_CURSOR_SCALE, GUI_MODEL_MAT_SCALE_LABEL_TEXT),
                LABEL_TEXT_LENGTH);

        Vector2 textAreaSize = MeasureTextEx(GuiGetFont(),
                                             guiModelMatScaleLabelText,
                                             GuiGetFont().baseSize,
                                             -2.0f);

        float valueBoxWidth = (((guiModelMatScaleArea.width - textAreaSize.x)
                                - guiDefaultPaddingSize)
                               - (2.0f * guiDefaultPaddingSize))
                              / 3.0f;

        float valueBoxHeight = guiModelMatScaleArea.height;

        for (int i = 0; i < 3; i++)
            guiModelMatScaleValueBoxArea[i] = (Rectangle) {
                .x = (guiModelMatScaleArea.x + textAreaSize.x)
                     + guiDefaultPaddingSize
                     + (i * (valueBoxWidth + guiDefaultPaddingSize)),
                .y = guiModelMatScaleArea.y,
                .width = valueBoxWidth,
                .height = valueBoxHeight
            };

        for (int i = 0; i < 3; i++)
            strncpy(guiModelMatScaleValueText[i],
                    TextFormat("%.2f", guiModelMatScaleValues[i]),
                    LABEL_TEXT_LENGTH);
    }

    {
        guiModelMatTransArea = (Rectangle) {
            .x = guiModelMatScaleArea.x,
            .y = (guiModelMatScaleArea.y + guiModelMatScaleArea.height)
                 + guiDefaultPaddingSize,
            .width = guiModelMatScaleArea.width,
            .height = guiMatEntryAreaHeight
        };

        strncpy(guiModelMatTransLabelText,
                GuiIconText(ICON_CURSOR_MOVE, GUI_MODEL_MAT_TRANS_LABEL_TEXT),
                LABEL_TEXT_LENGTH);

        Vector2 textAreaSize = MeasureTextEx(GuiGetFont(),
                                             guiModelMatTransLabelText,
                                             GuiGetFont().baseSize,
                                             -2.0f);

        float valueBoxWidth = (((guiModelMatTransArea.width - textAreaSize.x)
                                - guiDefaultPaddingSize)
                               - (2.0f * guiDefaultPaddingSize))
                              / 3.0f;

        float valueBoxHeight = guiModelMatTransArea.height;

        for (int i = 0; i < 3; i++)
            guiModelMatTransValueBoxArea[i] = (Rectangle) {
                .x = (guiModelMatTransArea.x + textAreaSize.x)
                     + guiDefaultPaddingSize
                     + (i * (valueBoxWidth + guiDefaultPaddingSize)),
                .y = guiModelMatTransArea.y,
                .width = valueBoxWidth,
                .height = valueBoxHeight
            };

        for (int i = 0; i < 3; i++)
            strncpy(guiModelMatTransValueText[i],
                    TextFormat("%.2f", guiModelMatTransValues[i]),
                    LABEL_TEXT_LENGTH);
    }

    {
        guiModelMatRotateArea = (Rectangle) {
            .x = guiModelMatTransArea.x,
            .y = (guiModelMatTransArea.y + guiModelMatTransArea.height)
                 + guiDefaultPaddingSize,
            .width = guiModelMatTransArea.width,
            .height = guiMatEntryAreaHeight
        };

        strncpy(guiModelMatRotateLabelText,
                GuiIconText(ICON_ROTATE, GUI_MODEL_MAT_ROTATE_LABEL_TEXT),
                LABEL_TEXT_LENGTH);

        Vector2 textAreaSize = MeasureTextEx(GuiGetFont(),
                                             guiModelMatRotateLabelText,
                                             GuiGetFont().baseSize,
                                             -2.0f);

        float valueBoxWidth = (((guiModelMatRotateArea.width - textAreaSize.x)
                                - guiDefaultPaddingSize)
                               - (2.0f * guiDefaultPaddingSize))
                              / 3.0f;

        float valueBoxHeight = guiModelMatRotateArea.height;

        for (int i = 0; i < 3; i++)
            guiModelMatRotateValueBoxArea[i] = (Rectangle) {
                .x = (guiModelMatRotateArea.x + textAreaSize.x)
                     + guiDefaultPaddingSize
                     + (i * (valueBoxWidth + guiDefaultPaddingSize)),
                .y = guiModelMatRotateArea.y,
                .width = valueBoxWidth,
                .height = valueBoxHeight
            };

        for (int i = 0; i < 3; i++)
            strncpy(guiModelMatRotateValueText[i],
                    TextFormat("%.1f", guiModelMatRotateValues[i]),
                    LABEL_TEXT_LENGTH);
    }

    {
        guiModelMatArea.height += 3.0f
                                  * (guiMatEntryAreaHeight
                                     + guiDefaultPaddingSize);
    }

    guiViewMatArea = (Rectangle) {
        .x = 0.0f,
        .y = (guiModelMatArea.y + guiModelMatArea.height)
             - guiDefaultPaddingSize,
        .width = guiArea.width,
        .height = ((guiArea.y + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT)
                   + (2.5f * guiDefaultPaddingSize))
                  + ((4.0f * guiMatEntryAreaHeight)
                     + (4.0f * guiDefaultPaddingSize))
    };

    {
        for (int i = 0,
                 j = sizeof guiViewMatEntryArea / sizeof *guiViewMatEntryArea;
             i < j;
             i++) {
            float guiMatEntryPaddingWidth = ((i / 4)
                                             * (guiMatEntryAreaWidth
                                                + guiDefaultPaddingSize));

            float guiMatEntryPaddingHeight = ((i % 4)
                                              * (guiMatEntryAreaHeight
                                                 + guiDefaultPaddingSize));

            guiViewMatEntryArea[i] = (Rectangle) {
                .x = (guiViewMatArea.x + guiMatEntryOffsetX)
                     + guiMatEntryPaddingWidth,
                .y = (guiViewMatArea.y
                      + (RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT
                         + (1.25f * guiDefaultPaddingSize)))
                     + guiMatEntryPaddingHeight,
                .width = guiMatEntryAreaWidth,
                .height = guiMatEntryAreaHeight
            };
        }
    }

    {
        guiViewMatEyeArea = (Rectangle) {
            .x = guiViewMatArea.x + guiMatEntryOffsetX,
            .y = (guiViewMatArea.y
                  + (RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT
                     + (1.5f * guiDefaultPaddingSize)))
                 + 4.0f * (guiMatEntryAreaHeight + guiDefaultPaddingSize),
            .width = ((4.0f * guiMatEntryAreaWidth)
                      + (3.0f * guiDefaultPaddingSize)),
            .height = guiMatEntryAreaHeight
        };

        strncpy(guiViewMatEyeLabelText,
                GuiIconText(ICON_EYE_ON, GUI_VIEW_MAT_EYE_LABEL_TEXT),
                LABEL_TEXT_LENGTH);

        Vector2 textAreaSize = MeasureTextEx(GuiGetFont(),
                                             guiViewMatEyeLabelText,
                                             GuiGetFont().baseSize,
                                             -2.0f);

        float valueBoxWidth = (((guiViewMatEyeArea.width - textAreaSize.x)
                                - guiDefaultPaddingSize)
                               - (2.0f * guiDefaultPaddingSize))
                              / 3.0f;

        float valueBoxHeight = guiViewMatEyeArea.height;

        for (int i = 0; i < 3; i++)
            guiViewMatEyeValueBoxArea[i] = (Rectangle) {
                .x = (guiViewMatEyeArea.x + textAreaSize.x)
                     + guiDefaultPaddingSize
                     + (i * (valueBoxWidth + guiDefaultPaddingSize)),
                .y = guiViewMatEyeArea.y,
                .width = valueBoxWidth,
                .height = valueBoxHeight
            };
    }

    {
        guiViewMatAtArea = (Rectangle) { .x = guiViewMatEyeArea.x,
                                         .y = (guiViewMatEyeArea.y
                                               + guiViewMatEyeArea.height)
                                              + guiDefaultPaddingSize,
                                         .width = guiViewMatEyeArea.width,
                                         .height = guiMatEntryAreaHeight };

        strncpy(guiViewMatAtLabelText,
                GuiIconText(ICON_TARGET, GUI_VIEW_MAT_AT_LABEL_TEXT),
                LABEL_TEXT_LENGTH);

        Vector2 textAreaSize = MeasureTextEx(GuiGetFont(),
                                             guiViewMatAtLabelText,
                                             GuiGetFont().baseSize,
                                             -2.0f);

        float valueBoxWidth = (((guiViewMatAtArea.width - textAreaSize.x)
                                - guiDefaultPaddingSize)
                               - (2.0f * guiDefaultPaddingSize))
                              / 3.0f;

        float valueBoxHeight = guiViewMatAtArea.height;

        for (int i = 0; i < 3; i++)
            guiViewMatAtValueBoxArea[i] = (Rectangle) {
                .x = (guiViewMatAtArea.x + textAreaSize.x)
                     + guiDefaultPaddingSize
                     + (i * (valueBoxWidth + guiDefaultPaddingSize)),
                .y = guiViewMatAtArea.y,
                .width = valueBoxWidth,
                .height = valueBoxHeight
            };
    }

    {
        guiViewMatUpArea = (Rectangle) { .x = guiViewMatAtArea.x,
                                         .y = (guiViewMatAtArea.y
                                               + guiViewMatAtArea.height)
                                              + guiDefaultPaddingSize,
                                         .width = guiViewMatAtArea.width,
                                         .height = guiMatEntryAreaHeight };

        strncpy(guiViewMatUpLabelText,
                GuiIconText(ICON_ARROW_UP, GUI_VIEW_MAT_UP_LABEL_TEXT),
                LABEL_TEXT_LENGTH);

        Vector2 textAreaSize = MeasureTextEx(GuiGetFont(),
                                             guiViewMatUpLabelText,
                                             GuiGetFont().baseSize,
                                             -2.0f);

        float valueBoxWidth = (((guiViewMatUpArea.width - textAreaSize.x)
                                - guiDefaultPaddingSize)
                               - (2.0f * guiDefaultPaddingSize))
                              / 3.0f;

        float valueBoxHeight = guiViewMatUpArea.height;

        for (int i = 0; i < 3; i++)
            guiViewMatUpValueBoxArea[i] = (Rectangle) {
                .x = (guiViewMatUpArea.x + textAreaSize.x)
                     + guiDefaultPaddingSize
                     + (i * (valueBoxWidth + guiDefaultPaddingSize)),
                .y = guiViewMatUpArea.y,
                .width = valueBoxWidth,
                .height = valueBoxHeight
            };
    }

    {
        guiViewMatArea.height += 3.0f
                                 * (guiMatEntryAreaHeight
                                    + guiDefaultPaddingSize);
    }

    guiProjMatArea = (Rectangle) {
        .x = 0.0f,
        .y = (guiViewMatArea.y + guiViewMatArea.height) - guiDefaultPaddingSize,
        .width = guiArea.width,
        .height = ((guiArea.y + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT)
                   + (2.5f * guiDefaultPaddingSize))
                  + ((4.0f * guiMatEntryAreaHeight)
                     + (4.0f * guiDefaultPaddingSize))
    };

    {
        for (int i = 0,
                 j = sizeof guiProjMatEntryArea / sizeof *guiProjMatEntryArea;
             i < j;
             i++) {
            float guiMatEntryPaddingWidth = ((i / 4)
                                             * (guiMatEntryAreaWidth
                                                + guiDefaultPaddingSize));

            float guiMatEntryPaddingHeight = ((i % 4)
                                              * (guiMatEntryAreaHeight
                                                 + guiDefaultPaddingSize));

            guiProjMatEntryArea[i] = (Rectangle) {
                .x = (guiProjMatArea.x + guiMatEntryOffsetX)
                     + guiMatEntryPaddingWidth,
                .y = (guiProjMatArea.y
                      + (RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT
                         + (1.25f * guiDefaultPaddingSize)))
                     + guiMatEntryPaddingHeight,
                .width = guiMatEntryAreaWidth,
                .height = guiMatEntryAreaHeight
            };
        }
    }

    {
        guiProjMatFovArea = (Rectangle) {
            .x = guiProjMatArea.x + guiMatEntryOffsetX,
            .y = (guiProjMatArea.y
                  + (RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT
                     + (1.5f * guiDefaultPaddingSize)))
                 + 4.0f * (guiMatEntryAreaHeight + guiDefaultPaddingSize),
            .width = ((4.0f * guiMatEntryAreaWidth)
                      + (3.0f * guiDefaultPaddingSize)),
            .height = guiMatEntryAreaHeight
        };

        strncpy(guiProjMatFovLabelText,
                GuiIconText(ICON_LENS_BIG, GUI_PROJ_MAT_FOV_LABEL_TEXT),
                LABEL_TEXT_LENGTH);

        Vector2 textAreaSize = MeasureTextEx(GuiGetFont(),
                                             guiProjMatFovLabelText,
                                             GuiGetFont().baseSize,
                                             -2.0f);

        guiProjMatFovValueBoxArea[0] = (Rectangle) {
            .x = (guiProjMatFovArea.x + textAreaSize.x) + guiDefaultPaddingSize,
            .y = guiProjMatFovArea.y,
            .width = (guiProjMatFovArea.width - textAreaSize.x)
                     - guiDefaultPaddingSize,
            .height = guiProjMatFovArea.height
        };
    }

    {
        guiProjMatAspectArea = (Rectangle) { .x = guiProjMatFovArea.x,
                                             .y = (guiProjMatFovArea.y
                                                   + guiProjMatFovArea.height)
                                                  + guiDefaultPaddingSize,
                                             .width = guiProjMatFovArea.width,
                                             .height = guiMatEntryAreaHeight };

        strncpy(guiProjMatAspectLabelText,
                GuiIconText(ICON_ZOOM_ALL, "Aspect:  "),
                LABEL_TEXT_LENGTH);

        Vector2 textAreaSize = MeasureTextEx(GuiGetFont(),
                                             guiProjMatAspectLabelText,
                                             GuiGetFont().baseSize,
                                             -2.0f);

        guiProjMatAspectValueBoxArea[0] = (Rectangle) {
            .x = (guiProjMatAspectArea.x + textAreaSize.x)
                 + guiDefaultPaddingSize,
            .y = guiProjMatAspectArea.y,
            .width = (guiProjMatAspectArea.width - textAreaSize.x)
                     - guiDefaultPaddingSize,
            .height = guiProjMatAspectArea.height
        };
    }

    {
        guiProjMatNearFarArea = (Rectangle) {
            .x = guiProjMatAspectArea.x,
            .y = (guiProjMatAspectArea.y + guiProjMatAspectArea.height)
                 + guiDefaultPaddingSize,
            .width = guiProjMatAspectArea.width,
            .height = guiMatEntryAreaHeight
        };

        strncpy(guiProjMatNearFarLabelText,
                GuiIconText(ICON_CUBE, GUI_PROJ_MAT_NEAR_FAR_LABEL_TEXT),
                LABEL_TEXT_LENGTH);

        Vector2 textAreaSize = MeasureTextEx(GuiGetFont(),
                                             guiProjMatNearFarLabelText,
                                             GuiGetFont().baseSize,
                                             -2.0f);

        float valueBoxWidth = (((guiProjMatNearFarArea.width - textAreaSize.x)
                                - guiDefaultPaddingSize)
                               - (1.0f * guiDefaultPaddingSize))
                              / 2.0f;

        float valueBoxHeight = guiProjMatNearFarArea.height;

        for (int i = 0; i < 2; i++)
            guiProjMatNearFarValueBoxArea[i] = (Rectangle) {
                .x = (guiProjMatNearFarArea.x + textAreaSize.x)
                     + guiDefaultPaddingSize
                     + (i * (valueBoxWidth + guiDefaultPaddingSize)),
                .y = guiProjMatNearFarArea.y,
                .width = valueBoxWidth,
                .height = valueBoxHeight
            };
    }

    {
        guiProjMatArea.height += 3.0f
                                 * (guiMatEntryAreaHeight
                                    + guiDefaultPaddingSize);
    }

    {
        Vector2 textAreaSize = MeasureTextEx(GuiGetFont(),
                                             magicNumbers + 6,
                                             GuiGetFont().baseSize,
                                             -2.0f);

        guiReservedArea = (Rectangle) {
            .x = guiDefaultPaddingSize,
            .y = (guiArea.height - RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT)
                 - 0.25f
                       * ((guiArea.height - RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT)
                          - (guiProjMatArea.y + guiProjMatArea.height)),
            .width = guiArea.width - (2.0f * guiDefaultPaddingSize),
            .height = textAreaSize.y
        };
    }
}

/* "모델 행렬"을 단위 행렬로 초기화하는 함수 */
static void ResetModelMatrix(void) {
    for (int i = 0; i < 3; i++) {
        guiModelMatScaleValues[i] = 1.0f;
        guiModelMatRotateValues[i] = 0.0f;
        guiModelMatTransValues[i] = 0.0f;
    }

    gameObjects[OBJ_TYPE_PLAYER].model.transform = MatrixIdentity();

    UpdateModelMatrix(false);
}

/* 행렬의 각 요소를 나타내는 문자열을 업데이트하는 함수 */
static void UpdateMatrixEntryText(char (*matEntryText)[16], Matrix matrix) {
    if (matEntryText == NULL) return;

    strncpy(matEntryText[0],
            TextFormat("%.2f", matrix.m0),
            MATRIX_VALUE_TEXT_LENGTH);
    strncpy(matEntryText[1],
            TextFormat("%.2f", matrix.m1),
            MATRIX_VALUE_TEXT_LENGTH);
    strncpy(matEntryText[2],
            TextFormat("%.2f", matrix.m2),
            MATRIX_VALUE_TEXT_LENGTH);
    strncpy(matEntryText[3],
            TextFormat("%.2f", matrix.m3),
            MATRIX_VALUE_TEXT_LENGTH);
    strncpy(matEntryText[4],
            TextFormat("%.2f", matrix.m4),
            MATRIX_VALUE_TEXT_LENGTH);
    strncpy(matEntryText[5],
            TextFormat("%.2f", matrix.m5),
            MATRIX_VALUE_TEXT_LENGTH);
    strncpy(matEntryText[6],
            TextFormat("%.2f", matrix.m6),
            MATRIX_VALUE_TEXT_LENGTH);
    strncpy(matEntryText[7],
            TextFormat("%.2f", matrix.m7),
            MATRIX_VALUE_TEXT_LENGTH);
    strncpy(matEntryText[8],
            TextFormat("%.2f", matrix.m8),
            MATRIX_VALUE_TEXT_LENGTH);
    strncpy(matEntryText[9],
            TextFormat("%.2f", matrix.m9),
            MATRIX_VALUE_TEXT_LENGTH);
    strncpy(matEntryText[10],
            TextFormat("%.2f", matrix.m10),
            MATRIX_VALUE_TEXT_LENGTH);
    strncpy(matEntryText[11],
            TextFormat("%.2f", matrix.m11),
            MATRIX_VALUE_TEXT_LENGTH);
    strncpy(matEntryText[12],
            TextFormat("%.2f", matrix.m12),
            MATRIX_VALUE_TEXT_LENGTH);
    strncpy(matEntryText[13],
            TextFormat("%.2f", matrix.m13),
            MATRIX_VALUE_TEXT_LENGTH);
    strncpy(matEntryText[14],
            TextFormat("%.2f", matrix.m14),
            MATRIX_VALUE_TEXT_LENGTH);
    strncpy(matEntryText[15],
            TextFormat("%.2f", matrix.m15),
            MATRIX_VALUE_TEXT_LENGTH);
}