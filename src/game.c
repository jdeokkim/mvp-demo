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

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "raygui_style_darkr.h"

/* Constants =============================================================== */

/* 게임 화면의 왼쪽 (GUI 패널) 영역 */
static const Rectangle guiArea = {
    .x = 0.0f, .y = 0.0f, .width = 0.2f * SCREEN_WIDTH, .height = SCREEN_HEIGHT
};

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

/* NOTE: CC0 License (https://pixelfrog-assets.itch.io/pixel-adventure-1) */
static const char *terrainImageFileName = "res/images/terrain-16x16.png";

/* Private Variables ======================================================= */

/* GUI 패널의 "모델 행렬" 영역 */
static Rectangle guiModelMatArea;

/* "모델 행렬"의 각 요소를 그릴 영역 */
static Rectangle guiModelMatEntryArea[16];

/* "모델 행렬"의 각 요소를 나타내는 문자열 */
static char guiModelMatEntryText[16][MATRIX_ENTRY_STRING_LENGTH];

/* "모델 행렬"의 크기 변환 영역 */
static Rectangle guiModelMatScaleArea;

/* "모델 행렬"의 크기 변환 레이블 영역 */
static char guiModelMatScaleLabelText[LABEL_STRING_LENGTH];

/* "모델 행렬"의 크기 변환을 위한 입력 상자 영역 */
static Rectangle guiModelMatScaleValueBoxArea;

/* "모델 행렬"의 이동 변환 영역 */
static Rectangle guiModelMatTranslateArea;

/* "모델 행렬"의 이동 변환 레이블 영역 */
static char guiModelMatTranslateLabelText[LABEL_STRING_LENGTH];

/* "모델 행렬"의 이동 변환을 위한 입력 상자 영역 */
static Rectangle guiModelMatTranslateValueBoxArea;

/* "모델 행렬"의 회전 변환 영역 */
static Rectangle guiModelMatRotateArea;

/* "모델 행렬"의 회전 변환 레이블 영역 */
static char guiModelMatRotateLabelText[LABEL_STRING_LENGTH];

/* "모델 행렬"의 회전 변환을 위한 입력 상자 영역 */
static Rectangle guiModelMatRotateValueBoxArea;

/* ========================================================================= */

/* GUI 패널의 "뷰 행렬" 영역 */
static Rectangle guiViewMatArea;

/* "뷰 행렬"의 각 요소를 그릴 영역 */
static Rectangle guiViewMatEntryArea[16];

/* "뷰 행렬"의 각 요소를 나타내는 문자열 */
static char guiViewMatEntryText[16][MATRIX_ENTRY_STRING_LENGTH];

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
static Texture cameraTexture, enemyTexture, playerTexture;

/* `renderMode` 텍스트의 애니메이션 길이 */
static float renderModeCounter = 0.0f;

/* Private Function Prototypes ============================================= */

/* 게임 화면의 왼쪽 영역을 그리는 함수 */
static void DrawGuiArea(void);

/* 게임 화면의 오른쪽 영역을 그리는 함수 */
static void DrawMvpArea(void);

/* 게임 세계의 무작위 위치에 물체를 생성하는 함수 */
static void GenerateGameObjects(void);

/* 마우스 및 키보드 입력을 처리하는 함수 */
static void HandleInputEvents(void);

/* GUI 패널에 그릴 위젯들의 영역을 정의하는 함수 */
static void InitGuiAreas(void);

/* 행렬의 각 요소를 나타내는 문자열을 업데이트하는 함수 */
static void UpdateMatrixEntries(char (*matEntryText)[16], Matrix matrix);

/* Public Functions ======================================================== */

/* 게임 화면을 초기화하는 함수 */
void InitGameScreen(void) {
    GuiLoadStyleDarkr();

    InitGuiAreas();

    for (int i = MVP_RENDER_ALL + 1; i < MVP_RENDER_COUNT_; i++) {
        if (initSpaceFuncs[i] == NULL) continue;

        renderTextures[i] = LoadRenderTexture(mvpArea.width, mvpArea.height);

        initSpaceFuncs[i]();
    }

    shaderProgram = LoadCommonShader();

    {
        Image terrainImage = LoadImage(terrainImageFileName);

        {
            Image terrainImageCopy = ImageCopy(terrainImage);

            ImageCrop(&terrainImageCopy,
                      (Rectangle) { .x = 208.0f,
                                    .y = 80.0f,
                                    .width = 32.0f,
                                    .height = 32.0f });

            cameraTexture = LoadTextureFromImage(terrainImageCopy);

            UnloadImage(terrainImageCopy);
        }

        {
            Image terrainImageCopy = ImageCopy(terrainImage);

            ImageCrop(&terrainImageCopy,
                      (Rectangle) { .x = 272.0f,
                                    .y = 64.0f,
                                    .width = 48.0f,
                                    .height = 48.0f });

            enemyTexture = LoadTextureFromImage(terrainImageCopy);

            UnloadImage(terrainImageCopy);
        }

        {
            Image terrainImageCopy = ImageCopy(terrainImage);

            ImageCrop(&terrainImageCopy,
                      (Rectangle) { .x = 288.0f,
                                    .y = 144.0f,
                                    .width = 32.0f,
                                    .height = 32.0f });

            playerTexture = LoadTextureFromImage(terrainImageCopy);

            UnloadImage(terrainImageCopy);
        }

        UnloadImage(terrainImage);
    }

    GenerateGameObjects();

    {
        UpdateMatrixEntries(guiModelMatEntryText,
                            gameObjects[OBJ_TYPE_PLAYER].model.transform);
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
    }

    // 이중 버퍼링 (double buffering) 기법으로 프레임버퍼 교체
    EndDrawing();
}

/* 게임 화면에 필요한 메모리 공간을 해제하는 함수 */
void DeinitGameScreen(void) {
    UnloadFont(GuiGetFont());

    {
        UnloadTexture(cameraTexture);
        UnloadTexture(playerTexture);
        UnloadTexture(enemyTexture);
    }

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

/* Private Functions ======================================================= */

/* 게임 화면의 왼쪽 영역을 그리는 함수 */
static void DrawGuiArea(void) {
    {
        int tmpTextAlignment = GuiGetStyle(STATUSBAR, TEXT_ALIGNMENT);

        {
            // 패널의 상태 표시줄 텍스트를 가운데 정렬
            GuiSetStyle(STATUSBAR, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);

            GuiPanel(guiArea, "MVP Transform Visualizer Demo");

            GuiSetStyle(STATUSBAR, TEXT_ALIGNMENT, tmpTextAlignment);
        }

        Matrix modelMat = gameObjects[OBJ_TYPE_PLAYER].model.transform;

        {
            GuiPanel(guiModelMatArea, "Model Matrix");

            for (int i = 0; i < 16; i++)
                GuiTextBox(guiModelMatEntryArea[i],
                           guiModelMatEntryText[i],
                           MATRIX_ENTRY_STRING_LENGTH,
                           false);

            GuiLabel(guiModelMatScaleArea, guiModelMatScaleLabelText);
            DrawRectangleRec(guiModelMatScaleValueBoxArea, WHITE);

            GuiLabel(guiModelMatTranslateArea, guiModelMatTranslateLabelText);
            DrawRectangleRec(guiModelMatTranslateValueBoxArea, WHITE);

            GuiLabel(guiModelMatRotateArea, guiModelMatRotateLabelText);
            DrawRectangleRec(guiModelMatRotateValueBoxArea, WHITE);
        }

        // TODO: ...

        {
            GuiPanel(guiViewMatArea, "View Matrix");

            for (int i = 0; i < 16; i++)
                GuiTextBox(guiViewMatEntryArea[i],
                           guiViewMatEntryText[i],
                           MATRIX_ENTRY_STRING_LENGTH,
                           false);
        }
    }

    {
        /* MVP 영역에 그릴 화면의 종류 표시 */

        float renderModeAlpha = 1.0f
                                - (renderModeCounter
                                   / RENDER_MODE_ANIMATION_DURATION);

        DrawTextEx(GetFontDefault(),
                   TextFormat("%d", renderMode),
                   (Vector2) { .x = guiArea.x + 8.0f, .y = 8.0f },
                   (GetFontDefault().baseSize << 1),
                   1.0f,
                   ColorAlpha(DARKGRAY, renderModeAlpha));
    }
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

/* 게임 세계의 무작위 위치에 물체를 생성하는 함수 */
static void GenerateGameObjects(void) {
    for (int i = 0; i < GAME_OBJECT_COUNT; i++) {
        if (i == OBJ_TYPE_CAMERA) {
            Mesh cameraMesh = GenMeshCylinder(0.5f, 0.4f, 6);

            gameObjects[i].model = LoadModelFromMesh(cameraMesh);

            gameObjects[i].model.transform =
                MatrixMultiply(MatrixRotateZ(55.0f * DEG2RAD),
                               MatrixTranslate(-3.0f, 2.25f, 0.0f));

            gameObjects[i]
                .model.materials[0]
                .maps[MATERIAL_MAP_DIFFUSE]
                .texture = cameraTexture;
        } else if (i == OBJ_TYPE_PLAYER) {
            Mesh playerMesh = GenMeshCube(1.0f, 1.0f, 1.0f);

            gameObjects[i].model = LoadModelFromMesh(playerMesh);

            gameObjects[i].model.transform = MatrixTranslate(2.0f, 0.5f, 2.0f);

            gameObjects[i]
                .model.materials[0]
                .maps[MATERIAL_MAP_DIFFUSE]
                .texture = playerTexture;
        } else {
            Mesh enemyMesh = GenMeshCube(0.5f, 0.5f, 0.5f);

            gameObjects[i].model = LoadModelFromMesh(enemyMesh);

            gameObjects[i].model.transform =
                MatrixMultiply(MatrixRotateY(GetRandomValue(45, 275) * DEG2RAD),
                               MatrixTranslate(0.75f, 0.25f, 1.0f));

            gameObjects[i]
                .model.materials[0]
                .maps[MATERIAL_MAP_DIFFUSE]
                .texture = enemyTexture;
        }
    }
}

/* 마우스 및 키보드 입력을 처리하는 함수 */
static void HandleInputEvents(void) {
    {
        /* MVP 영역에 그릴 화면 종류 변경 */

        if (IsKeyDown(KEY_LEFT_CONTROL)) {
            int keyCode = GetKeyPressed();

            if (keyCode >= '0' + MVP_RENDER_ALL
                && keyCode < '0' + MVP_RENDER_COUNT_)
                renderMode = keyCode - '0', renderModeCounter = 0.0f;
        }
    }
}

/* GUI 패널에 그릴 위젯들의 영역을 정의하는 함수 */
static void InitGuiAreas(void) {
    float guiDefaultPaddingSize = 8.0f;

    float guiMatEntryAreaWidth = 48.0f;
    float guiMatEntryAreaHeight = 24.0f;

    guiModelMatArea = (Rectangle) {
        .x = guiDefaultPaddingSize,
        .y = (guiArea.y + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT)
             + guiDefaultPaddingSize,
        .width = guiArea.width - (2.0f * guiDefaultPaddingSize),
        .height = ((guiArea.y + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT)
                   + guiDefaultPaddingSize)
                  + ((4.0f * guiMatEntryAreaHeight)
                     + (4.0f * guiDefaultPaddingSize))
    };

    float guiMatEntryOffsetX = 0.5f
                               * (guiModelMatArea.width
                                  - ((4.0f * guiMatEntryAreaWidth)
                                     + (3.0f * guiDefaultPaddingSize)));

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
                  + (RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT + guiDefaultPaddingSize))
                 + guiMatEntryPaddingHeight,
            .width = guiMatEntryAreaWidth,
            .height = guiMatEntryAreaHeight
        };
    }

    {
        guiModelMatScaleArea = (Rectangle) {
            .x = guiModelMatArea.x + guiMatEntryOffsetX,
            .y = (guiModelMatArea.y
                  + (RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT + guiDefaultPaddingSize))
                 + 4.0f * (guiMatEntryAreaHeight + guiDefaultPaddingSize),
            .width = guiModelMatArea.width - (3.0f * guiDefaultPaddingSize),
            .height = guiMatEntryAreaHeight
        };

        strncpy(guiModelMatScaleLabelText,
                GuiIconText(ICON_CURSOR_SCALE, "Scale:"),
                LABEL_STRING_LENGTH);

        Vector2 textAreaSize = MeasureTextEx(GuiGetFont(),
                                             guiModelMatScaleLabelText,
                                             GuiGetFont().baseSize,
                                             -2.0f);

        guiModelMatScaleValueBoxArea = (Rectangle) {
            .x = (guiModelMatScaleArea.x + textAreaSize.x)
                 + guiDefaultPaddingSize,
            .y = guiModelMatScaleArea.y,
            .width = (guiModelMatScaleArea.width - textAreaSize.x)
                     - guiDefaultPaddingSize,
            .height = guiModelMatScaleArea.height
        };
    }

    {
        guiModelMatTranslateArea = (Rectangle) {
            .x = guiModelMatScaleArea.x,
            .y = (guiModelMatScaleArea.y + guiModelMatScaleArea.height)
                 + guiDefaultPaddingSize,
            .width = guiModelMatScaleArea.width,
            .height = guiMatEntryAreaHeight
        };

        strncpy(guiModelMatTranslateLabelText,
                GuiIconText(ICON_CURSOR_MOVE, "Trans:"),
                LABEL_STRING_LENGTH);

        Vector2 textAreaSize = MeasureTextEx(GuiGetFont(),
                                             guiModelMatTranslateLabelText,
                                             GuiGetFont().baseSize,
                                             -2.0f);

        guiModelMatTranslateValueBoxArea = (Rectangle) {
            .x = (guiModelMatTranslateArea.x + textAreaSize.x)
                 + guiDefaultPaddingSize,
            .y = guiModelMatTranslateArea.y,
            .width = (guiModelMatTranslateArea.width - textAreaSize.x)
                     - guiDefaultPaddingSize,
            .height = guiModelMatTranslateArea.height
        };
    }

    {
        guiModelMatRotateArea = (Rectangle) {
            .x = guiModelMatTranslateArea.x,
            .y = (guiModelMatTranslateArea.y + guiModelMatTranslateArea.height)
                 + guiDefaultPaddingSize,
            .width = guiModelMatTranslateArea.width,
            .height = guiMatEntryAreaHeight
        };

        strncpy(guiModelMatRotateLabelText,
                GuiIconText(ICON_ROTATE, "Rotate:"),
                LABEL_STRING_LENGTH);

        Vector2 textAreaSize = MeasureTextEx(GuiGetFont(),
                                             guiModelMatRotateLabelText,
                                             GuiGetFont().baseSize,
                                             -2.0f);

        guiModelMatRotateValueBoxArea = (Rectangle) {
            .x = (guiModelMatRotateArea.x + textAreaSize.x)
                 + guiDefaultPaddingSize,
            .y = guiModelMatRotateArea.y,
            .width = (guiModelMatRotateArea.width - textAreaSize.x)
                     - guiDefaultPaddingSize,
            .height = guiModelMatRotateArea.height
        };
    }

    guiModelMatArea.height += 3.0f
                              * (guiMatEntryAreaHeight + guiDefaultPaddingSize);

    guiViewMatArea = (Rectangle) {
        .x = guiDefaultPaddingSize,
        .y = (guiModelMatArea.y + guiModelMatArea.height)
             + guiDefaultPaddingSize,
        .width = guiArea.width - (2.0f * guiDefaultPaddingSize),
        .height = ((guiArea.y + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT)
                   + guiDefaultPaddingSize)
                  + ((4.0f * guiMatEntryAreaHeight)
                     + (4.0f * guiDefaultPaddingSize))
    };

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
                  + (RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT + guiDefaultPaddingSize))
                 + guiMatEntryPaddingHeight,
            .width = guiMatEntryAreaWidth,
            .height = guiMatEntryAreaHeight
        };
    }
}

/* 행렬의 각 요소를 나타내는 문자열을 업데이트하는 함수 */
static void UpdateMatrixEntries(char (*matEntryText)[16], Matrix matrix) {
    if (matEntryText == NULL) return;

    strncpy(matEntryText[0],
            TextFormat("%.2f", matrix.m0),
            MATRIX_ENTRY_STRING_LENGTH);
    strncpy(matEntryText[1],
            TextFormat("%.2f", matrix.m1),
            MATRIX_ENTRY_STRING_LENGTH);
    strncpy(matEntryText[2],
            TextFormat("%.2f", matrix.m2),
            MATRIX_ENTRY_STRING_LENGTH);
    strncpy(matEntryText[3],
            TextFormat("%.2f", matrix.m3),
            MATRIX_ENTRY_STRING_LENGTH);
    strncpy(matEntryText[4],
            TextFormat("%.2f", matrix.m4),
            MATRIX_ENTRY_STRING_LENGTH);
    strncpy(matEntryText[5],
            TextFormat("%.2f", matrix.m5),
            MATRIX_ENTRY_STRING_LENGTH);
    strncpy(matEntryText[6],
            TextFormat("%.2f", matrix.m6),
            MATRIX_ENTRY_STRING_LENGTH);
    strncpy(matEntryText[7],
            TextFormat("%.2f", matrix.m7),
            MATRIX_ENTRY_STRING_LENGTH);
    strncpy(matEntryText[8],
            TextFormat("%.2f", matrix.m8),
            MATRIX_ENTRY_STRING_LENGTH);
    strncpy(matEntryText[9],
            TextFormat("%.2f", matrix.m9),
            MATRIX_ENTRY_STRING_LENGTH);
    strncpy(matEntryText[10],
            TextFormat("%.2f", matrix.m10),
            MATRIX_ENTRY_STRING_LENGTH);
    strncpy(matEntryText[11],
            TextFormat("%.2f", matrix.m11),
            MATRIX_ENTRY_STRING_LENGTH);
    strncpy(matEntryText[12],
            TextFormat("%.2f", matrix.m12),
            MATRIX_ENTRY_STRING_LENGTH);
    strncpy(matEntryText[13],
            TextFormat("%.2f", matrix.m13),
            MATRIX_ENTRY_STRING_LENGTH);
    strncpy(matEntryText[14],
            TextFormat("%.2f", matrix.m14),
            MATRIX_ENTRY_STRING_LENGTH);
    strncpy(matEntryText[15],
            TextFormat("%.2f", matrix.m15),
            MATRIX_ENTRY_STRING_LENGTH);
}