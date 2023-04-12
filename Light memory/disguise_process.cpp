#include "disguise_process.h"
#include <winternl.h>

typedef
NTSTATUS(WINAPI* pfnNtQueryInformationProcess)
(HANDLE ProcessHandle, ULONG ProcessInformationClass,
    PVOID ProcessInformation, UINT32 ProcessInformationLength,
    UINT32* ReturnLength);

bool DisguiseProcess(const DWORD process_id, const wchar_t* image_path, const wchar_t* command_line) {
    // 打开进程获取句柄
    HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);
    if (hProcess == nullptr)
        return false;

    do {
        pfnNtQueryInformationProcess NtQueryInformationProcess = nullptr;
        PROCESS_BASIC_INFORMATION pbi = { 0 };

        // 需要通过 GetModuleHandle、GetProcessAddress 从 ntdll.dll 中获取地址
        NtQueryInformationProcess = (pfnNtQueryInformationProcess)::GetProcAddress(
            ::GetModuleHandle(L"ntdll.dll"), "NtQueryInformationProcess");
        if (NtQueryInformationProcess == nullptr)
            break;

        // 获取指定进程的基本信息
        NTSTATUS status = NtQueryInformationProcess(hProcess, ProcessBasicInformation, &pbi, sizeof(pbi), nullptr);
        if (!NT_SUCCESS(status))
            break;

        PEB peb = { 0 };
        RTL_USER_PROCESS_PARAMETERS Param = { 0 };

        /*
        注意在读写其他进程的时候，注意要使用ReadProcessMemory/WriteProcessMemory进行操作，
        每个指针指向的内容都需要获取，因为指针只能指向本进程的地址空间，必须要读取到本进程空间。
        要不然一直提示位置访问错误!
        */
        ::ReadProcessMemory(hProcess, pbi.PebBaseAddress, &peb, sizeof(peb), nullptr);
        ::ReadProcessMemory(hProcess, peb.ProcessParameters, &Param, sizeof(Param), nullptr);

        if (command_line) {
            USHORT usCmdLen = (::wcslen(command_line) + 1) * sizeof(wchar_t);
            ::WriteProcessMemory(hProcess, Param.CommandLine.Buffer, command_line, usCmdLen, nullptr);
            ::WriteProcessMemory(hProcess, &Param.CommandLine.Length, &usCmdLen, sizeof(usCmdLen), nullptr);
        }

        if (image_path) {
            USHORT usPathLen = (::wcslen(image_path) + 1) * sizeof(wchar_t);
            ::WriteProcessMemory(hProcess, Param.ImagePathName.Buffer, image_path, usPathLen, nullptr);
            ::WriteProcessMemory(hProcess, &Param.ImagePathName.Length, &usPathLen, sizeof(usPathLen), nullptr);
        }
    } while (0);

    ::CloseHandle(hProcess);

    return true;
}