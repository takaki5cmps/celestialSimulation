#include "Logger.h"
#include <iostream>
#include <locale>
#include <codecvt>

// コンストラクタ: ログファイルを開く
Logger::Logger(const std::string& fileName) {
    logFile.open(fileName, std::ios::app | std::ios::out);
    if (!logFile.is_open()) {
        std::cerr << "Error: Unable to open log file." << std::endl;
    }
}

// 日本語対応のログ出力メソッド
void Logger::log(const std::wstring& message) {
    if (logFile.is_open()) {
        std::string utf8Message = convertToUTF8(message);
        logFile << utf8Message << std::endl;
    } else {
        std::cerr << "Error: Unable to write to log file." << std::endl;
    }
}

// デストラクタ: ログファイルを閉じる
Logger::~Logger() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

// ワイド文字列をUTF-8に変換
std::string Logger::convertToUTF8(const std::wstring& wstr) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    return converter.to_bytes(wstr);
}
