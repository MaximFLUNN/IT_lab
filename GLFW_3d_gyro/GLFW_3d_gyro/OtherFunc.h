#include <windows.h>
#include "COM_controller.h"
#include <string>

using namespace std;
unsigned int recalc_tmp_time(unsigned int start, unsigned int end, unsigned int res);
std::string wait_answer_from_esp32(COM_controller _cc);
void test_send_message_to_com_port(COM_controller cc, std::string answer, std::string message);

HANDLE hSerial;
void ReadCOM() {
    DWORD iSize;
    char sReceivedChar;
    while (true) {
        ReadFile(hSerial, &sReceivedChar, 1, &iSize, 0);  // получаем 1 байт
        if (iSize > 0)   // если что-то принято, выводим
            cout << sReceivedChar;
    }
}

int str_to_int(std::string _str) {
    int tmp_value = 0;
    bool minus = false;
    int size = _str.size();
    for (unsigned int i = 0; i < size; i++) {
        char s = _str[i];
        s == '-' ? minus = true : tmp_value = (tmp_value * 10) + (static_cast<unsigned int>(s) - 48);
    }
    minus ? tmp_value *= -1 : NULL;
    return tmp_value;
}

void test_send_message_to_com_port(COM_controller cc, std::string answer, std::string message) {
    cc.write_com_port(message);
    auto begin = std::chrono::steady_clock::now();
    answer = wait_answer_from_esp32(cc);
    auto end = std::chrono::steady_clock::now();
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
    std::cout << answer << " | " << elapsed_ms.count() << " ms\n";
}

void Blinking(HANDLE hStdOut, bool flag) {
    CONSOLE_CURSOR_INFO structCursorInfo;
    GetConsoleCursorInfo(hStdOut, &structCursorInfo);
    flag == false ? structCursorInfo.bVisible = FALSE : structCursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(hStdOut, &structCursorInfo);
}

std::string wait_answer_from_esp32(COM_controller _cc) {
    while (true) {
        std::string ans = _cc.read_com_port();
        if (!ans.empty()) {
            return ans;
        }
    }
}