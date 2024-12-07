#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>

class Logger {
public:
    // コンストラクタ: ログファイル名を指定
    explicit Logger(const std::string& fileName);

    // 日本語対応のログ出力メソッド
    void log(const std::wstring& message);

    // デストラクタ
    ~Logger();

private:
    std::ofstream logFile;

    // ワイド文字列をUTF-8に変換
    std::string convertToUTF8(const std::wstring& wstr);
};

#endif // LOGGER_H
