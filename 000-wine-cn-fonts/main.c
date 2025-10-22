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

static void output_writeconsole(const WCHAR *str)
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

static LPCWSTR installed_fonts()
{
//    WCHAR    achKey[MAX_KEY_LENGTH];   // buffer for subkey name
//    DWORD    cbName = 0;                   // size of name string
//    DWORD    cchClassName = MAX_PATH;  // size of class string

    DWORD    c= 0;              // number of values for key

//    DWORD    cbMaxSubKey = 0;              // longest subkey size
//    DWORD    cchMaxClass = 0;              // longest class string
//
//
//    DWORD    cchMaxValue = 0;          // longest value name
//    DWORD    cbMaxValueData = 0;       // longest value data
//    DWORD    cbSecurityDescriptor = 0; // size of security descriptor
//    FILETIME ftLastWriteTime;      // last write time

    LONG lResult;

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

    //wprintf(L"Keys: %d, Values: %d\n", cSubKeys, cValues);
    printf("Values: %ld\n", c);

    WCHAR s_szName[MAX_PATH];
    DWORD s_cchName;
    DWORD i, cb, type;
    /* Retrieve the value names associated with the current key */
    for(i = 0; i < c; i++)
    {
        s_cchName = _countof(s_szName);
        lResult = RegEnumValueW(hKey, i, s_szName, &s_cchName, NULL, NULL, NULL, &cb);

//      lResult = RegEnumValueW(hKey, i, szValueName, &cbName, NULL, &dwType, szValueData, &cbData);
        if (lResult != ERROR_SUCCESS)
            goto done;
        //if (s_cchName >= _countof(s_szName))
        //    continue;
        output_writeconsole(s_szName);
        output_writeconsole(L"\n");
        //ppszNames[i] = _wcsdup(s_szName);
    }

//    LONG lResult;
//    DWORD dwIndex, dwType, cbName, cbData;
//    FILETIME ft;
//    WCHAR szSubKey[256];
//
//    dwIndex = 0;
//    do
//    {
//        cbName = ARRAY_SIZE(szValueName);
//        cbData = ARRAY_SIZE(szValueData);
//        lResult = RegEnumValueW(hKey, dwIndex++, szValueName, &cbName, NULL, &dwType, szValueData, &cbData);
//        if (lResult == ERROR_SUCCESS)
//        {
//            lResult = RegSetValueExW(hDestSubKey, szValueName, 0, dwType, szValueData, cbData);
//            if (lResult)
//                goto done;
//        }
//    }
//    while(lResult == ERROR_SUCCESS);

done:
    //output_writeconsole(const WCHAR *str, DWORD wlen);
//    subkey_name = malloc(MAX_SUBKEY_LEN * sizeof(WCHAR));
//
//    path_len = lstrlenW(path);
//
//    i = 0;
//    for (;;)
//    {
//        subkey_len = MAX_SUBKEY_LEN;
//        rc = RegEnumKeyExW(key, i, subkey_name, &subkey_len, NULL, NULL, NULL, NULL);
//        if (rc == ERROR_SUCCESS)
//        {
//            subkey_path = build_subkey_path(path, path_len, subkey_name, subkey_len);
//            if (!RegOpenKeyExW(key, subkey_name, 0, KEY_READ, &subkey))
//            {
//                export_registry_data(fp, subkey, subkey_path, unicode);
//                RegCloseKey(subkey);
//            }
//            free(subkey_path);
//            i++;
//        }
//        else break;
//    }
//
//    free(subkey_name);
//
//    lRet = RegQueryValueExW(hkey, name, 0, &type, 0, &size);
//    if (lRet == ERROR_FILE_NOT_FOUND ) {
//        printf("key is not exists\n");
//        type = REG_SZ;
//        lRet = RegSetValueExW(hkey, name, 0, type, (LPBYTE)value,
//                              (DWORD)(wcslen(value) + 1) * sizeof(WCHAR));
//        if (lRet == ERROR_SUCCESS)
//            return TRUE;
//        else
//            return FALSE;
//    } else if (lRet == ERROR_SUCCESS) {
//        printf("query success\n");
//        printf("res: %ld, type: %ld, size: %ld\n", lRet, type, size);
//        if (type == REG_SZ || type == REG_NONE) {
//            lRet = RegSetValueExW(hkey, name, 0, type, (LPBYTE)value,
//                                  (DWORD)(wcslen(value) + 1) * sizeof(WCHAR));
//            if (lRet == ERROR_SUCCESS)
//                return TRUE;
//            else
//                return FALSE;

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
    installed_fonts();
    //CloseHandle(console);
    return 0;
}
