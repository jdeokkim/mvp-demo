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
void DrawAxes(void) {
    // X축
    DrawCylinderEx(Vector3Zero(),
                   (Vector3) { .x = 1.0f, .y = 0.0f, .z = 0.0f },
                   0.02f,
                   0.02f,
                   32,
                   RED);

    // Y축
    DrawCylinderEx(Vector3Zero(),
                   (Vector3) { .x = 0.0f, .y = 1.0f, .z = 0.0f },
                   0.02f,
                   0.02f,
                   32,
                   GREEN);

    // Z축
    DrawCylinderEx(Vector3Zero(),
                   (Vector3) { .x = 0.0f, .y = 0.0f, .z = 1.0f },
                   0.02f,
                   0.02f,
                   32,
                   BLUE);

    // 원점
    DrawSphere(Vector3Zero(), 0.05f, ColorAlpha(BLACK, 0.75f));
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