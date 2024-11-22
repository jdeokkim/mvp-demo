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

#ifndef MVP_DEMO_H
#define MVP_DEMO_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================ */

#include <assert.h>
#include <stddef.h>

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

/* Macro Constants ========================================================= */

// clang-format off: [주의] 이 주석은 코드 포맷팅에 사용되므로 지우지 말 것!!!

/* 게임의 목표 FPS */
#define TARGET_FPS                          60

/* 게임 창의 크기 */
#define SCREEN_WIDTH                        1280
#define SCREEN_HEIGHT                       800

/* 게임 창의 이름 */
#define WINDOW_TITLE                        "jdeokkim/mvp-demo: " __FILE__

/* 가상 카메라의 "FOV" 최솟값 */
#define CAMERA_FOV_MIN_VALUE                1.0f

/* 가상 카메라의 "FOV" 최댓값 */
#define CAMERA_FOV_MAX_VALUE                179.0f

/* 게임 세계에 추가할 물체의 개수 */
#define GAME_OBJECT_COUNT                   3

/* 레이블에 들어갈 문자열의 최대 길이 */
#define LABEL_TEXT_LENGTH                   32

/* 행렬의 각 요소를 문자열로 나타냈을 때, 그 문자열의 최대 길이 */
#define MATRIX_VALUE_TEXT_LENGTH            16

/* 플레이어 모델의 정점 개수 */
#define PLAYER_MODEL_VERTEX_COUNT            8

/* MVP 영역에 그릴 화면의 종류를 몇 초 동안 보여줄지 설정 */
#define RENDER_MODE_ANIMATION_DURATION      2.75f

// clang-format on: [주의] 이 주석은 코드 포맷팅에 사용되므로 지우지 말 것!!!

/* Typedefs ================================================================ */

/* 게임 세계에 존재하는 물체의 종류 */
typedef enum GameObjectType_ {
    OBJ_TYPE_CAMERA,  // 가상 카메라
    OBJ_TYPE_PLAYER,  // 플레이어
    OBJ_TYPE_ENEMY,   // 적...?
    OBJ_TYPE_COUNT_   // (총 몇 가지?)
} GameObjectType;

/* 정점의 좌표 및 색상 정보 */
typedef struct VertexData_ {
    Vector3 vertices[PLAYER_MODEL_VERTEX_COUNT];
    Color colors[PLAYER_MODEL_VERTEX_COUNT];
} VertexData;

/* 게임 세계에 존재하는 물체 */
typedef struct GameObject_ {
    VertexData vertexData;  // 정점 색상
    GameObjectType type;    // 물체 종류
    Model model;            // 물체 모델
} GameObject;

/* MVP 영역에 그릴 화면의 종류 */
typedef enum MvpRenderMode_ {
    MVP_RENDER_ALL,    // "전부 다"
    MVP_RENDER_LOCAL,  // "물체 공간"
    MVP_RENDER_WORLD,  // "세계 공간"
    MVP_RENDER_VIEW,   // "카메라 공간"
    MVP_RENDER_CLIP,   // "클립 공간"
    MVP_RENDER_COUNT_  // (총 몇 가지?)
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

/* 공용 셰이더 프로그램을 반환하는 함수 */
Shader GetCommonShader(void);

/* 게임 세계의 `index`번째 물체를 반환하는 함수 */
GameObject *GetGameObject(int index);

/* GUI 영역에 사용되는 글꼴을 반환하는 함수 */
Font GetGuiDefaultFont(void);

/* MVP 영역에 그려지고 있는 화면의 종류를 반환하는 함수 */
MvpRenderMode GetMvpRenderMode(void);

/* View Frustum의 "Aspect" 값을 반환하는 함수 */
float GetViewFrustumAspect(void);

/* View Frustum의 "Near Distance" 값을 반환하는 함수 */
float GetViewFrustumNearDistance(void);

/* View Frustum의 "Far Distance" 값을 반환하는 함수 */
float GetViewFrustumFarDistance(void);

/* "모델 행렬"을 업데이트하는 함수 */
void UpdateModelMatrix(bool fromGUI);

/* "뷰 행렬"을 업데이트하는 함수 */
void UpdateViewMatrix(bool fromGUI);

/* "투영 행렬"을 업데이트하는 함수 */
void UpdateProjMatrix(bool fromGUI);

/* ======================================================= (from src/clip.c) */

/* "클립 공간"을 초기화하는 함수 */
void InitClipSpace(void);

/* 프레임버퍼에 "클립 공간"을 그리는 함수 */
void UpdateClipSpace(RenderTexture renderTexture);

/* "클립 공간"에 필요한 메모리 공간을 해제하는 함수 */
void DeinitClipSpace(void);

/* ========================================================================= */

/* 가상 카메라로 만들어지는 "투영 행렬"을 반환하는 함수 */
Matrix GetVirtualCameraProjMat(bool readOnly);

/* ===================================================== (from src/local.c) */

/* "물체 공간"을 초기화하는 함수 */
void InitLocalSpace(void);

/* 프레임버퍼에 "물체 공간"을 그리는 함수 */
void UpdateLocalSpace(RenderTexture renderTexture);

/* "물체 공간"에 필요한 메모리 공간을 해제하는 함수 */
void DeinitLocalSpace(void);

/* ===================================================== (from src/utils.c) */

/* 화살표를 그리는 함수 */
void DrawArrow(Vector3 startPos, Vector3 endPos, Color color);

/* X축, Y축과 Z축을 그리는 함수 */
void DrawAxes(void);

/* 세 축 `axis1`, `axis2`와 `axis3`를 `position`에 그리는 함수 */
void DrawAxesEx(Vector3 position,
                Vector3 axis1,
                Vector3 axis2,
                Vector3 axis3,
                Color color1,
                Color color2,
                Color color3);

/* 게임 세계의 물체를 그리는 함수 */
void DrawGameObject(GameObject *gameObject, MvpRenderMode renderMode);

/* 관찰자 시점 카메라의 입력 잠금 여부를 그리는 함수 */
void DrawHelpText(RenderTexture renderTexture,
                  bool isCameraLocked);

/* 공용 셰이더 프로그램으로 XZ 평면에 격자 무늬를 그리는 함수 */
void DrawInfiniteGrid(const Camera *camera);

/* 가상 카메라의 View Frustum을 그리는 함수 */
void DrawViewFrustum(MvpRenderMode renderMode, Color color);

/* 공용 셰이더 프로그램을 반환하는 함수 */
Shader LoadCommonShader(void);

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

/* ========================================================================= */

/* "세계 공간"의 가상 카메라를 반환하는 함수 */
Camera *GetVirtualCamera(void);

/* 가상 카메라의 "모델 행렬"을 반환하는 함수 */
Matrix GetVirtualCameraModelMat(bool readOnly);

/* 가상 카메라에 대한 "뷰 행렬"을 반환하는 함수 */
Matrix GetVirtualCameraViewMat(bool readOnly);

/* 가상 카메라에 대한 "뷰 행렬"의 U축을 반환하는 함수 */
Vector3 GetVirtualCameraUAxis(void);

/* 가상 카메라에 대한 "뷰 행렬"의 V축을 반환하는 함수 */
Vector3 GetVirtualCameraVAxis(void);

/* 가상 카메라에 대한 "뷰 행렬"의 N축을 반환하는 함수 */
Vector3 GetVirtualCameraNAxis(void);

/* ========================================================================= */

#ifdef __cplusplus
}
#endif

#endif  // `MVP_DEMO_H`