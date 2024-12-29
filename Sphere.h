#ifndef SPHERE_H
#define SPHERE_H

#include <vector>   // std::vector, std::tuple
#include <GL/gl.h>   // OpenGLの基本機能を使うためのヘッダー
#include <GL/glu.h>  // OpenGLのユーティリティ関数（例: gluSphere）を使うためのヘッダー

#include "Constants.h"

class Sphere {
public:
    std::string name;      // 名前
    float x, y, z;         // 球の位置
    float vx, vy, vz;      // 球の速度（x, y, z成分）
    float ax, ay, az;      // 球の加速度（x, y, z成分）
    float angle_theta;     // 球の回転角度（z軸回りの角度）
    float angle_phi;       // 球の回転軸のz軸に対する角度（-90度から90度）
    float mass;            // 質量
    float radius;          // 半径
    float color[3];        // 球の色（RGB） 
    // 軌跡用のベクター
    std::vector<std::tuple<float, float, float>> trajectory; // 位置の軌跡（最大1000点）
    size_t trajectoryLength;  // 各天体の軌跡の長さ

    
    Sphere(     // コンストラクタ: 初期値を設定
        std::string nameInput,
        float posX, float posY, float posZ,
        float velX, float velY, float velZ, 
        float m, float rad,
        float r, float g, float b,
        bool lightEmission
    );
    void updateRotation(float delta);   // 回転角度を更新
    void draw(); // 球を描画
    void drawTrajectory();  //軌跡を描画
private:
    bool lightEmission_;    // 球が光を放つかどうか

};
#endif