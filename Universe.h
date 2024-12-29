#ifndef UNIVERSE_H
#define UNIVERSE_H

#include <chrono>
// #include "Constants.h"
#include "Sphere.h"



// Universeクラス：すべてのSphereオブジェクトを管理し、相互作用を計算、Sphereオブジェクトの状態も更新
class Universe {
public:
    // プロパティ
    std::vector<Sphere> spheres;  // Sphereオブジェクトのリスト
    float centerOfMass[3];  // 重心座標（x, y, z）
    IntegrationMethod integrationMethod;    // 数値積分の方法(Constants.hで定義されたIntegrationMethodという列挙体を入れる。)
    // コンストラクタ
    Universe(IntegrationMethod method, std::chrono::system_clock::time_point startTime);
    // その他メソッド
    void addSphere(const Sphere& sphere);
    void calculateForces();
    void updatePosition(float dt);
    void update(float dt);
    float getSimulationTime();
    std::chrono::system_clock::time_point getSimulationTime_tp();

private:
    float simulationTime_; // シミュレーションタイム
    std::chrono::system_clock::time_point startTime_;
};

#endif