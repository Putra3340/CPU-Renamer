#include <iostream>
#include <Windows.h>
#include <locale>
#include <codecvt>
#include <string>
#include <fstream>
void UpdateRegistryValue(const wchar_t* processorName) {
    HKEY hKey;
    LONG result = RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_SET_VALUE, &hKey);

    if (result == ERROR_SUCCESS) {
        result = RegSetValueExW(hKey, L"ProcessorNameString", 0, REG_SZ, reinterpret_cast<const BYTE*>(processorName), (wcslen(processorName) + 1) * sizeof(wchar_t));

        if (result == ERROR_SUCCESS) {
            std::wcout << L"Updated successfully." << std::endl;
        }
        else {
            std::wcerr << L"Error updating registry key: " << result << std::endl;
        }

        RegCloseKey(hKey);
    }
    else {
        std::wcerr << L"Error opening registry key: " << result << std::endl;
    }
}

bool fileExists(const std::string& filename) {
    DWORD attributes = GetFileAttributesA(filename.c_str());
    return (attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY));
}

std::string WStringToString(const std::wstring& wstr) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(wstr);
}

std::string Restore() {
    std::ifstream file("backup.dat");
    std::string proc;
    std::getline(file, proc);
    file.close();
    return proc;
}

void Backup(std::string backup) {
    std::ofstream out("backup.dat");
    out << backup;
    out.close();
}

std::string GetRegistryValue(const wchar_t* keyPath, const wchar_t* valueName) {
    HKEY hKey;
    LONG result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, keyPath, 0, KEY_READ, &hKey);

    if (result == ERROR_SUCCESS) {
        wchar_t buffer[MAX_PATH];
        DWORD bufferSize = sizeof(buffer);

        result = RegQueryValueEx(hKey, valueName, 0, NULL, (LPBYTE)buffer, &bufferSize);

        if (result == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            // Convert the wide string to a standard string
            return WStringToString(buffer);
        }
        else {
            RegCloseKey(hKey);
            throw std::runtime_error("Error reading registry value");
        }
    }
    else {
        throw std::runtime_error("Error opening registry key");
    }
}

void menu() {
    int option = 0;
    std::wcout << "-------------------" << std::endl << "1. Change Name" << std::endl << "2. Restore Default" << std::endl << std::endl << "->";
    std::cin >> option;
    std::cin.ignore();
    std::cout << std::endl;
    if (option == 1)
    {
        std::wcout << L"Enter the new processor name: ";
        wchar_t processorName[256]; // Adjust the size based on your needs
        std::wcin.getline(processorName, sizeof(processorName) / sizeof(processorName[0]));
        UpdateRegistryValue(processorName);
    }
    else if (option == 2)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        std::wstring proc = converter.from_bytes(Restore());
        UpdateRegistryValue(proc.c_str());
    }
}

int main() {
    // Specify the registry key path and value name
    const wchar_t* keyPath = L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0";
    const wchar_t* valueName = L"ProcessorNameString";
    const std::string filename = "backup.dat";

    // Call the function to get the registry value
    std::string processorName = GetRegistryValue(keyPath, valueName);

    // Display the result
    std::cout << "Current Processor Name: " << processorName << std::endl;


    if (fileExists(filename)) {
        menu();
    }
    else {
        Backup(processorName);
        menu();
    }

    return 0;
}
