/*
    Copyright (c) 2024 Jaedeok Kim <jdeokkim@protonmail.com>
    Copyright (c) 2024 Minhu Lee <fghkk12d@gmail.com>

    Copyright (c) 2024 Jaedeok Kim <jdeokkim@protonmail.com>

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

/* Public Functions ======================================================== */

int main(void) {
    // MSAA 4x 안티-에일리어싱 (anti-aliasing) 기능 활성화
    SetConfigFlags(FLAG_MSAA_4X_HINT);

    // 게임 창 생성 및 OpenGL 컨텍스트 (context) 초기화
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);

    // 종료 키 설정 (비활성화)
    SetExitKey(KEY_NULL);

    // 게임의 최대 FPS 설정
    SetTargetFPS(TARGET_FPS);

    {
        InitGameScreen();

        // 게임 창의 '닫기' 버튼 또는 'ESC' 키가 눌리기 전까지...
        while (!WindowShouldClose())
            UpdateGameScreen();

        DeinitGameScreen();
    }

    // 게임 창 제거 및 OpenGL 컨텍스트에 할당된 메모리 해제
    CloseWindow();

    return 0;
}