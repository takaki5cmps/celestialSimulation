
#include "Constants.h"

// グローバル定数
const float DT = 10000; // 数値積分および描画の時間ステップ。
const float INITIAL_WAITING_PERIOD = 0.2*250; //exe実行からアニメーション開始まで時間[DT]
const size_t TRAJECTORYLENGTH = 500;    // 軌跡の長さ(残存時間)[DT]

// 物理定数
namespace celestialConstants {
    const float G = 6.67430e-11f; // 万有引力定数(単位: m^3 kg^-1 s^-2)
    const float solar_mass = 1.989e30;       // 太陽の質量 (kg)
    const float solar_radius = 6.957e5;      // 太陽の半径 (km)
    const float earth_mass = 5.972e24;       // 地球の質量 (kg)
    const float earth_radius = 6.371e3;      // 地球の半径 (km)
    const float distance_sun_earth = 1.496e8; // 太陽と地球の平均距離 (km)
    const float earth_orbital_speed = 2.978e1; // 地球の公転速度 (km/s)
    const float distance_earth_moon=3.844e5;    // 地球と月の距離(km)
    const float moon_orbital_speed = 1.022;  // 月の公転速度(km/s)
    const float moon_mass=7.342e22; // 月の質量(kg)
    const float moon_radius=1.7374e3;   // 月の半径(km)
}

// スケール係数
namespace scaling{
    // 物理量関係
    const float distance = 100/1.0e8; // 長さをkmからシミュレーション単位へ変換。10を1e+8kmくらいとする。(参考：地球から太陽までの距離は1.496e+8km)
    const float time_toDT = 100*1000.0f/(365.0f*24.0f*60.0f*60.0f);      // 時間をsecondからシミュレーション単位へ変換。1000[DT]で地球1周にしたい->1000[DT]=365日*24時間*60分*60second
    const float time = time_toDT * ::DT;            // 時間をDTから描画単位へ変換。
    const float G = 1e-9 * (distance * distance * distance) / (time * time);   // 万有引力定数を変換
    const float velocity = distance / time; // 速度を変換
    const float acceleration = distance / (time * time);    // 加速度。これは必要ない可能性が高い。ユーザーが入力するものではないので。
    // RGB
    const float color = 1/255.0f;  // ユーザーは0~255のRGBを入力するが、openGLは0~1で扱うため
}

// カメラや描画に関する定数
namespace cameraSetting{
    const float fovy = 60.0f;
    const float zNear = 1.0e-5f;
    const float zFar = 10000.0f;
    const float omega= 2.0*M_PI/(365*24*60*60);               //カメラの経度方向の移動の角速度
    const float omega_z= 2.0*M_PI/(365*24*60*60);            //カメラの緯度方向の移動の角速度
}
