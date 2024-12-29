#include <iostream>
#include <chrono>
#include <format>


 maketimepiont(int year, int month, int day, int hour, int minute, int second)
{
    std::tm t;
    t.tm_year = year-1900;
    t.tm_mon = month-1;
    t.tm_mday = day;
    t.tm_hour = hour;
    t.tm_min = minute;
    t.tm_sec = second;
    t.tm_isdst = 0;
    std::time_t tt = std::mktime(&t);
    return std::chrono::system_clock::from_time_t(tt);
}

int main()
{
    // 日時の入力
    int year, month, day, hour, minute, second;
    std::cout << "year:" << std::endl;
    std::cin >> year;
    std::cout << "month:" << std::endl;
    std::cin >> month;
    std::cout << "day:" << std::endl;
    std::cin >> day;
    std::cout << "hour:" << std::endl;
    std::cin >> hour;
    std::cout << "minute:" << std::endl;
    std::cin >> minute;
    std::cout << "second:" << std::endl;
    std::cin >> second;
    // タイムポイントを作成
    std::chrono::system_clock::time_point tp = maketimepiont(year, month, day, hour, minute, second);
    std::time_t tp_time_t = std::chrono::system_clock::to_time_t(tp);
    std::tm *tp_tm = std::localtime(&tp_time_t);
    // タイムポイントを出力
    std::cout << tp_tm->tm_year+1900 << "/" << tp_tm->tm_mon+1 << "/" << tp_tm->tm_mday << " " << tp_tm->tm_hour << ":" << tp_tm->tm_min << ":" << tp_tm->tm_sec << std::endl;
    std::cout << std::format("{:%Y-%m-%d %H:%M:%S}", tp) << std::endl;

    return 0;
}