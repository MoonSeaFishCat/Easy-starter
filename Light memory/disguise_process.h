#ifndef DISGUISE_PROCESS_H_
#define DISGUISE_PROCESS_H_

#include <windows.h>

/**
 * 功能：伪造进程路径，进程命令行
 * 参数：process_id：需要伪造的进程id，当前进程使用::GetCurrentProcessId()
 *      image_path：需要伪造的进程路径，不需要传null
 *      command_line：需要伪造的命令行参数，不需要传null
 * 返回值：成功返回true，失败返回false
 */
bool DisguiseProcess(const DWORD process_id, const wchar_t* image_path, const wchar_t* command_line);

#endif // DISGUISE_PROCESS_H_