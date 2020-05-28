#include <windows.h>
#include <iostream>

using namespace std;

const int kTimeout = 1000;

void Server()
{
    STARTUPINFO cif;
    ZeroMemory(&cif, sizeof(STARTUPINFO));
    PROCESS_INFORMATION pi;

    CreateProcess(L"D:\\�����\\4 ���\\���\\lab7client\\Debug\\lab7client.exe", NULL, NULL, NULL, NULL, CREATE_NEW_CONSOLE, NULL, NULL, &cif, &pi); // �������� ��������

    string ComName = "COM1";
    HANDLE Semaphores[3];


    Semaphores[0] = CreateSemaphore(NULL, 0, 1, L"SEMAPHORE");       // ������� ����������.
    Semaphores[1] = CreateSemaphore(NULL, 0, 1, L"SEMAPHORE_END");   // ������� ����������.
    Semaphores[2] = CreateSemaphore(NULL, 0, 1, L"SEMAPHORE_EXIT");  // ������� ������.

    HANDLE Handler = CreateFileA(
        (LPCSTR)ComName.c_str(),                                            // ��� ������������ �����.
        GENERIC_READ | GENERIC_WRITE,                               // ��� ������� � �����.
        0,                                                          // ��������� ����������� �������.
        NULL,                                                       // �������� ������ �����.
        OPEN_EXISTING,                                              // ����� ������������.
        FILE_ATTRIBUTE_NORMAL,                                      // ����������� ����� ������.
        NULL                                                        // ��������� ����� �������.
    );

    if (Handler == INVALID_HANDLE_VALUE)
        cout << "erorr occured! id of error: " << GetLastError() << endl;
    
    SetCommMask(Handler, EV_RXCHAR);                                // ������������� ����� �� ������� �����.
    SetupComm(Handler, 1500, 1500);                                 // �������������� ���������������� ��������� ��� ��������� ���������� (����������, ����� ����� � ������)

    COMMTIMEOUTS CommTimeOuts;                                      // ���������, ��������������� ��������� ��������� ����������������� �����.
    CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF;                  // M����������� ����� ��� ��������� ����� ������������ ���� �������� �� ����� �����.
    CommTimeOuts.ReadTotalTimeoutMultiplier = 0;                    // ���������, ������������, ����� ��������� ������ ������ ������� ������� ��� �������� ������.
    CommTimeOuts.ReadTotalTimeoutConstant = kTimeout;               // ���������, ������������, ����� ��������� ������ (������������) ������ ������� ������� ��� �������� ������.
    CommTimeOuts.WriteTotalTimeoutMultiplier = 0;                   // ���������, ������������, ����� ��������� ������ ������ ������� ������� ��� �������� ������.
    CommTimeOuts.WriteTotalTimeoutConstant = kTimeout;              // ���������, ������������, ����� ��������� ������ ������ ������� ������� ��� �������� ������.

    if (!SetCommTimeouts(Handler, &CommTimeOuts))
    {
        CloseHandle(Handler);
        Handler = INVALID_HANDLE_VALUE;
        return;
    }

    DCB ComDCM;                               // ���������, ��������������� �������� ��������� ����������������� �����.
    memset(&ComDCM, 0, sizeof(ComDCM));       // ��������� ������ ��� ���������.
    ComDCM.DCBlength = sizeof(DCB);           // ������ �����, � ������, ���������.
    GetCommState(Handler, &ComDCM);           // ��������� ������ � ������� ���������� ����������� �������� ��� ���������� ����������.
    ComDCM.BaudRate = DWORD(9600);            // �������� �������� ������.
    ComDCM.ByteSize = 8;                      // ���������� ����� �������������� ��� � ������������ � ����������� ������.
    ComDCM.Parity = NOPARITY;                 // ���������� ����� ����� �������� �������� (��� ��������� �����������).
    ComDCM.StopBits = ONESTOPBIT;             // ������ ���������� �������� ��� (���� ���).
    ComDCM.fAbortOnError = TRUE;              // ������ ������������� ���� �������� ������/������ ��� ������������� ������.
    ComDCM.fDtrControl = DTR_CONTROL_DISABLE; // ������ ����� ���������� ������� ��� ������� DTR.
    ComDCM.fRtsControl = RTS_CONTROL_DISABLE; // ������ ����� ���������� ������� ��� ������� RTS.
    ComDCM.fBinary = TRUE;                    // �������� �������� ����� ������.
    ComDCM.fParity = FALSE;                   // �������� ����� �������� ��������.
    ComDCM.fInX = FALSE;                      // ������ ������������� XON/XOFF ���������� ������� ��� ������.
    ComDCM.fOutX = FALSE;                     // ������ ������������� XON/XOFF ���������� ������� ��� ��������.
    ComDCM.XonChar = 0;                       // ������ ������ XON ������������ ��� ��� ������, ��� � ��� ��������.
    ComDCM.XoffChar = (unsigned char)0xFF;    // ������ ������ XOFF ������������ ��� ��� ������, ��� � ��� ��������.
    ComDCM.fErrorChar = FALSE;                // ������ ������, �������������� ��� ������ �������� � ��������� ���������.
    ComDCM.fNull = FALSE;                     // ��������� �� ������������� ������ �������� � ������� �������� �� ������ ���������� ����� ErrorChar.
    ComDCM.fOutxCtsFlow = FALSE;              // �������� ����� �������� �� �������� CTS.
    ComDCM.fOutxDsrFlow = FALSE;              // �������� ����� �������� �� �������� DSR.
    ComDCM.XonLim = 128;                      // ������ ����������� ����� �������� � �������� ������ ����� �������� ������� XON.
    ComDCM.XoffLim = 128;                     // ���������� ������������ ���������� ���� � �������� ������ ����� �������� ������� XOFF.

    if (!SetCommState(Handler, &ComDCM))
    {
        CloseHandle(Handler);
        Handler = INVALID_HANDLE_VALUE;
        return;
    }

    cout << "type quit to stop the programm\n\n";

    while (true)
    {
        string message;
        DWORD NumberOfBytesWritten;
        char buffer[20];
        int bufferSize = sizeof(buffer);

        cout << "your message: ";
        cin.clear();
        cin >> message;
        if (message == "quit")
        {                                                             // ������� ������ �� ���������.
            ReleaseSemaphore(Semaphores[2], 1, NULL);                 // ���������� �������� ������ (������ � ���������� ���������).
            break;
        }

        ReleaseSemaphore(Semaphores[0], 1, NULL);                     // ���������� �������� ��������� (������ � ���������� ���������).

        int NumberOfBlocks = message.size() / bufferSize + 1;         // ��������� ����� ������ 
        WriteFile(Handler, &NumberOfBlocks, sizeof(NumberOfBlocks), &NumberOfBytesWritten, NULL);

        int size = message.size();
        WriteFile(Handler, &size, sizeof(size), &NumberOfBytesWritten, NULL);

        for (int i = 0; i < NumberOfBlocks; i++)
        {
            message.copy(buffer, bufferSize, i * bufferSize);
            if (!WriteFile(Handler, buffer, bufferSize, &NumberOfBytesWritten, NULL))
                cout << "error!" << endl;
        }
        WaitForSingleObject(Semaphores[1], INFINITE);                 // �������� �������� � ������������ ���������.
    }

    CloseHandle(Handler);
    CloseHandle(Semaphores[0]);
    CloseHandle(Semaphores[1]);
    cout << "\n";
    system("pause");
    return;
}

int main()
{
    system("cls");
    Server();
}