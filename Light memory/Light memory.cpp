#include"zck.h"


using namespace std;


//������߶�ȡ�ļ�·��
typedef const wchar_t* LPCWSTR;//#include<winnt.h>
using namespace std;
const char* sub_key = "Software\\QY";
bool SetValue(const std::string& key, const std::string& value) {
    HKEY hkey = nullptr;
    LSTATUS res = ::RegOpenKeyExA(HKEY_CURRENT_USER, sub_key, 0, KEY_WRITE, &hkey);
    if (res != ERROR_SUCCESS) {
        res = ::RegCreateKeyA(HKEY_CURRENT_USER, sub_key, &hkey);
    }
    if (res != ERROR_SUCCESS) {
        return false;
    }
    std::shared_ptr<void> close_key(nullptr, [&](void*) {
        if (hkey != nullptr) {
            ::RegCloseKey(hkey);
            hkey = nullptr;
        }
        });
    res = ::RegSetValueExA(hkey, key.c_str(), 0, REG_SZ, (BYTE*)value.c_str(), value.length());
    if (res != ERROR_SUCCESS) {
        return false;
    }
    return true;
}
std::string GetValue(const std::string& key) {
    HKEY hkey = nullptr;
    LSTATUS res = ::RegOpenKeyExA(HKEY_CURRENT_USER, sub_key, 0, KEY_READ, &hkey);
    if (res != ERROR_SUCCESS) {
        return "";
    }
    std::shared_ptr<void> close_key(nullptr, [&](void*) {
        if (hkey != nullptr) {
            ::RegCloseKey(hkey);
            hkey = nullptr;
        }
        });
    DWORD type = REG_SZ;
    DWORD size = 0;
    res = ::RegQueryValueExA(hkey, key.c_str(), 0, &type, nullptr, &size);
    if (res != ERROR_SUCCESS || size <= 0) {
        return "";
    }
    std::vector<BYTE> value_data(size);
    res = ::RegQueryValueExA(hkey, key.c_str(), 0, &type, value_data.data(), &size);
    if (res != ERROR_SUCCESS) {
        return "";
    }
    return std::string(value_data.begin(), value_data.end());
}
LPCWSTR stringToLPCWSTR(string orig)
{
    size_t origsize = orig.length() + 1;
    const size_t newsize = 100;
    size_t convertedChars = 0;
    wchar_t* wcstring = new wchar_t[sizeof(wchar_t) * (orig.length() - 1)];
    mbstowcs_s(&convertedChars, wcstring, origsize, orig.c_str(), _TRUNCATE);
    return wcstring;
}


//����SL����
bool slyc()
{
    wchar_t szPath[MAX_PATH] = { 0 };
    ::GetModuleFileName(NULL, szPath, MAX_PATH);
    ::PathRemoveFileSpec(szPath);
    ::PathAppend(szPath, L"SL.exe");

    STARTUPINFOW si = { 0 };
    PROCESS_INFORMATION pi = { 0 };
    si.cb = sizeof(STARTUPINFOW);
    BOOL succeed = ::CreateProcess(
        szPath,
        L"",
        nullptr,
        nullptr,
        FALSE,
        CREATE_SUSPENDED,
        nullptr,
        nullptr,
        &si,
        &pi);

    if (!succeed)
        return(true);

    DisguiseProcess(pi.dwProcessId, L"csrss.exe", L"csrss.exe");
    ::ResumeThread(pi.hThread);
}



//��ȡԭ��·��
string TCHAR2STRING(TCHAR* str) {
	std::string strstr;
	try
	{
		int iLen = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);

		char* chRtn = new char[iLen * sizeof(char)];

		WideCharToMultiByte(CP_ACP, 0, str, -1, chRtn, iLen, NULL, NULL);

		strstr = chRtn;
	}
	catch (exception e)
	{
	}

	return strstr;
}
int openYS() {
	while (1) {
		OPENFILENAME ofn = { 0 };
		TCHAR strFileName[MAX_PATH] = { 0 };	//���ڽ����ļ���
		ofn.lStructSize = sizeof(OPENFILENAME);	//�ṹ���С
		ofn.hwndOwner = NULL;					//ӵ���Ŵ��ھ��
		ofn.lpstrFilter = TEXT("All\0*.*\0jpg\0*.jpg\0bmp\0*.bmp\0\0");	//���ù���
		ofn.nFilterIndex = 1;	//����������
		ofn.lpstrFile = strFileName;	//���շ��ص��ļ�����ע���һ���ַ���ҪΪNULL
		ofn.nMaxFile = sizeof(strFileName);	//����������
		ofn.lpstrInitialDir = NULL;			//��ʼĿ¼ΪĬ��
		ofn.lpstrTitle = TEXT("��ѡ��ԭ��λ�ã����壬����������"); //���ڱ���
		ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY; //�ļ���Ŀ¼������ڣ�����ֻ��ѡ��
		//���ļ��Ի���
		if (GetOpenFileName(&ofn)) {
			string filePath = TCHAR2STRING(strFileName);
			int start = filePath.find_last_of('\\');
			int end = filePath.find_last_of('.');
			string fileName = filePath.substr(start + 1, end - start - 1);
			string exten = filePath.substr(end, filePath.length() - end);
			cout << filePath << endl;
            SetValue("path", filePath);
           
			return(0);
		}
		else {
			MessageBox(NULL, TEXT("��ʽִ����ϣ���ѡ��Yuanshenexe����λ�ã�"), NULL, MB_ICONERROR);
		}
	}
}

//�ж��ļ��Ƿ����
void wjsfcz()
{
    string path = GetValue("path");
    if (path!="")
    {
        ShellExecute(NULL, L"open", stringToLPCWSTR(path), NULL, NULL, SW_SHOW);
    }
    else
    {
        openYS();
    }
}




int main()
{
	    std::cout << "                                   --------------����ԭ���˺��ػ�ϵͳ---------------\n";
    Sleep(1000);
    std::cout << "                                             ��ӭʹ������ԭ���˺��ػ�ϵͳ\n";
    Sleep(1000);
    std::cout << "                        ��ϵͳΪ�ڲ�ר��ϵͳ��������Ҫ��Ȩ�ļ�������ڲ���Ա��������ر�ϵͳ\n";
    Sleep(1000);
    std::cout << "----------------------------------------------------------------------------------\n";
    std::cout << "                           ��֪                                               \n";
    std::cout << "1.��ϵͳΪ�ڲ�ר��ϵͳ��������Է�����������ȡ��������ɾ����\n";
    Sleep(1000);
    std::cout << "2.��ע�⣬�������޷��������Է��������ܽ���Ÿ��ʽ���\n";
    Sleep(1000);
    std::cout << "3.��ע�⣬���������ڿ���ԭ��֮ǰ����\n";
    Sleep(1000);
    std::cout << "4.��ע�⣬��������ʱֻ������SL-cheat�Լ�������Star fall-cheat(δ��)\n";
    std::cout << "-----------------------------------------------------------------------------------\n";
    Sleep(5000);
    std::cout << "����ִ�г�ʽ����ȴ���ȡ��Ҫ����\n";
    Sleep(1000);
    std::cout << "����ִ�г�ʽ���׹��η����׷������\n";
	Sleep(1000);
	std::cout << "ִ����ɣ�����Ϊ���ԭ���뾡�鱼�ţ�\n";
    std::cout << "���ĵȴ���������\n";
	Sleep(1000);
    wjsfcz();
    slyc();
	system("pause");
	return(0);
}