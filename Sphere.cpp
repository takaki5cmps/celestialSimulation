// Sphereクラスの実装部分

// #include <iostream>
// #include <GL/gl.h>   // OpenGLの基本機能を使うためのヘッダー
// #include <GL/glu.h>  // OpenGLのユーティリティ関数（例: gluSphere）を使うためのヘッダー#include "Sphere.h"
// #include <cmath>    // std::sqrtなど

#include "Constants.h"
#include "Geometry.h"

#include "Sphere.h"


// コンストラクタ: 初期値を設定
Sphere::Sphere(
        std::wstring nameInput,
        float posX, float posY, float posZ,
        float velX, float velY, float velZ, 
        float m, float rad,
        float r, float g, float b
)
:   name(nameInput), // 名前
    x(posX*scaling::distance), y(posY*scaling::distance), z(posZ*scaling::distance), // 位置
    mass(m),    // 質量
    radius(rad*scaling::distance), // 半径
    vx(velX*scaling::velocity), vy(velY*scaling::velocity), vz(velZ*scaling::velocity), // 速度
    ax(0.0f), ay(0.0f), az(0.0f), // 加速度
    angle_theta(0.0f),  // 球の回転角度（z軸回りの角度）
    angle_phi(0.0f),    // 球の回転軸のz軸に対する角度（-90度から90度）
    trajectoryLength(TRAJECTORYLENGTH)
{
    // 球体の色
    color[0] = r*scaling::color; color[1] = g*scaling::color; color[2] = b*scaling::color;

    // 球体の初期条件を表示
    // std::cout << "initial position:" << x << ", " << y << ", " << z << std::endl;
    // std::cout << "initial velosity:" << vx << ", " << vy << ", " << vz << std::endl;
    // std::cout << "mass:" << mass << std::endl;
    // std::cout << "radius:" << radius << std::endl;
    // std::cout << "RGB:" << color[0] << ", " << color[1] << ", " << color[2] << std::endl;
}

// 自転角度を更新
void Sphere::updateRotation(float delta) {
    angle_theta += delta; // 回転角度を更新
    if (angle_theta > 360.0f) angle_theta -= 360.0f; // 360度を超えたらリセット
}

// 球を描画
void Sphere::draw() {
    glPushMatrix();                     // 現在の座標系を保存
    glTranslatef(x, y, z);              // 球の位置に移動
    glRotatef(angle_theta, 0.0f, 0.0f, 1.0f); // Y軸を中心にangle_theta度回転

    // 球の材質の色を設定（AmbientとDiffuseを設定）
    GLfloat matColor[] = {color[0], color[1], color[2], 1.0f}; // RGB + α（アルファ値）
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matColor);

    GLUquadric* quadric = gluNewQuadric();   // 球を生成するためのオブジェクト

    gluSphere(quadric, radius, 10, 10);        // 半径radius、分割数10x10の球を描画
    gluDeleteQuadric(quadric);              // 使用後に解放

    glPopMatrix();                          // 座標系を元に戻す
}

// 軌跡を描画する
void Sphere::drawTrajectory() {
    glPushMatrix(); // 座標系を保存

    glDisable(GL_LIGHTING);     //ライティングを一度無効にしないと色が反映されない。
    glColor3f(color[0], color[1], color[2]);  // 球体と同じ色
    glBegin(GL_LINE_STRIP);  // 連続した線として軌跡を描く
    for (auto& point : trajectory) {
        glVertex3f(std::get<0>(point), std::get<1>(point), std::get<2>(point));
    }
    glEnd();
    glEnable(GL_LIGHTING);

    glPopMatrix();  // 座標系を復元
}
