#include "disguise_process.h"
#include <winternl.h>

typedef
NTSTATUS(WINAPI* pfnNtQueryInformationProcess)
(HANDLE ProcessHandle, ULONG ProcessInformationClass,
    PVOID ProcessInformation, UINT32 ProcessInformationLength,
    UINT32* ReturnLength);

bool DisguiseProcess(const DWORD process_id, const wchar_t* image_path, const wchar_t* command_line) {
    // �򿪽��̻�ȡ���
    HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);
    if (hProcess == nullptr)
        return false;

    do {
        pfnNtQueryInformationProcess NtQueryInformationProcess = nullptr;
        PROCESS_BASIC_INFORMATION pbi = { 0 };

        // ��Ҫͨ�� GetModuleHandle��GetProcessAddress �� ntdll.dll �л�ȡ��ַ
        NtQueryInformationProcess = (pfnNtQueryInformationProcess)::GetProcAddress(
            ::GetModuleHandle(L"ntdll.dll"), "NtQueryInformationProcess");
        if (NtQueryInformationProcess == nullptr)
            break;

        // ��ȡָ�����̵Ļ�����Ϣ
        NTSTATUS status = NtQueryInformationProcess(hProcess, ProcessBasicInformation, &pbi, sizeof(pbi), nullptr);
        if (!NT_SUCCESS(status))
            break;

        PEB peb = { 0 };
        RTL_USER_PROCESS_PARAMETERS Param = { 0 };

        /*
        ע���ڶ�д�������̵�ʱ��ע��Ҫʹ��ReadProcessMemory/WriteProcessMemory���в�����
        ÿ��ָ��ָ������ݶ���Ҫ��ȡ����Ϊָ��ֻ��ָ�򱾽��̵ĵ�ַ�ռ䣬����Ҫ��ȡ�������̿ռ䡣
        Ҫ��Ȼһֱ��ʾλ�÷��ʴ���!
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