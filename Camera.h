#ifndef CAMERA_H
#define CAMERA_H

#include "Universe.h"
#include "Sphere.h"

class Camera{
public:

    Camera(Universe& universe, std::vector<Sphere*> targetSpheres);
    void changeSpheres(std::vector<Sphere*> targetSpheres);
    void addSphere(Sphere* sphere);
    float getPosition(int i) const;
    float getTarget(int i) const;
    float getUp(int i) const;
    void update();
private:
    // float fovy_, aspect_, zNear_, zFar;     // 一応作っておいた。
    float position_[3];  // カメラの位置（x, y, z）
    float target_[3];    // 注視点（見る方向）
    float up_[3];        // 上方向
    // float rot_[3];  // カメラの回転軸
    float omegaLatitude_;   // カメラの回転角速度(緯度方向)
    float omegaLongitude_;  // カメラの回転角速度(経度方向)
    std::vector<Sphere*> targetSpheres_;   // 注目の対象とする天体のアドレスを格納する。
    Universe& universe_; // カメラが対象とする宇宙
    
};

#endif