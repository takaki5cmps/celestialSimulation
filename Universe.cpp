// #include <cmath>
// #include <vector>
#include <chrono>

#include "Universe.h"
#include "Sphere.h"
#include "Constants.h"
#include "Geometry.h"


// コンストラクタで積分手法を指定できるようにする
Universe::Universe(IntegrationMethod method, std::chrono::system_clock::time_point startTime)
:   integrationMethod(method),  // 数値積分の方法
    simulationTime_(-1*scaling::DT*INITIAL_WAITING_PERIOD),   // simulationTimeの初期値:0を上回らないと開始しないので、マイナスの値を入れることで開始までのカウントダウンをしている。
    startTime_(startTime)  // シミュレーション開始時刻
{    
    centerOfMass[0] = 0.0f;
    centerOfMass[1] = 0.0f;
    centerOfMass[2] = 0.0f;
}

void Universe::addSphere(const Sphere& sphere) {
    spheres.push_back(sphere);  // 新しいSphereを追加
}

void Universe::calculateForces() {
    //重心計算用の変数
    float totalMass = 0.0f;
    float weightedX = 0.0f, weightedY = 0.0f, weightedZ = 0.0f;

    // 各天体に対して、他のすべての天体からの引力を計算
    for (size_t i = 0; i < spheres.size(); ++i) {
        Sphere& sphere1 = spheres[i];
        sphere1.ax = sphere1.ay = sphere1.az = 0.0f;  // 初期化（前回の加速度をクリア）

        // 質量と位置に基づいて重心を計算
        totalMass += sphere1.mass;
        weightedX += sphere1.x * sphere1.mass;
        weightedY += sphere1.y * sphere1.mass;
        weightedZ += sphere1.z * sphere1.mass;

        for (size_t j = 0; j < spheres.size(); ++j) {
            if (i == j) continue;  // 同じ天体は無視

            Sphere& sphere2 = spheres[j];

            // 2つの天体間の距離を計算
            float dx = sphere2.x - sphere1.x;
            float dy = sphere2.y - sphere1.y;
            float dz = sphere2.z - sphere1.z;
            float distance = std::sqrt(dx*dx + dy*dy + dz*dz);

            // 引力を計算
            float force = (celestialConstants::G * scaling::G * sphere1.mass * sphere2.mass) / (distance * distance);
            
            // 力の成分を加速度に変換（F = ma, a = F / m）
            float ax = force * dx / (sphere1.mass * distance);
            float ay = force * dy / (sphere1.mass * distance);
            float az = force * dz / (sphere1.mass * distance);

            // 加速度を加算
            sphere1.ax += ax;
            sphere1.ay += ay;
            sphere1.az += az;
        
            //引力と遠心力を出力
            float v = sqrt(sphere1.vx * sphere1.vx + sphere1.vy * sphere1.vy + sphere1.vz * sphere1.vz);
            float a = sqrt(sphere1.ax * sphere1.ax + sphere1.ay * sphere1.ay + sphere1.az * sphere1.az);
            // std::cout << "gravity:" << sphere1.mass * a << "centrifualForce:" << sphere1.mass * v * v /distance << std::endl;
        }
        // 重心を更新（質量加重平均）
        if (totalMass > 0.0f) {
            centerOfMass[0] = weightedX / totalMass;
            centerOfMass[1] = weightedY / totalMass;
            centerOfMass[2] = weightedZ / totalMass;
        }

    }
}

// 位置と速度を更新
void Universe::updatePosition(float dt) {
    for (size_t i = 0; i < spheres.size(); ++i) {
        
        switch(integrationMethod){
            case IntegrationMethod::Euler:{
                // 速度を更新
                spheres[i].vx += spheres[i].ax * dt;
                spheres[i].vy += spheres[i].ay * dt;
                spheres[i].vz += spheres[i].az * dt;
                // 位置を更新
                spheres[i].x += spheres[i].vx * dt;
                spheres[i].y += spheres[i].vy * dt;
                spheres[i].z += spheres[i].vz * dt;
                // 加速度を更新
                calculateForces();
                break;
            }
            case IntegrationMethod::Heun :{
                // このHeun法はうまく実装できてない可能性高い
                // 速度を予測
                float vx_predict = spheres[i].ax * dt;
                float vy_predict = spheres[i].ay * dt;
                float vz_predict = spheres[i].az * dt;
                // 位置を予測
                spheres[i].x += vx_predict * dt;
                spheres[i].y += vy_predict * dt;
                spheres[i].z += vz_predict * dt;
                // 加速度を保存後に更新して位置を戻す
                float ax_current = spheres[i].ax;
                float ay_current = spheres[i].ay;
                float az_current = spheres[i].az;
                calculateForces();
                spheres[i].x -= vx_predict * dt;
                spheres[i].y -= vy_predict * dt;
                spheres[i].z -= vz_predict * dt;
                // 速度を保存した上で修正
                float vx_current =spheres[i].vx;
                float vy_current =spheres[i].vy;
                float vz_current =spheres[i].vz;
                spheres[i].vx += 0.5f * (ax_current + spheres[i].ax) * dt;
                spheres[i].vy += 0.5f * (ay_current + spheres[i].ay) * dt;
                spheres[i].vz += 0.5f * (az_current + spheres[i].az) * dt;
                // 修正された速度で位置を修正
                spheres[i].x += 0.5f * (vx_current+spheres[i].vx)* dt;
                spheres[i].y += 0.5f * (vy_current+spheres[i].vy)* dt;
                spheres[i].z += 0.5f * (vz_current+spheres[i].vz)* dt;
                break;
            }
            case IntegrationMethod::RK4 :{
                float k1[3] ={spheres[i].vx, spheres[i].vy, spheres[i].vz};
                float l1[3] ={spheres[i].ax, spheres[i].ay, spheres[i].az};

                float k2[3]={spheres[i].vx+0.5f*l1[0]*dt, spheres[i].vy+0.5f *l1[1]*dt, spheres[i].vz+0.5f *l1[2]*dt};
                spheres[i].x += k1[0]*(dt/2);
                spheres[i].y += k1[1]*(dt/2);
                spheres[i].z += k1[0]*(dt/2);
                calculateForces();
                float l2[3] = {spheres[i].ax,spheres[i].ay, spheres[i].az};
                spheres[i].x -= k1[0]*(dt/2);
                spheres[i].y -= k1[1]*(dt/2);
                spheres[i].z -= k1[0]*(dt/2);

                float k3[3] = {spheres[i].vx + l2[0]*(dt/2), spheres[i].vy + l2[1]*(dt/2),spheres[i].vz + l2[2]*(dt/2)};
                spheres[i].x += k2[0]*(dt/2);
                spheres[i].y += k2[1]*(dt/2);
                spheres[i].z += k2[0]*(dt/2);
                calculateForces();
                float l3[3] = {spheres[i].ax,spheres[i].ay, spheres[i].az};
                spheres[i].x -= k2[0]*(dt/2);
                spheres[i].y -= k2[1]*(dt/2);
                spheres[i].z -= k2[0]*(dt/2);                    
                
                float k4[3] = {spheres[i].vx + l3[0]*dt,spheres[i].vy + l3[1]*dt,spheres[i].vz + l3[2]*dt};
                spheres[i].x += k3[0]*dt;
                spheres[i].y += k3[1]*dt;
                spheres[i].z += k3[2]*dt;
                calculateForces();
                float l4[3] = {spheres[i].ax,spheres[i].ay, spheres[i].az};
                spheres[i].x -= k3[0]*dt;
                spheres[i].y -= k3[1]*dt;
                spheres[i].z -= k3[2]*dt;
                
                float k[3] = {(k1[0] + 2*k2[0] + 2*k3[0] + k4[0])/6, (k1[1] + 2*k2[1] + 2*k3[1] + k4[1])/6, (k1[2] + 2*k2[2] + 2*k3[2] + k4[2])/6};
                float l[3] = {(l1[0] + 2*l2[0] + 2*l3[0] + l4[0])/6, (l1[1] + 2*l2[1] + 2*l3[1] + l4[1])/6, (l1[2] + 2*l2[2] + 2*l3[2] + l4[2])/6};

                spheres[i].x += k[0]*dt;
                spheres[i].y += k[1]*dt;
                spheres[i].z += k[2]*dt;
                spheres[i].vx += l[0]*dt;
                spheres[i].vy += l[1]*dt;
                spheres[i].vz += l[2]*dt;

                break;
            }

        }

        // 軌跡を更新
        spheres[i].trajectory.push_back(std::make_tuple(spheres[i].x, spheres[i].y, spheres[i].z));  // 新しい位置を追加
        // 軌跡の長さが1000を超えたら古いものを削除
        if (spheres[i].trajectory.size() > spheres[i].trajectoryLength) {
            spheres[i].trajectory.erase(spheres[i].trajectory.begin());
        }
    }
}

void Universe::update(float dt) {
    simulationTime_ += dt; // 時間を更新
    if (simulationTime_ > 0){
        calculateForces();  // 力を計算
        updatePosition(dt);  // 位置と速度を更新

        for (Sphere& sphere : spheres) {
            sphere.updateRotation(1.0f); // 回転角度を1度増加
        }
    }
}

float Universe::getSimulationTime(){
    return simulationTime_;
}

std::chrono::system_clock::time_point Universe::getSimulationTime_tp(){
    return startTime_+std::chrono::duration_cast<std::chrono::system_clock::duration>(std::chrono::duration<float>(simulationTime_));
}