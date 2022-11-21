#pragma once
#include <windows.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <msclr\marshal.h>
#include <msclr\marshal_cppstd.h>
using namespace Microsoft::Win32;
#pragma warning(disable : 4996)

extern LPCTSTR _globalPortName;
extern HANDLE _globalHSerial;
extern std::string _globalWords;
extern std::string _globalAnswer;

inline DWORD WINAPI _AsyncClosehandle(LPVOID lpParam) {
    if (_globalHSerial == nullptr) {
        return 0;
    }
    CloseHandle(_globalHSerial);
    return 0;
}

inline DWORD WINAPI _AsyncCreateFile(LPVOID lpParam) {
    _globalHSerial = ::CreateFile(_globalPortName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    return 0;
}

inline DWORD WINAPI _AsyncWriteFile(LPVOID lpParam) {
    const int buff_size = 48;
    char data[buff_size] = { 0 };  // строка для передачи
    int size = _globalWords.length();
    if (size > buff_size) {
        throw std::length_error("size is big");
    }
    for (int i = 0; i < size; i++) {
        data[i] = _globalWords[i];
    }
    DWORD dwSize = sizeof(data);   // размер этой строки
    DWORD dwBytesWritten;    // тут будет количество собственно переданных байт
    BOOL iRet = WriteFile(_globalHSerial, data, dwSize, &dwBytesWritten, NULL);
}

inline DWORD WINAPI _AsyncReadFile(LPVOID lpParam) {
    DWORD iSize;
    char sReceivedChar;
    while (true) {
        ReadFile(_globalHSerial, &sReceivedChar, 1, &iSize, 0);  // получаем 1 байт
        if (iSize > 0 && sReceivedChar != '\t') {  // если что-то принято, выводим
            _globalAnswer.push_back(sReceivedChar);
        }
        else {
            break;
        }
    }
    return 0;
}

class COM_controller {
public:
    HANDLE hSerial;
    LPCTSTR portName;
    bool isConnected;
    COM_controller() {
        hSerial = nullptr;
        portName = nullptr;
        isConnected = false;
    }
    ~COM_controller() {}

    void disconnect() {
        HANDLE close_thread = CreateThread(NULL, 0, _AsyncClosehandle, NULL, 0, NULL);
        WaitForSingleObject(close_thread, 10);
        hSerial = nullptr;
        _globalHSerial = nullptr;
        this->~COM_controller();
    }
    void clearBuff() {
        //PurgeComm(hSerial, PURGE_RXABORT | PURGE_TXABORT | PURGE_RXCLEAR | PURGE_TXCLEAR);
        ::FlushFileBuffers(hSerial);
    }
    bool connect_to_esp32(LPCTSTR _prtName) {
        LPCTSTR sPortName = _prtName;
        _globalPortName = _prtName;
        HANDLE close_thread = CreateThread(NULL, 0, _AsyncClosehandle, NULL, 0, NULL);
        WaitForSingleObject(close_thread, 10);
        hSerial = nullptr;
        _globalHSerial = nullptr;
        HANDLE create_thread = CreateThread(NULL, 0, _AsyncCreateFile, NULL, 0, NULL);
        WaitForSingleObject(create_thread, 10);
        hSerial = _globalHSerial;
        if (hSerial == nullptr) {
            return false;
        }
        DCB dcbSerialParams = { 0 };
        dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
        if (!GetCommState(hSerial, &dcbSerialParams)) {
            //throw "getting state error\n";
        }
        dcbSerialParams.BaudRate = CBR_256000;//CBR_115200;
        dcbSerialParams.ByteSize = 8;
        dcbSerialParams.StopBits = ONESTOPBIT;
        dcbSerialParams.Parity = NOPARITY;
        if (!SetCommState(hSerial, &dcbSerialParams)) {
            //throw "error setting serial port state\n";
        }
        return true;
    }

    std::vector<std::wstring> get_available_com_potrs() {
        std::vector<std::wstring> answer;
        int r = 0;
        HKEY hkey = NULL;
        Registry^ reg;
        RegistryKey^ SERIALCOM = reg->LocalMachine->OpenSubKey(TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM\\"));
        unsigned int size = SERIALCOM->ValueCount;
        array<System::String^>^ COMs;
        COMs = SERIALCOM->GetValueNames();
        for (int i = 0; i < size; i++) {
            System::Object^ word_Obj = SERIALCOM->GetValue(COMs[i]);
            System::String^ word_String = word_Obj->ToString();
            array<wchar_t>^ word_arr_wstring = word_String->ToCharArray();
            unsigned int word_size = word_arr_wstring->Length;
            std::wstring complete_word = std::wstring(word_size, L' ');
            for (int j = 0; j < word_size; j++) {
                complete_word[j] = word_arr_wstring[j];
            }
            answer.push_back(complete_word);
        }
        return answer;
    }

    std::string read_com_port(int time) {
        _globalAnswer = "";
        HANDLE read_thread = CreateThread(NULL, 0, _AsyncReadFile, NULL, 0, NULL);
        WaitForSingleObject(read_thread, time);
        return _globalAnswer;
    }

    void write_com_port(std::string words, int time) {
        _globalWords = words;
        HANDLE write_thread = CreateThread(NULL, 0, _AsyncWriteFile, NULL, 0, NULL);
        WaitForSingleObject(write_thread, time);
    }

    std::wstring auto_find_port_esp32() {
        std::wstring answer;
        std::vector<std::wstring> tmpvec;
        tmpvec = get_available_com_potrs();
        std::string ans;
        int size_vec = tmpvec.size();
        for (int i = 0; i < size_vec; i++) {
            if (connect_to_esp32(tmpvec[i].c_str())) {
                write_com_port("1\t", 10);
                ans = read_com_port(10);
            }
            if (ans == "1.000000") {
                answer = tmpvec[i];
            }
            else {
                continue;
            }
        }
        return answer;
    }
    std::string solve(std::string equation) {
        write_com_port(equation, 100);
        return read_com_port(100);
    }
};



//std::string wait_answer_from_esp32(COM_controller _cc) {
//    while (true) {
//        std::string ans = _cc.read_com_port(10);
//        if (!ans.empty()) {
//            return ans;
//        }
//    }
//}

//void test_send_message_to_com_port(COM_controller cc, std::string answer, std::string message) {
//    cc.write_com_port(message, 10);
//    auto begin = std::chrono::steady_clock::now();
//    answer = wait_answer_from_esp32(cc);
//    auto end = std::chrono::steady_clock::now();
//    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
//    std::cout << answer << " | " << elapsed_ms.count() << " ms\n";
//}

