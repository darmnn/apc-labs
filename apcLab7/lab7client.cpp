#include <windows.h>
#include <iostream>

using namespace std;

const int kTimeout = 1000;

void Client()
{
    HANDLE Semaphores[3];

    char buffer[20];
    int bufferSize = sizeof(buffer);


    string ComName = "COM2";


    Semaphores[0] = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, L"SEMAPHORE");
    Semaphores[1] = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, L"SEMAPHORE_END");
    Semaphores[2] = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, L"SEMAPHORE_EXIT");

    HANDLE hMyPipe = CreateFileA((LPCSTR)ComName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    while (true)
    {
        string message;
        bool successFlag = TRUE;
        DWORD NumberOfBytesRead;
        message.clear();

        int index = WaitForMultipleObjects(3, Semaphores, FALSE, INFINITE) - WAIT_OBJECT_0;
        if (index == 2)
            break;

        int NumberOfBlocks;
        if (!ReadFile(hMyPipe, &NumberOfBlocks, sizeof(NumberOfBlocks), &NumberOfBytesRead, NULL))
            break;

        int size;
        if (!ReadFile(hMyPipe, &size, sizeof(size), &NumberOfBytesRead, NULL))
            break;

        for (int i = 0; i < NumberOfBlocks; i++)
        {
            successFlag = ReadFile(hMyPipe, buffer, bufferSize, &NumberOfBytesRead, NULL);
            if (!successFlag)
                break;
            message.append(buffer, bufferSize);
        }

        if (!successFlag)
            break;

        message.resize(size);

        for (int i = 0; i < size; i++)
            cout << message[i];
        cout << endl;
        ReleaseSemaphore(Semaphores[1], 1, NULL);
    }
    CloseHandle(hMyPipe);
    CloseHandle(Semaphores[0]);
    CloseHandle(Semaphores[1]);
    return;
}

int main()
{
    system("cls");
    Client();
}