#include <chrono>
#include <iostream>

class Timer
{

public:
    Timer(double timeScale);
    void update();
private:
    // 時間は3種類存在する
    double DT_;  // シミュレーション1ステップで進む時間。1ステップを10000としている。DT/10000をTimeUnitとする。
    double DT_realSecond_;   // WM_TIMERが発生する間隔。DTをミリ秒に変換したものに近い。
    double DT_simulatedSecond_ = DT_realSecond_ * timeScale_; // シミュレーション1ステップによってシミュレーション内の世界で進む時間[秒]

    // double realElapsedTime_; // 前回updateから現実の時間の経過[TimeUnit]
    // std::chrono::time_point<std::chrono::high_resolution_clock> lastUpdate_; // 最後に時間を更新した時刻(getRealDuration()で使用。単位は現実の時間)

    double timeScale_;  // シミュレーションの経過時間/現実の経過時間。時間が何倍の速さで進むか。

    double time_;    // TimeUnit
    double realTime_;   // 現実の時間[TimeUnit]
    double simulatedTime_; // シミュレーション対象の世界の時間[TimeUnit]

    // ゲッター
    double getDT_realSecond();
    double getDT_simulatedSecond();
};

Timer::Timer(double timeScale = 40000.0)
{
    timeScale_ = timeScale;
    DT_ = 10000;  // シミュレーション1ステップで進む時間。1ステップを10000としている。DT/10000をTimeUnitとする。
    DT_realSecond_ = 0.016;   // WM_TIMERが発生する間隔。DTをミリ秒に変換したものに近い。
    DT_simulatedSecond_ = DT_realSecond_ * timeScale_; // シミュレーション1ステップによってシミュレーション内の世界で進む時間[秒]
    time_=0.0;        // TimeUnit
    realTime_ = 0.0;   // 現実の時間[秒]
    simulatedTime_ = 0.0; // シミュレーション対象の世界の時間[秒]
}


void Timer::update()
{
    time_ += DT_; // TimeUnitを更新
    realTime_ += DT_realSecond_ / 1000; // 現実の時間を更新[秒]
    simulatedTime_ += DT_simulatedSecond_; // シミュレーション対象の世界の時間を更新[秒]
}

double Timer::getDT_realSecond()
{
    return DT_realSecond_;
}
double Timer::getDT_simulatedSecond()
{
    return DT_simulatedSecond_;
}