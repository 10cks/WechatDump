#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <map>
#include <vector>
#include <iomanip>

using namespace std;

map<string, vector<int>> VersionList = {
	{"3.2.1.154", {328121948, 328122328, 328123056, 328121976, 328123020}},
	{"3.3.0.115", {31323364, 31323744, 31324472, 31323392, 31324436}},
	{"3.3.0.84", {31315212, 31315592, 31316320, 31315240, 31316284}},
	{"3.3.0.93", {31323364, 31323744, 31324472, 31323392, 31324436}},
	{"3.3.5.34", {30603028, 30603408, 30604120, 30603056, 30604100}},
	{"3.3.5.42", {30603012, 30603392, 30604120, 30603040, 30604084}},
	{"3.3.5.46", {30578372, 30578752, 30579480, 30578400, 30579444}},
	{"3.4.0.37", {31608116, 31608496, 31609224, 31608144, 31609188}},
	{"3.4.0.38", {31604044, 31604424, 31605152, 31604072, 31605116}},
	{"3.4.0.50", {31688500, 31688880, 31689608, 31688528, 31689572}},
	{"3.4.0.54", {31700852, 31701248, 31700920, 31700880, 31701924}},
	{"3.4.5.27", {32133788, 32134168, 32134896, 32133816, 32134860}},
	{"3.4.5.45", {32147012, 32147392, 32147064, 32147040, 32148084}},
	{"3.5.0.20", {35494484, 35494864, 35494536, 35494512, 35495556}},
	{"3.5.0.29", {35507980, 35508360, 35508032, 35508008, 35509052}},
	{"3.5.0.33", {35512140, 35512520, 35512192, 35512168, 35513212}},
	{"3.5.0.39", {35516236, 35516616, 35516288, 35516264, 35517308}},
	{"3.5.0.42", {35512140, 35512520, 35512192, 35512168, 35513212}},
	{"3.5.0.44", {35510836, 35511216, 35510896, 35510864, 35511908}},
	{"3.5.0.46", {35506740, 35507120, 35506800, 35506768, 35507812}},
	{"3.6.0.18", {35842996, 35843376, 35843048, 35843024, 35844068}},
	{"3.6.5.7", {35864356, 35864736, 35864408, 35864384, 35865428}},
	{"3.6.5.16", {35909428, 35909808, 35909480, 35909456, 35910500}},
	{"3.7.0.26", {37105908, 37106288, 37105960, 37105936, 37106980}},
	{"3.7.0.29", {37105908, 37106288, 37105960, 37105936, 37106980}},
	{"3.7.0.30", {37118196, 37118576, 37118248, 37118224, 37119268}},
	{"3.7.5.11", {37883280, 37884088, 37883136, 37883008, 37884052}},
	{"3.7.5.23", {37895736, 37896544, 37895592, 37883008, 37896508}},
	{"3.7.5.27", {37895736, 37896544, 37895592, 37895464, 37896508}},
	{"3.7.5.31", {37903928, 37904736, 37903784, 37903656, 37904700}},
	{"3.7.6.24", {38978840, 38979648, 38978696, 38978604, 38979612}},
	{"3.7.6.29", {38986376, 38987184, 38986232, 38986104, 38987148}},
	{"3.7.6.44", {39016520, 39017328, 39016376, 38986104, 39017292}},
	{"3.8.0.31", {46064088, 46064912, 46063944, 38986104, 46064876}},
	{"3.8.0.33", {46059992, 46060816, 46059848, 38986104, 46060780}},
	{"3.8.0.41", {46064024, 46064848, 46063880, 38986104, 46064812}},
	{"3.8.1.26", {46409448, 46410272, 46409304, 38986104, 46410236}},
	{"3.9.0.28", {48418376, 48419280, 48418232, 38986104, 48419244}},
	{"3.9.2.23", {50320784, 50321712, 50320640, 38986104, 50321676}},
	{"3.9.2.26", {50329040, 50329968, 50328896, 38986104, 50329932}},
	{"3.9.5.81", {61650872, 61652208, 61650680, 0, 61652144}},
	{"3.9.5.91", {61654904, 61654680, 61654712, 38986104, 61656176}},
	{"3.9.6.19", {61997688, 61997464, 61997496, 38986104, 61998960}},
	{"3.9.6.33", {62030600, 62031936, 62030408, 0, 62031872}},
	{"3.9.7.25", {63482760, 63484096, 63482568, 0, 63484032}},
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
