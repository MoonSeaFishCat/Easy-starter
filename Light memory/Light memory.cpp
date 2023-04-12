#include"zck.h"


using namespace std;


//保存或者读取文件路径
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


//隐藏SL进程
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



//获取原神路径
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
		TCHAR strFileName[MAX_PATH] = { 0 };	//用于接收文件名
		ofn.lStructSize = sizeof(OPENFILENAME);	//结构体大小
		ofn.hwndOwner = NULL;					//拥有着窗口句柄
		ofn.lpstrFilter = TEXT("All\0*.*\0jpg\0*.jpg\0bmp\0*.bmp\0\0");	//设置过滤
		ofn.nFilterIndex = 1;	//过滤器索引
		ofn.lpstrFile = strFileName;	//接收返回的文件名，注意第一个字符需要为NULL
		ofn.nMaxFile = sizeof(strFileName);	//缓冲区长度
		ofn.lpstrInitialDir = NULL;			//初始目录为默认
		ofn.lpstrTitle = TEXT("请选择原神位置（本体，非启动器）"); //窗口标题
		ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY; //文件、目录必须存在，隐藏只读选项
		//打开文件对话框
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
			MessageBox(NULL, TEXT("程式执行完毕，请选择Yuanshenexe所在位置！"), NULL, MB_ICONERROR);
		}
	}
}

//判断文件是否存在
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
	    std::cout << "                                   --------------轻忆原神账号守护系统---------------\n";
    Sleep(1000);
    std::cout << "                                             欢迎使用轻忆原神账号守护系统\n";
    Sleep(1000);
    std::cout << "                        本系统为内部专用系统，运行需要授权文件，如非内部人员，请立马关闭系统\n";
    Sleep(1000);
    std::cout << "----------------------------------------------------------------------------------\n";
    std::cout << "                           须知                                               \n";
    std::cout << "1.本系统为内部专用系统，如果你以非正常渠道获取，请立马删除！\n";
    Sleep(1000);
    std::cout << "2.请注意，本工具无法做到绝对防护，仅能将封号概率降低\n";
    Sleep(1000);
    std::cout << "3.请注意，本工具需在开启原神之前运行\n";
    Sleep(1000);
    std::cout << "4.请注意，本工具暂时只能用于SL-cheat以及后续的Star fall-cheat(未出)\n";
    std::cout << "-----------------------------------------------------------------------------------\n";
    Sleep(5000);
    std::cout << "正在执行程式，请等待获取必要数据\n";
    Sleep(1000);
    std::cout << "正在执行程式，米哈游反作弊服务禁用\n";
	Sleep(1000);
	std::cout << "执行完成！正在为你打开原神，请尽情奔放！\n";
    std::cout << "耐心等待五秒左右\n";
	Sleep(1000);
    wjsfcz();
    slyc();
	system("pause");
	return(0);
}