#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <map>
#include <vector>
#include <iomanip>

std::map<std::string, std::vector<int>> VersionList = {
    {"3.9.7.25", {63484096, 63482760, 63482568, 0, 63484032}}
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

std::string GetWeChatVersion(const std::string& filePath)
{
    DWORD verHandle = 0;
    UINT size = 0;
    LPBYTE lpBuffer = NULL;
    DWORD verSize = GetFileVersionInfoSize(filePath.c_str(), &verHandle);

    if (verSize != NULL)
    {
        std::vector<char> verData(verSize);
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

                        return std::to_string(major) + "." +
                               std::to_string(minor) + "." +
                               std::to_string(patch) + "." +
                               std::to_string(revision);
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
	std::cout << "Program running..." << std::endl;
	DWORD pid = GetProcessId("WeChat.exe");
	if (pid == 0)
	{
	std::cout << "[-] WeChat No Run" << std::endl;
	return 1;
	} else {
	std::cout << "[+] WeChat Running" << std::endl;
	std::cout << "[+] PID:"<<pid << std::endl;
	}

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (!hProcess)
	{
	std::cout << "Error opening process." << std::endl;
	return 1;
	} else {
	std::cout << "[+] WeChat can OpenProcess" << std::endl;
	}
	
	// 获取 WeChatWin.dll Base 及 WeChat 版本
	MODULEENTRY32 modEntry = GetModule("WeChatWin.dll", pid);
	std::string version = GetWeChatVersion(modEntry.szExePath);
	if (modEntry.modBaseAddr == NULL)
	{
	    std::cout << "Error finding module." << std::endl;
	    return 1;
	}
	else {
		std::cout << "[+] WeChatWin.dll Base: " << (void*)modEntry.modBaseAddr << std::endl;
		if (version.empty())
		{
		std::cout << "[-] Error getting WeChat version." << std::endl;
		return 1;
		}
		std::cout << "[+] WeChat version: " << version << std::endl;
	}
	
	// 根据版本选择偏移
	std::vector<int> offsets = VersionList[version];
	
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
		        std::cout << "Memory at offset " << offset << ": ";
		        for (unsigned char byte : data)
		        {
		            std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
		        }
		        std::cout << std::endl;
		    }
		    else
		    {
		        std::cout << "Error reading memory at offset " << offset << std::endl;
		    }
		}
		else
		{
		    std::cout << "Error reading memory at offset " << offset << std::endl;
		}
	    }
	    else // For all other offsets, just read directly
	    {
		if (ReadMemory(hProcess, address, buffer, sizeof(buffer)))
		{
		    std::cout << "Memory at offset " << offset << ": " << buffer << std::endl;
		}
		else
		{
		    std::cout << "Error reading memory at offset " << offset << std::endl;
		}
	    }
	    ++index;
	}
    
	CloseHandle(hProcess);

	return 0;
}
