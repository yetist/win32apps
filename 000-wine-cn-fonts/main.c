/*
 * SPDX-FileCopyrightText: Copyright (c) 2025 yetist <yetist@gmail.com>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <wchar.h>
#include <stdio.h>

LPWSTR chinese_fonts[] = {
    L"wqy-microhei.ttc",
    L"wqy-zenhei.ttc",
    L"NotoSerifCJK-Regular.ttc",
    L"SourceHanSansCN-Normal.otf",
    NULL
};

static HKEY openKey(HKEY rootkey, LPCWSTR subkey)
{
    HKEY hKey = NULL;
    LONG lResult;

    lResult = RegOpenKeyExW(rootkey, subkey, 0, KEY_ALL_ACCESS, &hKey);

    if (lResult == ERROR_FILE_NOT_FOUND)
    {
        lResult = RegCreateKeyExW(rootkey, subkey, 0, NULL, REG_OPTION_NON_VOLATILE,
                                  KEY_ALL_ACCESS, NULL, &hKey, NULL);
        if (lResult) {
            wprintf(L"Error: %ld: Could not create %ls\n", lResult,  subkey);
        }
    }

    return hKey;
}

static BOOL writeRegKey(HKEY hkey, LPCWSTR name, LPCWSTR value)
{
    LONG lRet;
    DWORD type;
    DWORD size;

    lRet = RegQueryValueExW(hkey, name, 0, &type, 0, &size);
    if (lRet == ERROR_FILE_NOT_FOUND ) {
        printf("key is not exists\n");
        type = REG_SZ;
        lRet = RegSetValueExW(hkey, name, 0, type, (LPBYTE)value,
                              (DWORD)(wcslen(value) + 1) * sizeof(WCHAR));
        if (lRet == ERROR_SUCCESS)
            return TRUE;
        else
            return FALSE;
    } else if (lRet == ERROR_SUCCESS) {
        printf("query success\n");
        printf("res: %ld, type: %ld, size: %ld\n", lRet, type, size);
        if (type == REG_SZ || type == REG_NONE) {
            lRet = RegSetValueExW(hkey, name, 0, type, (LPBYTE)value,
                                  (DWORD)(wcslen(value) + 1) * sizeof(WCHAR));
            if (lRet == ERROR_SUCCESS)
                return TRUE;
            else
                return FALSE;
        } else if (type == REG_MULTI_SZ) {
            size_t vlen, bufsize;
            LPWSTR s, buf = NULL;
            LPWSTR stringValueData = NULL;

            if (!(stringValueData = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size + sizeof(WCHAR))))
            {
                printf("size alloc error\n");
                //goto done;
            }
            lRet = RegQueryValueExW(hkey, name, 0, 0, (LPBYTE)stringValueData, &size);
            if (lRet != ERROR_SUCCESS)
            {
                //error(hwnd, IDS_BAD_VALUE, name);
                printf("name error\n");
                //goto done;
            }

            s = wcsstr(stringValueData, value);
            if (!s) {
                vlen = wcslen(value);
                bufsize = size + (vlen +1) * sizeof(WCHAR);
                buf = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, bufsize);

                memcpy(buf, value, vlen * sizeof(WCHAR));
                buf[vlen++] = L'\0';
                memcpy((buf + vlen), stringValueData, size * sizeof(WCHAR));
                lRet = RegSetValueExW(hkey, name, 0, type, (LPBYTE)buf, (DWORD)bufsize);
                HeapFree(GetProcessHeap(), 0, buf);
                HeapFree(GetProcessHeap(), 0, stringValueData);
                if (lRet != ERROR_SUCCESS) {
                    printf("write multi line error\n");
                }
            }
        }
        return TRUE;
    }
    return TRUE;
}

void replace_font(LPCWSTR fontname, LPCWSTR fontfile)
{
    HKEY hKey = openKey(HKEY_LOCAL_MACHINE,
                        L"Software\\Microsoft\\Windows NT\\CurrentVersion\\FontLink\\SystemLink");
    //WCHAR value[256];
    //DWORD size = 0;
    //getRegKey(hKey, fontname, value, size);
    //wprintf(L"key: %s, size: %d", value, size);
    writeRegKey(hKey, fontname, fontfile);
    RegCloseKey(hKey);
}

static void output_writeconsole(LPCWSTR str)
{
    DWORD count;

    if (!WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), str, wcslen(str), &count, NULL))
    {
        DWORD len;
        char  *msgA;

        /* WriteConsole() fails on Windows if its output is redirected. If this occurs,
         * we should call WriteFile() with OEM code page.
         */
        len = WideCharToMultiByte(GetOEMCP(), 0, str, wcslen(str), NULL, 0, NULL, NULL);
        msgA = malloc(len);
        if (!msgA)
            return;

        WideCharToMultiByte(GetOEMCP(), 0, str, wcslen(str), msgA, len, NULL, NULL);
        WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), msgA, len, &count, FALSE);
        free(msgA);
    }
}

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

// Function to convert LPBYTE (assuming it's a null-terminated ANSI string) to wchar_t*
wchar_t* ConvertLPBYTEToWChar(LPBYTE lpByteData)
{
    if (!lpByteData)
    {
        return NULL;
    }

    // Calculate the required buffer size for the wide-character string
    // CP_ACP specifies the system's default ANSI codepage
    int requiredSize = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)lpByteData, -1, NULL, 0);
    if (requiredSize == 0)
    {
        return NULL;
    }

    // Allocate memory for the wide-character string
    //wchar_t* wideCharString = new wchar_t[requiredSize];

    wchar_t* wideCharString = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, requiredSize);
    if (!wideCharString)
    {
        // Handle memory allocation error
        return NULL;
    }

    // Perform the conversion
    MultiByteToWideChar(CP_ACP, 0, (LPCSTR)lpByteData, -1, wideCharString, requiredSize);
    return wideCharString;
}


static LPCWSTR installed_fonts(LPCWSTR font)
{
    DWORD c= 0;              // number of values for key
    LONG  lResult;
    WCHAR name[MAX_PATH];
    DWORD name_size;
    DWORD i, size, type;
    LPBYTE value = NULL;
    LPWSTR s;

    HKEY hKey = openKey(HKEY_LOCAL_MACHINE,
                        L"Software\\Microsoft\\Windows\\CurrentVersion\\Fonts");
    lResult = RegQueryInfoKeyW(hKey,                    // key handle
                               NULL,                // buffer for class name
                               NULL,           // size of class string
                               NULL,                    // reserved
                               NULL,               // number of subkeys
                               NULL,            // longest subkey size
                               NULL,            // longest class string
                               &c,                // number of values for this key
                               NULL,            // longest value name
                               NULL,         // longest value data
                               NULL,   // security descriptor
                               NULL);       // last write time

    for(i = 0; i < c; i++)
    {
        name_size = _countof(name);
        lResult = RegEnumValueW(hKey, i, name, &name_size, NULL, &type, NULL, &size);
        if (lResult == ERROR_SUCCESS)
        {
            value = malloc(size);
            lResult = RegQueryValueExW(hKey, name, NULL, &type, value, &size);
            s = wcsstr((LPCWSTR)value, font);
            if (s) {
                wprintf(L"found font: %ls\n", font);
            }
            free(value);
        }
    }
    RegCloseKey(hKey);
    return NULL;
}

int wmain(int argc, wchar_t **argv)
{
    SetConsoleOutputCP(65001);

    WCHAR* message = L"Hello, 世界Win32 Console!\n";
    output_writeconsole(message);

    WCHAR* font_file = L"wqy-microhei.ttc";
    if (argc == 2) {
        font_file = argv[1];
    }

    HKEY hKey = openKey(HKEY_LOCAL_MACHINE,
                        L"Software\\Microsoft\\Windows NT\\CurrentVersion\\FontLink\\SystemLink");
    writeRegKey(hKey, L"Lucida Sans Unicode", font_file);
    writeRegKey(hKey, L"Microsoft Sans Serif", font_file);
    writeRegKey(hKey, L"Arial", font_file);
    writeRegKey(hKey, L"Courier New", font_file);
    writeRegKey(hKey, L"MS Sans Serif", font_file);
    writeRegKey(hKey, L"NSimSun", font_file);
    writeRegKey(hKey, L"SimSun", font_file);
    writeRegKey(hKey, L"Tahoma", font_file);
    writeRegKey(hKey, L"Times New Roman", font_file);

    RegCloseKey(hKey);
    installed_fonts(font_file);
    //CloseHandle(console);
    return 0;
}
