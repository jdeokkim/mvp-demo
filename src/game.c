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
static const InitSpaceFunc initSpaceFuncs[MODE_COUNT_] = {
    [MODE_LOCAL] = InitLocalSpace,
    [MODE_WORLD] = InitWorldSpace,
    [MODE_VIEW] = InitViewSpace,
    [MODE_CLIP] = InitClipSpace
};

/* 프레임버퍼에 "<물체 / 세계 / 카메라> 공간"을 그리는 함수들 */
static const UpdateSpaceFunc updateSpaceFuncs[MODE_COUNT_] = {
    [MODE_LOCAL] = UpdateLocalSpace,
    [MODE_WORLD] = UpdateWorldSpace,
    [MODE_VIEW] = UpdateViewSpace,
    [MODE_CLIP] = UpdateClipSpace
};

/* "<물체 / 세계 / 카메라> 공간"에 필요한 메모리 공간을 해제하는 함수들 */
static const DeinitSpaceFunc deinitSpaceFuncs[MODE_COUNT_] = {
    [MODE_LOCAL] = DeinitLocalSpace,
    [MODE_WORLD] = DeinitWorldSpace,
    [MODE_VIEW] = DeinitViewSpace,
    [MODE_CLIP] = DeinitClipSpace
};

/* Private Variables ======================================================= */

/* 게임 세계에 존재하는 물체들 */
static GameObject gameObjects[GAME_OBJECT_COUNT];

/* 게임 화면의 오른쪽 영역을 4개로 분할하고, 렌더 텍스처를 그림 */
static RenderTexture renderTextures[MODE_COUNT_];

/* MVP 영역에 그릴 화면의 종류 */
static MvpRenderMode renderMode = MODE_ALL;

/* 컴파일 및 링크 과정을 거친 공용 셰이더 프로그램 */
static Shader shaderProgram = { .id = 0 };

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

/* Public Functions ======================================================== */

/* 게임 화면을 초기화하는 함수 */
void InitGameScreen(void) {
    for (int i = MODE_ALL + 1; i < MODE_COUNT_; i++) {
        if (initSpaceFuncs[i] == NULL) continue;

        renderTextures[i] = LoadRenderTexture(mvpArea.width, mvpArea.height);

        initSpaceFuncs[i]();
    }

    shaderProgram = LoadCommonShader();

    GenerateGameObjects();
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
    /* TODO: ... */

    UnloadShader(shaderProgram);

    for (int i = MODE_ALL + 1; i < MODE_COUNT_; i++) {
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
    return (index >= 0 && index < GAME_OBJECT_COUNT) 
        ? &gameObjects[index] 
        : NULL;
}

/* MVP 영역에 그릴 화면의 종류를 반환하는 함수 */
MvpRenderMode GetRenderMode(void) {
    return renderMode;
}

/* Private Functions ======================================================= */

/* 게임 화면의 왼쪽 영역을 그리는 함수 */
static void DrawGuiArea(void) {
    // TODO: ...
    DrawRectangleRec(guiArea, ColorAlpha(DARKGRAY, 0.15f));

    {
        /* MVP 영역에 그릴 화면의 종류 표시 */

        float renderModeAlpha = 1.0f
                                - (renderModeCounter
                                   / MVP_RENDER_MODE_ANIMATION_DURATION);

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
        if (renderModeCounter < MVP_RENDER_MODE_ANIMATION_DURATION)
            renderModeCounter += GetFrameTime();
    }

    {
        for (int i = MODE_ALL + 1; i < MODE_COUNT_; i++) {
            if (updateSpaceFuncs[i] == NULL) continue;

            updateSpaceFuncs[i](renderTextures[i]);

            // 텍스처 필터링 (이중 선형 필터링)
            SetTextureFilter(renderTextures[i].texture,
                             TEXTURE_FILTER_BILINEAR);
        }
    }

    // 전부 다 그리기 vs. 하나만 그리기
    if (renderMode == MODE_ALL) {
        for (int i = MODE_ALL + 1; i < MODE_COUNT_; i++) {
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
                               .y = mvpArea.y + ((i > MODE_WORLD) * halfHeight),
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

    if (renderMode == MODE_ALL) {
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
        if (i == 0) {
            gameObjects[i].color = ColorAlpha(GOLD, 1.0f);
        } else {

        }
    }
}

/* 마우스 및 키보드 입력을 처리하는 함수 */
static void HandleInputEvents(void) {
    {
        /* MVP 영역에 그릴 화면 종류 변경 */

        int keyCode = GetKeyPressed();

        if (keyCode >= '0' + MODE_ALL && keyCode < '0' + MODE_COUNT_)
            renderMode = keyCode - '0', renderModeCounter = 0.0f;
    }
}