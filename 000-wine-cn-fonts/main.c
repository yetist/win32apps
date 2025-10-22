/*
 * SPDX-FileCopyrightText: Copyright (c) 2025 yetist <yetist@gmail.com>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <wchar.h>
#include <shlwapi.h>
#include <stdio.h>

LPCWSTR chinese_linux_fonts[] = {
    L"wqy-microhei.ttc",
    L"wqy-zenhei.ttc",
    L"NotoSerifCJK-Regular.ttc",
    L"SourceHanSansCN-Normal.otf",
    NULL
};

static HKEY open_key(HKEY root_key, LPCWSTR sub_key)
{
    HKEY key = NULL;
    LONG result;

    result = RegOpenKeyExW(root_key, sub_key, 0, KEY_ALL_ACCESS, &key);

    if (result == ERROR_FILE_NOT_FOUND)
    {
        result = RegCreateKeyExW(root_key, sub_key, 0, NULL, REG_OPTION_NON_VOLATILE,
                                 KEY_ALL_ACCESS, NULL, &key, NULL);
        if (result) {
            wprintf(L"Error: %ld: Could not create %ls\n", result,  sub_key);
        }
    }

    return key;
}

static BOOL writeRegKey(HKEY hkey, LPCWSTR name, LPCWSTR value)
{
    LONG result;
    DWORD type;
    DWORD size;

    result = RegQueryValueExW(hkey, name, 0, &type, 0, &size);
    if (result == ERROR_FILE_NOT_FOUND ) {
        type = REG_SZ;
        result = RegSetValueExW(hkey,
                                name,
                                0,
                                type,
                                (LPBYTE)value,
                                (DWORD)(wcslen(value) + 1) * sizeof(WCHAR));
        if (result == ERROR_SUCCESS)
            return TRUE;
        else
            return FALSE;
    } else if (result == ERROR_SUCCESS) {
        if (type == REG_SZ || type == REG_NONE) {
            result = RegSetValueExW(hkey,
                                    name,
                                    0,
                                    type,
                                    (LPBYTE)value,
                                    (DWORD)(wcslen(value) + 1) * sizeof(WCHAR));
            if (result == ERROR_SUCCESS) {
                wprintf(L"字体: \"%ls\" 使用 \"%s\" 文件\n", name, value);
                //wprintf(L"使用字体 %ls 替换 %s\n", value, name);
                return TRUE;
            }
            else
            return FALSE;
        } else if (type == REG_MULTI_SZ) {
            size_t vlen, bufsize;
            LPWSTR s, buf = NULL;
            LPWSTR value_data = NULL;

            if (!(value_data = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size + sizeof(WCHAR)))) {
                return FALSE;
            }
            result = RegQueryValueExW(hkey, name, 0, 0, (LPBYTE)value_data, &size);
            if (result != ERROR_SUCCESS) {
                if (value_data){
                    HeapFree(GetProcessHeap(), 0, value_data);
                }
                return FALSE;
            }
            s = wcsstr(value_data, value);
            if (!s) {
                vlen = wcslen(value);
                bufsize = size + (vlen +1) * sizeof(WCHAR);
                buf = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, bufsize);

                memcpy(buf, value, vlen * sizeof(WCHAR));
                buf[vlen++] = L'\0';
                memcpy((buf + vlen), value_data, size * sizeof(WCHAR));
                result = RegSetValueExW(hkey, name, 0, type, (LPBYTE)buf, (DWORD)bufsize);
                HeapFree(GetProcessHeap(), 0, buf);
                if (result == ERROR_SUCCESS) {
                    //wprintf(L"使用字体 %ls 替换 %s\n", value, name);
                    wprintf(L"字体: \"%ls\" 使用 \"%s\" 文件\n", name, value);
                }
            }
            if (value_data){
                HeapFree(GetProcessHeap(), 0, value_data);
            }
        }
        return TRUE;
    }
    return TRUE;
}

void wine_replace_font(LPCWSTR font_file)
{
    HKEY hKey = open_key(HKEY_LOCAL_MACHINE,
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


static BOOL linux_has_font (LPCWSTR font)
{
    DWORD c = 0;
    LSTATUS lResult;
    WCHAR name[MAX_PATH];
    DWORD name_size;
    DWORD i, size, type;
    LPBYTE value = NULL;
    LPWSTR s;
    BOOL found = FALSE;

    HKEY hKey = open_key(HKEY_LOCAL_MACHINE,
                         L"Software\\Microsoft\\Windows\\CurrentVersion\\Fonts");
    lResult = RegQueryInfoKeyW(hKey,    // key handle
                               NULL,    // buffer for class name
                               NULL,    // size of class string
                               NULL,    // reserved
                               NULL,    // number of subkeys
                               NULL,    // longest subkey size
                               NULL,    // longest class string
                               &c,      // number of values for this key
                               NULL,    // longest value name
                               NULL,    // longest value data
                               NULL,    // security descriptor
                               NULL);   // last write time
    if (lResult != ERROR_SUCCESS) {
        if (hKey != NULL)
            RegCloseKey(hKey);
        return FALSE;
    }

    for(i = 0; i < c; i++) {
        name_size = _countof(name);
        lResult = RegEnumValueW(hKey, i, name, &name_size, NULL, &type, NULL, &size);
        if (lResult == ERROR_SUCCESS)
        {
            value = malloc(size);
            lResult = RegQueryValueExW(hKey, name, NULL, &type, value, &size);
            if (lResult != ERROR_SUCCESS) {
                free(value);
                continue;
            }
            s = wcsstr((LPCWSTR)value, font);
            if (s) {
                if (PathFileExistsW((LPCWSTR)value)) {
                    wprintf(L"found font: %ls\n", font);
                    found = TRUE;
                    free(value);
                    break;
                }
            }
            free(value);
        }
    }

    if (value != NULL)
        free(value);

    if (hKey != NULL)
        RegCloseKey(hKey);
    return found;
}

int wmain(int argc, wchar_t **argv)
{
    SetConsoleOutputCP(65001);

    WCHAR* message = L"Hello, 世界Win32 Console!\n";
    output_writeconsole(message);

    for (int i = 0; chinese_linux_fonts[i]; ++i) {
        wprintf(L"data is %s\n", chinese_linux_fonts[i]);
        if (linux_has_font(chinese_linux_fonts[i])) {
            wine_replace_font(chinese_linux_fonts[i]);
            break;
        }
    }
    return 0;
}
