#ifndef DISGUISE_PROCESS_H_
#define DISGUISE_PROCESS_H_

#include <windows.h>

/**
 * ���ܣ�α�����·��������������
 * ������process_id����Ҫα��Ľ���id����ǰ����ʹ��::GetCurrentProcessId()
 *      image_path����Ҫα��Ľ���·��������Ҫ��null
 *      command_line����Ҫα��������в���������Ҫ��null
 * ����ֵ���ɹ�����true��ʧ�ܷ���false
 */
bool DisguiseProcess(const DWORD process_id, const wchar_t* image_path, const wchar_t* command_line);

#endif // DISGUISE_PROCESS_H_