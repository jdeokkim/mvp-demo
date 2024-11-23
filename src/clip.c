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

/* Private Variables ======================================================= */

/* 가상 카메라로 만들어지는 "투영 행렬" */
static Matrix projMat;

/* "투영 행렬"의 업데이트 필요 여부 */
static bool shouldUpdateProjMat = true;

/* Public Functions ======================================================== */

/* "클립 공간"을 초기화하는 함수 */
void InitClipSpace(void) {
    projMat = MatrixIdentity();
}

/* 프레임버퍼에 "클립 공간"을 그리는 함수 */
void UpdateClipSpace(RenderTexture renderTexture) {
    /* TODO: ... */

    // 렌더 텍스처 (프레임버퍼) 초기화
    BeginTextureMode(renderTexture);

    {
        ClearBackground(WHITE);

        {
            Camera *virtualCamera = GetVirtualCamera();

            BeginMode3D(*virtualCamera);

            projMat = rlGetMatrixProjection();

            if (shouldUpdateProjMat) {
                UpdateProjMatrix(false);

                shouldUpdateProjMat = false;
            }

            DrawAxes();

            for (int i = 0; i < GAME_OBJECT_COUNT; i++) {
                if (i == OBJ_TYPE_CAMERA) continue;

                DrawGameObject(GetGameObject(i),
                               renderTexture,
                               MVP_RENDER_CLIP);
            }

            DrawInfiniteGrid(virtualCamera);

            EndMode3D();
        }

        DrawVertexPositionText(renderTexture, MVP_RENDER_CLIP);

        // NOTE: 알파 값이 높은 (불투명한) 물체일수록 먼저 그려야 함
        DrawRectangleRec((Rectangle) { .width = renderTexture.texture.width,
                                       .height = renderTexture.texture.height },
                         ColorAlpha(DARKBLUE, 0.07f));

        DrawFPS(8, 8);
    }

    // 기본 프레임버퍼 상태로 되돌아가기
    EndTextureMode();
}

/* "클립 공간"에 필요한 메모리 공간을 해제하는 함수 */
void DeinitClipSpace(void) {
    // TODO: ...
}

/* ========================================================================= */

/* 가상 카메라로 만들어지는 "투영 행렬"을 반환하는 함수 */
Matrix GetVirtualCameraProjMat(bool readOnly) {
    // GUI에서 FOV 값을 업데이트할 때마다 "투영 행렬"도 같이 업데이트
    if (!readOnly) shouldUpdateProjMat = true;

    return projMat;
}