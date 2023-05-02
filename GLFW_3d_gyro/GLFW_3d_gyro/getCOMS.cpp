#include <windows.h>
#include <tchar.h>
//#include "COM_controller.h"

void ShowCOMPorts()
{
    int r = 0;
    HKEY hkey = NULL;
    //��������� ������ �������, � ������� �������� ����������� � COM ������
    r = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM\\"), 0, KEY_READ, &hkey);
    if (r != ERROR_SUCCESS)
        return;

    unsigned long CountValues = 0, MaxValueNameLen = 0, MaxValueLen = 0;
    //�������� ���������� �� �������� ������� �������
    RegQueryInfoKey(hkey, NULL, NULL, NULL, NULL, NULL, NULL, &CountValues, &MaxValueNameLen, &MaxValueLen, NULL, NULL);
    ++MaxValueNameLen;
    //�������� ������
    TCHAR* bufferName = NULL, * bufferData = NULL;
    bufferName = (TCHAR*)malloc(MaxValueNameLen * sizeof(TCHAR));
    if (!bufferName)
    {
        RegCloseKey(hkey);
        return;
    }
    bufferData = (TCHAR*)malloc((MaxValueLen + 1) * sizeof(TCHAR));
    if (!bufferData)
    {
        free(bufferName);
        RegCloseKey(hkey);
        return;
    }

    unsigned long NameLen, type, DataLen;
    //���� �������� ���������� ������� �������
    for (unsigned int i = 0; i < CountValues; i++)
    {
        NameLen = MaxValueNameLen;
        DataLen = MaxValueLen;
        r = RegEnumValue(hkey, i, bufferName, &NameLen, NULL, &type, (LPBYTE)bufferData, &DataLen);
        if ((r != ERROR_SUCCESS) || (type != REG_SZ))
            continue;

        _tprintf(TEXT("%s\n"), bufferData);
    }
    //����������� ������
    free(bufferName);
    free(bufferData);
    //��������� ������ �������
    RegCloseKey(hkey);
}
//
//int main()
//{
//    std::vector<std::string> tmpvec;
//    COM_controller cc;
//    tmpvec = cc.get_available_com_potrs();
//    for (auto& elem : tmpvec) {
//        std::cout << elem << std::endl;
//    }
//    //ShowCOMPorts();
//    return 0;
//}