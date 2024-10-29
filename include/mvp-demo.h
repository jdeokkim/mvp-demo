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

#ifndef MVP_DEMO_H
#define MVP_DEMO_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================ */

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

/* Macros ================================================================== */

/* Macro Constants ========================================================= */

// clang-format off: [주의] 이 주석은 코드 포맷팅에 사용되므로 지우지 말 것!!!

/* 게임의 목표 FPS */
#define TARGET_FPS                          60

/* 게임 창의 크기 */
#define SCREEN_WIDTH                        1280
#define SCREEN_HEIGHT                       800

/* MVP 영역에 그릴 화면의 종류를 몇 초 동안 보여줄지 설정 */
#define MVP_RENDER_MODE_ANIMATION_DURATION  0.75f

// clang-format on: [주의] 이 주석은 코드 포맷팅에 사용되므로 지우지 말 것!!!

/* Typedefs ================================================================ */

/* MVP 영역에 그릴 화면의 종류 */
typedef enum MvpRenderMode_ {
    MODE_ALL = 0,  // "전부 다"
    MODE_LOCAL,    // "물체 공간"
    MODE_WORLD,    // "세계 공간"
    MODE_VIEW,     // "카메라 공간"
    MODE_CLIP,     // "클립 공간"
    MODE_COUNT_    // 총 몇 가지?
} MvpRenderMode;

/* "<물체 / 세계 / 카메라> 공간"을 초기화하는 함수 */
typedef void (*InitSpaceFunc)(void);

/* 프레임버퍼에 "<물체 / 세계 / 카메라> 공간"을 그리는 함수 */
typedef void (*UpdateSpaceFunc)(RenderTexture);

/* "<물체 / 세계 / 카메라> 공간"에 필요한 메모리 공간을 해제하는 함수 */
typedef void (*DeinitSpaceFunc)(void);

/* Public Function Prototypes ============================================== */

/* ======================================================= (from src/game.c) */

/* 게임 화면을 초기화하는 함수 */
void InitGameScreen(void);

/* 게임 화면을 그리고 게임 상태를 업데이트하는 함수 */
void UpdateGameScreen(void);

/* 게임 화면에 필요한 메모리 공간을 해제하는 함수 */
void DeinitGameScreen(void);

/* ========================================================================= */

/* MVP 영역에 그릴 화면의 종류를 반환하는 함수 */
MvpRenderMode GetRenderMode(void);

/* ===================================================== (from src/clip.c) */

/* "클립 공간"을 초기화하는 함수 */
void InitClipSpace(void);

/* 프레임버퍼에 "클립 공간"을 그리는 함수 */
void UpdateClipSpace(RenderTexture renderTexture);

/* "클립 공간"에 필요한 메모리 공간을 해제하는 함수 */
void DeinitClipSpace(void);

/* ===================================================== (from src/local.c) */

/* "물체 공간"을 초기화하는 함수 */
void InitLocalSpace(void);

/* 프레임버퍼에 "물체 공간"을 그리는 함수 */
void UpdateLocalSpace(RenderTexture renderTexture);

/* "물체 공간"에 필요한 메모리 공간을 해제하는 함수 */
void DeinitLocalSpace(void);

/* ===================================================== (from src/view.c) */

/* "카메라 (뷰) 공간"을 초기화하는 함수 */
void InitViewSpace(void);

/* 프레임버퍼에 "카메라 (뷰) 공간"을 그리는 함수 */
void UpdateViewSpace(RenderTexture renderTexture);

/* "카메라 (뷰) 공간"에 필요한 메모리 공간을 해제하는 함수 */
void DeinitViewSpace(void);

/* ===================================================== (from src/world.c) */

/* "세계 공간"을 초기화하는 함수 */
void InitWorldSpace(void);

/* 프레임버퍼에 "세계 공간"을 그리는 함수 */
void UpdateWorldSpace(RenderTexture renderTexture);

/* "세계 공간"에 필요한 메모리 공간을 해제하는 함수 */
void DeinitWorldSpace(void);

/* ===================================================== (from src/utils.c) */

/* X축, Y축과 Z축을 그리는 함수 */
void DrawAxes(void);

/* 공용 셰이더 프로그램을 반환하는 함수 */
Shader LoadCommonShader(void);

/* ========================================================================= */

#ifdef __cplusplus
}
#endif

#endif  // `MVP_DEMO_H`