#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
#include <cstdlib>

void Writer(std::string backup) {
    std::ofstream out("backup.dat");
    out << backup;
    out.close();
}

int main() {
    HKEY hKey;
    DWORD dwType = REG_SZ;
    wchar_t szProcessorName[MAX_PATH];
    DWORD dwSize = sizeof(szProcessorName);

    // Open the registry key
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
        L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
        0,
        KEY_READ | KEY_SET_VALUE,  // Added KEY_SET_VALUE for write access
        &hKey) == ERROR_SUCCESS) {

        // Read the ProcessorNameString value
        if (RegQueryValueExW(hKey,
            L"ProcessorNameString",
            NULL,
            &dwType,
            (LPBYTE)szProcessorName,
            &dwSize) == ERROR_SUCCESS) {

            //Check The Backup Dat
            const char* file = "backup.dat";
            struct stat sb;
            if (stat(file, &sb) == 0 && !(sb.st_mode & S_IFDIR)) {
                //nah i leave it blank
            }
            else{
                /*if didnt have backup
                and convert wchar_t to string to make backup
                */
                std::cout << std::endl;
            int bufferSize = wcstombs(nullptr, szProcessorName, 0);
            if (bufferSize == -1) {
                std::cerr << "Conversion error" << std::endl;
                return 1;
            }
            char* multibyteStr = new char[bufferSize + 1];
            if (wcstombs(multibyteStr, szProcessorName, bufferSize + 1) == -1) {
                std::cerr << "Conversion error" << std::endl;
                delete[] multibyteStr;
                return 1;
            }
            std::string utf8Str(multibyteStr);
            delete[] multibyteStr;
            Writer(utf8Str);
        }

            std::wcout << L"Current Processor Name: " << szProcessorName << std::endl;

            // Prompt the user for a new ProcessorNameString
            std::wcin.getline(szProcessorName, sizeof(szProcessorName) / sizeof(wchar_t));

            // Write the updated ProcessorNameString back to the registry
            if (RegSetValueExW(hKey,
                L"ProcessorNameString",
                0,
                dwType,
                (LPBYTE)szProcessorName,
                (wcslen(szProcessorName) + 1) * sizeof(wchar_t)) == ERROR_SUCCESS) {
                std::wcout << L"Processor Name updated successfully." << std::endl;
            }
            else {
                std::cerr << "Failed to update ProcessorNameString." << std::endl;
            }
        }
        else {
            std::cerr << "Failed to read ProcessorNameString." << std::endl;
        }

        // Close the registry key
        RegCloseKey(hKey);
    }
    else {
        std::cerr << "Failed to open the registry key." << std::endl;
    }

    return 0;
}
