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
static const Rectangle gsGuiArea = {
    .x = 0.0f, .y = 0.0f, .width = 0.2f * SCREEN_WIDTH, .height = SCREEN_HEIGHT
};

/* 게임 화면의 오른쪽 (MVP 시각화) 영역 */
static const Rectangle gsMvpArea = { .x = 0.2f * SCREEN_WIDTH,
                                     .y = 0.0f,
                                     .width = 0.8f * SCREEN_WIDTH,
                                     .height = SCREEN_HEIGHT };

/* "<물체 / 세계 / 카메라> 공간"을 초기화하는 함수들 */
static const InitSpaceFunc gsInitSpaceFuncs[MODE_COUNT_] = {
    [MODE_LOCAL] = InitLocalSpace,
    [MODE_WORLD] = InitWorldSpace,
    [MODE_VIEW] = InitViewSpace,
    [MODE_CLIP] = InitClipSpace
};

/* 프레임버퍼에 "<물체 / 세계 / 카메라> 공간"을 그리는 함수들 */
static const UpdateSpaceFunc gsUpdateSpaceFuncs[MODE_COUNT_] = {
    [MODE_LOCAL] = UpdateLocalSpace,
    [MODE_WORLD] = UpdateWorldSpace,
    [MODE_VIEW] = UpdateViewSpace,
    [MODE_CLIP] = UpdateClipSpace
};

/* "<물체 / 세계 / 카메라> 공간"에 필요한 메모리 공간을 해제하는 함수들 */
static const DeinitSpaceFunc gsDeinitSpaceFuncs[MODE_COUNT_] = {
    [MODE_LOCAL] = DeinitLocalSpace,
    [MODE_WORLD] = DeinitWorldSpace,
    [MODE_VIEW] = DeinitViewSpace,
    [MODE_CLIP] = DeinitClipSpace
};

/* Private Variables ======================================================= */

/* 게임 화면의 오른쪽 영역을 4개로 분할하고, 렌더 텍스처를 그림 */
static RenderTexture gsRenderTextures[MODE_COUNT_];

/* MVP 영역에 그릴 화면의 종류 */
static MvpRenderMode gsRenderMode = MODE_ALL;

/* `gsRenderMode` 텍스트의 애니메이션 길이 */
static float gsRenderModeCounter = 0.0f;

/* Private Function Prototypes ============================================= */

/* 게임 화면의 왼쪽 영역을 그리는 함수 */
static void DrawGuiArea(void);

/* 게임 화면의 오른쪽 영역을 그리는 함수 */
static void DrawMvpArea(void);

/* 마우스 및 키보드 입력을 처리하는 함수 */
static void HandleInputEvents(void);

/* Public Functions ======================================================== */

/* 게임 화면을 초기화하는 함수 */
void InitGameScreen(void) {
    for (int i = MODE_ALL + 1; i < MODE_COUNT_; i++) {
        if (gsInitSpaceFuncs[i] == NULL) continue;

        gsRenderTextures[i] = LoadRenderTexture(gsMvpArea.width,
                                                gsMvpArea.height);

        gsInitSpaceFuncs[i]();
    }

    /* TODO: ... */
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

    for (int i = MODE_ALL + 1; i < MODE_COUNT_; i++) {
        if (gsDeinitSpaceFuncs[i] == NULL) continue;

        UnloadRenderTexture(gsRenderTextures[i]);

        gsDeinitSpaceFuncs[i]();
    }
}

/* ========================================================================= */

/* Private Functions ======================================================= */

/* 게임 화면의 왼쪽 영역을 그리는 함수 */
static void DrawGuiArea(void) {
    // TODO: ...
    DrawRectangleRec(gsGuiArea, ColorAlpha(DARKGRAY, 0.15f));

    {
        /* MVP 영역에 그릴 화면의 종류 표시 */

        float gsRenderModeAlpha = 1.0f
                                  - (gsRenderModeCounter
                                     / MVP_RENDER_MODE_ANIMATION_DURATION);

        DrawTextEx(GetFontDefault(),
                   TextFormat("%d", gsRenderMode),
                   (Vector2) { .x = gsGuiArea.x + 8.0f, .y = 8.0f },
                   (GetFontDefault().baseSize << 1),
                   1.0f,
                   ColorAlpha(DARKGRAY, gsRenderModeAlpha));
    }
}

/* 게임 화면의 오른쪽 영역을 그리는 함수 */
static void DrawMvpArea(void) {
    {
        if (gsRenderModeCounter < MVP_RENDER_MODE_ANIMATION_DURATION)
            gsRenderModeCounter += GetFrameTime();
    }

    {
        for (int i = MODE_ALL + 1; i < MODE_COUNT_; i++) {
            if (gsUpdateSpaceFuncs[i] == NULL) continue;

            gsUpdateSpaceFuncs[i](gsRenderTextures[i]);

            // 텍스처 필터링 (이중 선형 필터링)
            SetTextureFilter(gsRenderTextures[i].texture,
                             TEXTURE_FILTER_BILINEAR);
        }
    }

    // 전부 다 그리기 vs. 하나만 그리기
    if (gsRenderMode == MODE_ALL) {
        for (int i = MODE_ALL + 1; i < MODE_COUNT_; i++) {
            float halfWidth = 0.5f * gsMvpArea.width;
            float halfHeight = 0.5f * gsMvpArea.height;

            /*
                첫 번째 행에는 "물체 공간"과 "세계 공간",
                두 번째 행에는 "카메라 (뷰) 공간"과 "클립 공간"
            */
            DrawTexturePro(gsRenderTextures[i].texture,
                           (Rectangle) { .width = gsMvpArea.width,
                                         .height = -gsMvpArea.height },
                           (Rectangle) { .x = gsMvpArea.x
                                              + ((!(i & 1)) * halfWidth),
                                         .y = gsMvpArea.y
                                              + ((i > MODE_WORLD) * halfHeight),
                                         .width = 0.5f * gsMvpArea.width,
                                         .height = 0.5f * gsMvpArea.height },
                           Vector2Zero(),
                           0.0f,
                           WHITE);
        }
    } else {
        DrawTexturePro(gsRenderTextures[gsRenderMode].texture,
                       (Rectangle) { .width = gsMvpArea.width,
                                     .height = -gsMvpArea.height },
                       gsMvpArea,
                       Vector2Zero(),
                       0.0f,
                       WHITE);
    }

    {
        /* GUI 영역과 MVP 영역 사이의 경계선 */

        DrawLineEx((Vector2) { .x = gsMvpArea.x, .y = 0.0f * SCREEN_HEIGHT },
                   (Vector2) { .x = gsMvpArea.x, .y = 1.0f * SCREEN_HEIGHT },
                   1.0f,
                   GRAY);
    }

    if (gsRenderMode == MODE_ALL) {
        /* MVP 영역 위에 그려지는 회색 십자선 */

        Vector2 gsMvpCenter = { .x = gsMvpArea.x + (0.5f * gsMvpArea.width),
                                .y = gsMvpArea.y + (0.5f * gsMvpArea.height) };

        DrawLineEx((Vector2) { .x = gsMvpCenter.x, .y = gsMvpArea.y },
                   (Vector2) { .x = gsMvpCenter.x, .y = 1.0f * SCREEN_HEIGHT },
                   1.0f,
                   GRAY);

        DrawLineEx((Vector2) { .x = gsMvpArea.x, .y = gsMvpCenter.y },
                   (Vector2) { .x = 1.0f * SCREEN_WIDTH, .y = gsMvpCenter.y },
                   1.0f,
                   GRAY);
    }
}

/* 마우스 및 키보드 입력을 처리하는 함수 */
static void HandleInputEvents(void) {
    {
        /* MVP 영역에 그릴 화면 종류 변경 */

        int keyCode = GetKeyPressed();

        if (keyCode >= '0' + MODE_ALL && keyCode < '0' + MODE_COUNT_)
            gsRenderMode = keyCode - '0', gsRenderModeCounter = 0.0f;
    }
}