#include <iostream>
#include <Windows.h>
#include <Urlmon.h>
#pragma comment(lib, "urlmon.lib")

int main()
{
    // Find the path to the %APPDATA% folder
    char* appDataPath;
    size_t len;
    _dupenv_s(&appDataPath, &len, "APPDATA");

    // Check if the BetterDiscord folder exists in the %APPDATA% folder
    std::string bdFolderPath = std::string(appDataPath) + "\\BetterDiscord";
    DWORD bdFolderAttrib = GetFileAttributesA(bdFolderPath.c_str());
    if (bdFolderAttrib == INVALID_FILE_ATTRIBUTES || !(bdFolderAttrib & FILE_ATTRIBUTE_DIRECTORY))
    {
        std::cout << "BetterDiscord folder not found in %APPDATA%. Update cancelled." << std::endl;
        return 0;
    }

    // Check for the presence of plugins.json in any of the subfolders
    std::string pluginsJsonPath;
    std::string pluginsJsonFolder;
    DWORD pluginsJsonAttrib;
    if (GetFileAttributesA((bdFolderPath + "\\data\\ptb\\plugins.json").c_str()) != INVALID_FILE_ATTRIBUTES)
    {
        pluginsJsonPath = bdFolderPath + "\\data\\ptb\\plugins.json";
        pluginsJsonFolder = "Discord PTB";
        pluginsJsonAttrib = GetFileAttributesA((bdFolderPath + "\\data\\ptb").c_str());
    }
    else if (GetFileAttributesA((bdFolderPath + "\\data\\canary\\plugins.json").c_str()) != INVALID_FILE_ATTRIBUTES)
    {
        pluginsJsonPath = bdFolderPath + "\\data\\canary\\plugins.json";
        pluginsJsonFolder = "Discord Canary";
        pluginsJsonAttrib = GetFileAttributesA((bdFolderPath + "\\data\\canary").c_str());
    }
    else if (GetFileAttributesA((bdFolderPath + "\\data\\stable\\plugins.json").c_str()) != INVALID_FILE_ATTRIBUTES)
    {
        pluginsJsonPath = bdFolderPath + "\\data\\stable\\plugins.json";
        pluginsJsonFolder = "Discord";
        pluginsJsonAttrib = GetFileAttributesA((bdFolderPath + "\\data\\stable").c_str());
    }
    else
    {
        std::cout << "No plugins.json file found in BetterDiscord subfolders. Update cancelled." << std::endl;
		system("pause");
        return 0;
    }

    // Ask user if they want to continue with update
    std::cout << pluginsJsonFolder << " plugins.json file found. Do you want to continue with the update? (y/n)" << std::endl;
	//  change std color to red only for this text
	std::cout << "\033[31m";
    std::cout << "Note: This will kill any active Discord process running and also disable any plugins you have enabled." << std::endl;
	std::cout << "\033[0m";
    char input;
    std::cin >> input;
    if (input != 'y')
    {
        std::cout << "Update cancelled by user." << std::endl;
		system("pause");
        return 0;
    }

    // Get the process ID of the Discord process
    DWORD dwProcessId = 0;
    HWND hwnd = FindWindowA(NULL, "Discord");
    if (hwnd != NULL)
    {
        GetWindowThreadProcessId(hwnd, &dwProcessId);
    }
    else if (pluginsJsonFolder == "Discord PTB")
    {
        hwnd = FindWindowA(NULL, "Discord PTB");
        if (hwnd != NULL)
        {
            GetWindowThreadProcessId(hwnd, &dwProcessId);
        }
    }
    else if (pluginsJsonFolder == "Discord Canary")
    {
        hwnd = FindWindowA(NULL, "Discord Canary");
        if (hwnd != NULL)
        {
            GetWindowThreadProcessId(hwnd, &dwProcessId);
        }
    }
	
	// Terminate the discordptb, discordcanary, or discord process if running
	if (dwProcessId != 0)
	{
		HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessId);
		if (hProcess != NULL)
		{
			TerminateProcess(hProcess, 0);
			CloseHandle(hProcess);
		}
	}



    // Delete old betterdiscord.asar file
    std::string bdAsarPath = bdFolderPath + "\\data\\betterdiscord.asar";
    DeleteFileA(bdAsarPath.c_str());

    // Download new betterdiscord.asar file from Discord
    std::string downloadUrl = "https://cdn.discordapp.com/attachments/826217269323235358/1079920833662816277/betterdiscord.asar";
    HRESULT hr = URLDownloadToFileA(NULL, downloadUrl.c_str(), bdAsarPath.c_str(), 0, NULL);
    if (hr != S_OK)
    {
        std::cout << "Error downloading new betterdiscord.asar file. Update cancelled." << std::endl;
        return 0;
    }

    // Delete old plugins.json file
    DeleteFileA(pluginsJsonPath.c_str());

    // Update complete
	std::cout << "Update complete. Restart " << pluginsJsonFolder << " to apply changes." << std::endl;
	return 0;
}
