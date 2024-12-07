#include <windows.h> // Windows APIを使用するためのヘッダー
#include <GL/gl.h>   // OpenGLの基本機能を使うためのヘッダー
#include <GL/glu.h>  // OpenGLのユーティリティ関数（例: gluSphere）を使うためのヘッダー
#include <vector>    // std::vector
#include <cmath>
#include <iostream>

// 自作ヘッダー
#include "Constants.h"  // 物理定数、スケール係数、カメラや描画に関する定数、数値積分の手法列挙
#include "Geometry.h"   // Geometry::distanceBetweenPoints, Geometry::angleBetweenSegments   二点間の距離を求める関数と、二つのベクトルのなす角を求める関数
#include "Sphere.h" // 球体を表すクラスSphereの宣言
#include "Universe.h" // SphereをまとめたクラスSpheresをメンバとして持つ。相互作用を計算し、各Sphereの位置や速度を決める。
#include "Camera.h" // 名前の通り。カメラの動きを決める。

// グローバル変数
// float globalTime = 0.0f;

Universe universe(IntegrationMethod::RK4);

// Camera camera(&universe.spheres[1]);
Camera camera(universe, {});

//光源の設定
void setLighting(GLenum lightSource) {
    // 環境光の設定（すべての物体に同じ環境光を与える）
    GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f }; // 低い光強度
    glLightfv(lightSource, GL_AMBIENT, ambientLight);

    // 拡散光の設定（物体に当たる直接光）
    GLfloat diffuseLight[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // 白色の強い光
    glLightfv(lightSource, GL_DIFFUSE, diffuseLight);

    // 鏡面反射光の設定（反射光による光沢）
    GLfloat specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // 鏡のような反射光
    glLightfv(lightSource, GL_SPECULAR, specularLight);

    // 光源の位置設定
    GLfloat lightPos[] = {0.0f, 0.0f, 10.0f, 1.0f}; // 光源位置 (点光源)
    glLightfv(lightSource, GL_POSITION, lightPos);
}

void drawGrid(float size, float step, float z) {
    glPushMatrix(); // 座標系の保存

    // 格子線を描画
    glDisable(GL_LIGHTING);     //ライティングを一度無効にしないと色が反映されない。
    glColor3f(0.3f, 0.3f, 0.3f); // 白色の格子線
    glBegin(GL_LINES);  // 線を描く

    // 横線
    for (float i = -size; i <= size; i += step) {
        glVertex3f(i, -size, z);
        glVertex3f(i, size, z);
    }

    // 縦線
    for (float i = -size; i <= size; i += step) {
        glVertex3f(-size, i, z);
        glVertex3f(size, i, z);
    }

    glEnd();
    glEnable(GL_LIGHTING);

    glPopMatrix(); // 座標系の復元
}
void drawRadialLines(int numLines, float length, float z) {
    glPushMatrix(); // 座標系の保存

    // 放射状の線を描画
    glDisable(GL_LIGHTING);     // ライティングを一度無効にしないと色が反映されない。
    glColor3f(0.3f, 0.3f, 0.3f); // 灰色の線
    glBegin(GL_LINES);  // 線を描く

    // 原点から放射状に線を引く
    for (int i = 0; i < numLines; ++i) {
        float angle = 2.0f * M_PI * i / numLines;  // 放射方向の角度を計算
        float x = length * cos(angle);
        float y = length * sin(angle);
        glVertex3f(0.0f, 0.0f, z);  // 原点
        glVertex3f(x, y, z);         // 放射先の位置
    }

    glEnd();
    glEnable(GL_LIGHTING);

    glPopMatrix(); // 座標系の復元
}
void drawPlane(float size, float step, float y) {
    glPushMatrix(); // 座標系を保存

    // 平面を描画
    glDisable(GL_LIGHTING);     // ライティングを一度無効にしないと色が反映されない。
    glColor3f(0.7f, 0.7f, 0.7f); // 灰色の平面
    glBegin(GL_QUADS);  // 四角形の面を描く

    // 平面の分割
    for (float i = -size; i < size; i += step) {
        for (float j = -size; j < size; j += step) {
            glVertex3f(i, y, j);
            glVertex3f(i + step, y, j);
            glVertex3f(i + step, y, j + step);
            glVertex3f(i, y, j + step);
        }
    }

    glEnd();
    glEnable(GL_LIGHTING);

    glPopMatrix(); // 座標系を復元
}



void drawTextW(HDC hdc, wchar_t* text, int x, int y) {
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 255, 255));  // 白色
    TextOutW(hdc, x, y, text, wcslen(text));  // 指定した位置にワイド文字のテキストを描画
}




// ウィンドウプロシージャ: ウィンドウが受け取るメッセージ（描画要求など）を処理する関数
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    float aspect = 0.0;
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0); // ウィンドウが閉じられたときにプログラムを終了する
            return 0;

        case WM_SIZE: {     //ウィンドウサイズが変更されたとき
            // ウィンドウの幅と高さを取得
            int width = LOWORD(lParam);  // ウィンドウの幅（下位16ビット）
            int height = HIWORD(lParam); // ウィンドウの高さ（上位16ビット）

            // ビューポートのサイズを設定
            glViewport(0, 0, width, height);  // 描画領域をウィンドウのサイズに合わせる

            // 投影行列（カメラの視野の設定）
            glMatrixMode(GL_PROJECTION); // 投影行列モード
            glLoadIdentity();            // 投影行列を単位行列にリセット
            aspect = (float)width / (float)height; // アスペクト比を計算
            gluPerspective(cameraSetting::fovy, aspect, cameraSetting::zNear, cameraSetting::zFar);  // 視野角とアスペクト比を設定

            glMatrixMode(GL_MODELVIEW); // モデルビュー行列に戻す
            return 0;
        }
        case WM_TIMER:
                universe.update(DT);
                InvalidateRect(hwnd, NULL, FALSE); // 間接的に再描画をリクエスト : 間接的にウィンドウに「WM_PAINT」が送られるらしい。
            return 0;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // 描画内容をクリア（画面を黒に塗りつぶし、深度バッファをリセット）
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // モデルビュー行列（カメラ位置やモデルの変換を設定）
            glMatrixMode(GL_MODELVIEW); // 操作対象行列をモデルビュー行列に設定
            glLoadIdentity();           // モデルビュー行列を単位行列にリセット
            
            // カメラの位置と向きの設定
            camera.update();
            // 上記で用意した値を渡してカメラの位置・向き(前・上)を設定
            gluLookAt(
                camera.getPosition(0), camera.getPosition(1), camera.getPosition(2),    // カメラの位置：全天体の重心の周りをまわっていく感じ
                camera.getTarget(0),camera.getTarget(1),camera.getTarget(2),    // カメラが注視する点：全天体の重心に設定
                camera.getUp(0),camera.getUp(1), camera.getUp(2)  // カメラの上方向
            );

            // 光源の設定
            glEnable(GL_LIGHTING);      // ライティングを有効化
            setLighting(GL_LIGHT0);     // 光源0の設定
            glEnable(GL_LIGHT0);        // 光源0を有効化

            //gridを描画
            drawGrid(30.0f, 3.0f, -10.0f);
            // drawGrid(10000.0f, 100, -1000);
            // drawRadialLines(500,10000,-10);

            // 全ての球を描画
            for (Sphere& sphere : universe.spheres) {
                sphere.draw();
                sphere.drawTrajectory();
            }
            SwapBuffers(hdc); // 描画内容を画面に反映
            
            // 位置や速度の情報をテキストとして描画
            int linePosition=0;
            wchar_t text[256];
            swprintf(text, sizeof(text)/sizeof(wchar_t), 
                L"Scale of distance (/km) : %.2E,  Scale of time (/s) : %.2E",
                scaling::distance,scaling::time
            );
            drawTextW(hdc, text, 10, 10 + static_cast<int>(linePosition));  // 位置や速度を画面に表示
            linePosition+=20;
            for (size_t i = 0; i < universe.spheres.size(); ++i) {
                swprintf(text, sizeof(text)/sizeof(wchar_t), 
                    L"%ls (Sphere%zu) : ",
                    universe.spheres[i].name.c_str(),
                    i + 1
                );
                drawTextW(hdc, text, 10, 10 + static_cast<int>(linePosition));  // 位置や速度を画面に表示
                linePosition+=20;

                swprintf(text, sizeof(text)/sizeof(wchar_t), 
                    L"Position(%.2E, %.2E, %.2E)[km], Velosity(%.2E, %.2E, %.2E)[km/s], Mass:%.2E[kg], Radius:%.2E[km], RGB(%.0f, %.0f, %.0f)", 
                    universe.spheres[i].x/scaling::distance, universe.spheres[i].y/scaling::distance, universe.spheres[i].z/scaling::distance, 
                    universe.spheres[i].vx/scaling::velocity, universe.spheres[i].vy/scaling::velocity, universe.spheres[i].vz/scaling::velocity, 
                    universe.spheres[i].mass,
                    universe.spheres[i].radius/scaling::distance,
                    universe.spheres[i].color[0]/scaling::color, universe.spheres[i].color[1]/scaling::color,universe.spheres[i].color[2]/scaling::color
                );
                drawTextW(hdc, text, 10, 10 + static_cast<int>(linePosition));  // 位置や速度を画面に表示
                linePosition+=20;
            }

            EndPaint(hwnd, &ps);
            return 0;
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}



// WinMain関数: プログラムのエントリーポイント
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    char CLASS_NAME[] = "OpenGLWindow";

    // ウィンドウクラスを登録
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc; // ウィンドウプロシージャを指定
    wc.hInstance = hInstance;   // アプリケーションのインスタンス
    wc.lpszClassName = CLASS_NAME; // クラス名
    RegisterClass(&wc);

    // 画面サイズを取得
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);  // 画面の幅
    int screenHeight = GetSystemMetrics(SM_CYSCREEN); // 画面の高さ

    // ウィンドウを作成
    HWND hwnd = CreateWindowEx(0, CLASS_NAME, "3D Sphere Simulation",
                               WS_OVERLAPPEDWINDOW, 0, 0, 1100, 1030,        // ウィンドウ位置座標のデフォルト値:CW_USEDEFAULT
                               NULL, NULL, hInstance, NULL);
    

    // 描画の準備
    HDC hdc = GetDC(hwnd);
    PIXELFORMATDESCRIPTOR pfd = {};
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;

    int format = ChoosePixelFormat(hdc, &pfd); // ピクセルフォーマットを選択
    SetPixelFormat(hdc, format, &pfd);         // ピクセルフォーマットを設定

    HGLRC glrc = wglCreateContext(hdc);        // OpenGLのレンダリングコンテキストを作成
    wglMakeCurrent(hdc, glrc);                 // 作成したレンダリングコンテキストを使用

    // ShowWindow(hwnd, nCmdShow);                // ウィンドウを表示
    ShowWindow(hwnd, SW_SHOW);                // ウィンドウを表示

        // OpenGLの初期化
    glEnable(GL_DEPTH_TEST);                   // 深度テストを有効化
    glClearColor(0.01f, 0.01f, 0.01f, 1.0f);      // 背景色を黒に設定
    // glClearColor(1.0f, 1.0f, 1.0f, 1.0f);           // 背景色を白に設定



// Sphereクラスのインスタンス化、天体の初期条件入力-------------------------------------------------------------------------
    float radiusScaler= 1.0;  // 実際の比にすると星が小さすぎて見えないので、便宜的に半径のみ実際より大きくしたい場合がある。
    universe.addSphere(Sphere(
        L"Sun", //名前(ワイド文字)
        0.0f, 0.0f, 0.0f,   //位置(km)
        0.0f, 0.0f, 0.0f,   //速度(km/s)
        celestialConstants::solar_mass, // 質量(kg)
        celestialConstants::solar_radius*radiusScaler,               //半径(km)
        255.0f, 100.0f, 0.0f    //rgb(0-255)
    ));  // 赤い球
    universe.addSphere(Sphere(
        L"Earth",   //名前(ワイド文字)
        celestialConstants::distance_sun_earth, 0.0f, 0.0f,   //位置(km)
        0.0f, celestialConstants::earth_orbital_speed, 0.0f,  //速度(km/s)
        celestialConstants::earth_mass,               //質量(kg)
        celestialConstants::earth_radius*radiusScaler,               //半径(km)
        69.0f, 130.0f, 181.0f    //rgb(0-255)
    ));
    universe.addSphere(Sphere(
        L"Moon",   //名前(ワイド文字)
        celestialConstants::distance_sun_earth+celestialConstants::distance_earth_moon, 0.0f, 0.0f,   //位置(km)
        0.0f, celestialConstants::earth_orbital_speed+celestialConstants::moon_orbital_speed, 0.0f,  //速度(km/s)
        celestialConstants::moon_mass,               //質量(kg)
        celestialConstants::moon_radius*radiusScaler,               //半径(km)
        190.0f, 190.0f, 190.0f    //rgb(0-255)
    ));
    // camera.addSphere(&universe.spheres[0]);
    camera.addSphere(&universe.spheres[1]);
    camera.addSphere(&universe.spheres[2]);
// Sphereクラスのインスタンス化、天体の初期条件入力-------------------------------------------------------------------------

    // タイマーを設定（16msごとにWM_TIMERメッセージを送信）
    SetTimer(hwnd, 1, 16, NULL);
          
    // メッセージループ
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // 後処理
    wglMakeCurrent(NULL, NULL); // レンダリングコンテキストを解除
    wglDeleteContext(glrc);     // レンダリングコンテキストを削除
    ReleaseDC(hwnd, hdc);       // デバイスコンテキストを解放

    return 0;
}


// 改良したいこと
// floatをdoubleにする
// クラスのメンバを隠蔽。ゲッタやセッタを設定
// フォーカスする天体を設定できるようにしたい。
// 地球に月を作る。
// 公転面の傾きの情報を設定する。
// Sphereクラスに主星を設定できるメンバを作る。そして、衛星の位置(速度)は主星に対する相対位置(相対速度)でも入力できるようにする。
// 星の自転に関する情報も加えたい。自転軸の傾き、自転周期
// 恒星か惑星かの情報を加えて、恒星は自ら光るようにする
// ルンゲクッタより精度のよい方法
// 時間のすぎる速さについての現実との対応
// 球面に星の模様をつける
// (済)クラスの宣言部分と実装を分ける。