#ifndef CONSTANTS_H

#define CONSTANTS_H

#include <iostream> // size_t
#include <cmath> // M_PI

// グローバル定数
extern const float INITIAL_WAITING_PERIOD; //exe実行からアニメーション開始まで時間[DT]
extern const size_t TRAJECTORYLENGTH;    // 軌跡の長さ(残存時間)[DT]

// 物理定数
namespace celestialConstants {
    extern const float G; // 万有引力定数(単位: m^3 kg^-1 s^-2)
    extern const float solar_mass;       // 太陽の質量 (kg)
    extern const float solar_radius;      // 太陽の半径 (km)
    extern const float earth_mass;       // 地球の質量 (kg)
    extern const float earth_radius;      // 地球の半径 (km)
    extern const float distance_sun_earth; // 太陽と地球の平均距離 (km)
    extern const float earth_orbital_speed; // 地球の公転速度 (km/s)
    extern const float distance_earth_moon;    // 地球と月の距離(km)
    extern const float moon_orbital_speed;  // 月の公転速度(km/s)
    extern const float moon_mass; // 月の質量(kg)
    extern const float moon_radius;   // 月の半径(km)
}

// スケール係数
namespace scaling{
   // 時間関係
    extern const float time_simu2real; // 現実時間1sで経過するシミュレーション時間[s]
    extern const float DT; // 数値積分の時間ステップ。単位はシミュレーション内のsecond
    // 物理量関係
    extern const float distance; // 長さをkmからシミュレーション単位へ変換。10を1e+8kmくらいとする。(参考：地球から太陽までの距離は1.496e+8km)
    extern const float time_toDT;      // 時間をsecondからシミュレーション単位へ変換。1000[DT]で地球1周にしたい->1000[DT]=365日*24時間*60分*60second
    extern const float time;            // 時間をDTから描画単位へ変換。
    extern const float G;   // 万有引力定数を変換
    extern const float velocity; // 速度を変換
    extern const float acceleration;    // 加速度。これは必要ない可能性が高い。ユーザーが入力するものではないので。
    // RGB
    extern const float color;  // ユーザーは0~255のRGBを入力するが、openGLは0~1で扱うため
}





// 数値積分の手法列挙
enum class IntegrationMethod {
    Euler,  // Euler法
    Heun,   // Heun法
    RK4     // 4次のRunge-Kutta
};
#endif