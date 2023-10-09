#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <map>
#include <vector>
#include <iomanip>

using namespace std;

map<string, vector<int>> VersionList = {
    {"3.9.7.25", {63484096, 63482760, 63482568, 0, 63484032}},
	{"3.9.7.29", {63486984, 63488320, 63486792, 0, 63488256}},
};

DWORD GetProcessId(const char* processName)
{
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32 pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32);
        
        if (Process32First(hSnap, &pe32))
        {
            do {
                if (_stricmp(processName, pe32.szExeFile) == 0)
                {
                    CloseHandle(hSnap);
                    return pe32.th32ProcessID;
                }
            } while (Process32Next(hSnap, &pe32));
        }
        
        CloseHandle(hSnap);
    }
    
    return 0;
}

MODULEENTRY32 GetModule(const char* moduleName, DWORD processId)
{
    MODULEENTRY32 modEntry = { 0 };
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);
    if (hSnap != INVALID_HANDLE_VALUE)
    {
        modEntry.dwSize = sizeof(MODULEENTRY32);
        if (Module32First(hSnap, &modEntry))
        {
            do {
                if (_stricmp(modEntry.szModule, moduleName) == 0)
                {
                    CloseHandle(hSnap);
                    return modEntry;
                }
            } while (Module32Next(hSnap, &modEntry));
        }
        CloseHandle(hSnap);
    }
    return modEntry;
}

string GetWeChatVersion(const string& filePath)
{
    DWORD verHandle = 0;
    UINT size = 0;
    LPBYTE lpBuffer = NULL;
    DWORD verSize = GetFileVersionInfoSize(filePath.c_str(), &verHandle);

    if (verSize != NULL)
    {
        vector<char> verData(verSize);
        if (GetFileVersionInfo(filePath.c_str(), verHandle, verSize, verData.data()))
        {
            if (VerQueryValue(verData.data(), "\\", (VOID FAR* FAR*)&lpBuffer, &size))
            {
                if (size)
                {
                    VS_FIXEDFILEINFO* verInfo = (VS_FIXEDFILEINFO*)lpBuffer;
                    if (verInfo->dwSignature == 0xfeef04bd)
                    {
                        int major = (verInfo->dwFileVersionMS >> 16) & 0xffff;
                        int minor = (verInfo->dwFileVersionMS >> 0) & 0xffff;
                        int patch = (verInfo->dwFileVersionLS >> 16) & 0xffff;
                        int revision = (verInfo->dwFileVersionLS >> 0) & 0xffff;

                        return to_string(major) + "." +
                               to_string(minor) + "." +
                               to_string(patch) + "." +
                               to_string(revision);
                    }
                }
            }
        }
    }

    return "";
}

bool ReadMemory(HANDLE hProcess, LPVOID baseAddress, LPVOID buffer, SIZE_T size)
{
    SIZE_T bytesRead;
    return ReadProcessMemory(hProcess, baseAddress, buffer, size, &bytesRead);
}

int main()
{
	cout << "Program running..." << endl;
	DWORD pid = GetProcessId("WeChat.exe");
	if (pid == 0)
	{
	cout << "[-] WeChat No Run" << endl;
	return 1;
	} else {
	cout << "[+] WeChat Running" << endl;
	cout << "[+] PID:"<<pid << endl;
	}

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (!hProcess)
	{
	cout << "Error opening process." << endl;
	return 1;
	} else {
	cout << "[+] WeChat can OpenProcess" << endl;
	}
	
	// 获取 WeChatWin.dll Base 及 WeChat 版本
	MODULEENTRY32 modEntry = GetModule("WeChatWin.dll", pid);
	string version = GetWeChatVersion(modEntry.szExePath);
	if (modEntry.modBaseAddr == NULL)
	{
	    cout << "Error finding module." << endl;
	    return 1;
	}
	else {
		cout << "[+] WeChatWin.dll Base: " << (void*)modEntry.modBaseAddr << endl;
		if (version.empty())
		{
		cout << "[-] Error getting WeChat version." << endl;
		return 1;
		}
		cout << "[+] WeChat version: " << version << endl;
	}
	
	// 根据版本选择偏移
	vector<int> offsets = VersionList[version];
	
	// 打印出每个偏移的内存内容
	char buffer[100];
	int index = 0;
	for (int offset : offsets)
	{
	    LPVOID address = (LPBYTE)modEntry.modBaseAddr + offset;
	    if (index == 4) // 第五个参数为key
	    {
		unsigned long long nextAddress;
		if (ReadMemory(hProcess, address, &nextAddress, sizeof(nextAddress)))
		{
		    unsigned char data[32];
		    if (ReadMemory(hProcess, (LPVOID)nextAddress, &data, sizeof(data)))
		    {
		        cout << "Memory at offset " << offset << ": ";
		        for (unsigned char byte : data)
		        {
		            cout << hex << setw(2) << setfill('0') << (int)byte;
		        }
		        cout << endl;
		    }
		    else
		    {
		        cout << "Error reading memory at offset " << offset << endl;
		    }
		}
		else
		{
		    cout << "Error reading memory at offset " << offset << endl;
		}
	    }
	    else // For all other offsets, just read directly
	    {
		if (ReadMemory(hProcess, address, buffer, sizeof(buffer)))
		{
		    cout << "Memory at offset " << offset << ": " << buffer << endl;
		}
		else
		{
		    cout << "Error reading memory at offset " << offset << endl;
		}
	    }
	    ++index;
	}
    
	CloseHandle(hProcess);

	return 0;
}
