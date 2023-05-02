#pragma once
#include <windows.h>
#include <iostream>
#include <vector>
#include <chrono>
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
    const int buff_size = 64;
    char data[buff_size] = { 0 };  // строка дл€ передачи
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
        if (iSize > 0 && sReceivedChar != '\t') {  // если что-то прин€то, выводим
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
	COM_controller() {
        hSerial = nullptr;
        portName = nullptr;
    }
	~COM_controller() {}

    void disconnect() {
        HANDLE close_thread = CreateThread(NULL, 0, _AsyncClosehandle, NULL, 0, NULL);
        WaitForSingleObject(close_thread, 10);
        hSerial = nullptr;
        _globalHSerial = nullptr;
        this->~COM_controller();
        std::cout << "\ndisconnect";
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
        //ќткрываем раздел реестра, в котором хранитс€ иинформаци€ о COM портах
        r = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM\\"), 0, KEY_READ, &hkey);
        if (r != ERROR_SUCCESS) {
            throw std::runtime_error("ERROR");
        }
        unsigned long CountValues = 0, MaxValueNameLen = 0, MaxValueLen = 0;
        //ѕолучаем информацию об открытом разделе реестра
        RegQueryInfoKey(hkey, NULL, NULL, NULL, NULL, NULL, NULL, &CountValues, &MaxValueNameLen, &MaxValueLen, NULL, NULL);
        ++MaxValueNameLen;
        //¬ыдел€ем пам€ть
        TCHAR* bufferName = NULL, * bufferData = NULL;
        bufferName = (TCHAR*)malloc(MaxValueNameLen * sizeof(TCHAR));
        if (!bufferName) {
            RegCloseKey(hkey);
            throw std::runtime_error("ERROR");
        }
        bufferData = (TCHAR*)malloc((MaxValueLen + 1) * sizeof(TCHAR));
        if (!bufferData) {
            free(bufferName);
            RegCloseKey(hkey);
            throw std::runtime_error("ERROR");
        }

        unsigned long NameLen, type, DataLen;
        //÷икл перебора параметров раздела реестра
        for (unsigned int i = 0; i < CountValues; i++)
        {
            NameLen = MaxValueNameLen;
            DataLen = MaxValueLen;
            r = RegEnumValue(hkey, i, bufferName, &NameLen, NULL, &type, (LPBYTE)bufferData, &DataLen);
            if ((r != ERROR_SUCCESS) || (type != REG_SZ))
                continue;

            std::wstring tmp = bufferData;
            answer.push_back(std::wstring(begin(tmp), end(tmp)));
        }
        //ќсвобождаем пам€ть
        free(bufferName);
        free(bufferData);
        //«акрываем раздел реестра
        RegCloseKey(hkey);
        return answer;
	}

    std::string read_com_port() {
        _globalAnswer = "";
        HANDLE read_thread = CreateThread(NULL, 0, _AsyncReadFile, NULL, 0, NULL);
        WaitForSingleObject(read_thread, 10);
        return _globalAnswer;
    }

    void write_com_port(std::string words) {
        _globalWords = words;
        HANDLE write_thread = CreateThread(NULL, 0, _AsyncWriteFile, NULL, 0, NULL);
        WaitForSingleObject(write_thread, 10);
    }

    std::wstring auto_find_port_esp32() {
        std::wstring answer;
        std::vector<std::wstring> tmpvec;
        tmpvec = get_available_com_potrs();
        std::string ans;
        int size_vec = tmpvec.size();
        for (int i = 0; i < size_vec; i++) {
            if (connect_to_esp32(tmpvec[i].c_str())) {
                write_com_port("1\t");
                ans = read_com_port();
                std::cout << "\nTry connect: " << ans;
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
};
