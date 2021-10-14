#include <Windows.h>
#include <shobjidl_core.h>
#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

EXTERN_C const PROPERTYKEY DECLSPEC_SELECTANY PKEY_AppUserModel_ID =
{ { 0x9F4C2855, 0x9F79, 0x4B39,
{ 0xA8, 0xD0, 0xE1, 0xD4, 0x2D, 0xE1, 0xD5, 0xF3, } }, 5 };

void LaunchSpotify(LPWSTR launcher)
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    CreateProcess(NULL,
        launcher,
        NULL,
        NULL,
        FALSE,
        CREATE_NO_WINDOW,
        NULL,
        NULL,
        &si,
        &pi
    );

    WaitForSingleObject(pi.hProcess, 5000);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

bool safeDo(HRESULT hr, string message)
{
    if( !SUCCEEDED(hr) ) {
        ShowWindow(GetConsoleWindow(), SW_SHOW);
        wcout << "Error: " << message.c_str() << endl;
        return false;
    }
    return true;
}

bool setAppId(LPWSTR path, LPWSTR name)
{
    bool ok = true;
    if( ok ) ok &= safeDo(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED), "Failed to initialize COM");
    else return false;

    IShellLink* link;
    if( ok ) ok &= safeDo(CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&link)), "Failed to create ShellLink object");
    else return false;

    IPersistFile* file;
    if( ok ) ok &= safeDo(link->QueryInterface(IID_PPV_ARGS(&file)), "Failed to obtain PersistFile interface");
    else return false;
    if( ok ) ok &= safeDo(file->Load(path, STGM_READWRITE), "Failed to load shortcut file");
    else return false;

    IPropertyStore* store;
    if( ok ) ok &= safeDo(link->QueryInterface(IID_PPV_ARGS(&store)), "Failed to obtain PropertyStore interface");
    else return false;

    PROPVARIANT pv;
    if( ok ) ok &= safeDo(store->GetValue(PKEY_AppUserModel_ID, &pv), "Failed to retrieve AppId");
    else return false;

    bool idSet = false;
    wcout << path << endl;
    if( pv.vt != VT_EMPTY ) {
        if( pv.vt != VT_LPWSTR ) {
            wcout << "Type: " << pv.vt << endl;
            return false;
        }

        wcout << " -> Current AppId: " << pv.pwszVal << endl;
        idSet = !wcscmp(pv.pwszVal, name);
    }
    else {
        wcout << " -> No current AppId" << endl;
    }
    PropVariantClear(&pv);

    if( !idSet ) {
        pv.vt = VT_LPWSTR;
        pv.pwszVal = name;
        wcout << "New AppId: " << pv.pwszVal << endl;

        if( ok ) ok &= safeDo(store->SetValue(PKEY_AppUserModel_ID, pv), "Failed to set AppId");

        // Not sure if we need to do this
        pv.pwszVal = NULL;
        PropVariantClear(&pv);

        if( ok ) ok &= safeDo(store->Commit(), "Failed to commit AppId property");
        if( ok ) ok &= safeDo(file->Save(NULL, TRUE), "Failed to save shortcut");
    }

    store->Release();
    file->Release();
    link->Release();

    return ok;
}

int main(int argc, LPWSTR* argv)
{
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    wchar_t appName[MAX_PATH], noControlLnk[MAX_PATH], spotifyLnk[MAX_PATH], spotifyExe[MAX_PATH], appData[MAX_PATH];
    wcsncpy_s(appName, L"Spotify", MAX_PATH);
    GetEnvironmentVariable(L"APPDATA", appData, MAX_PATH);

    HWND stepChild = FindWindow(L"Chrome_WidgetWin_0", NULL);
    HWND parent = GetParent(stepChild);
    DWORD parentD = GetWindowThreadProcessId(parent, NULL);

    if (parentD == 0) {
        if( argc > 1 ) {
            wcout << "USAGE: SpotifyNoControl.exe [SpotifyNoControl.lnk] [Spotify.lnk] [spotify.exe]" << endl;
            wcout << "  - [SpotifyNoControl.lnk] is the path to your shortcut pointing to SpotifyNoControl" << endl;
            wcout << "    defaults to %appdata%\\Microsoft\\Internet Explorer\\Quick Launch\\User Pinned\\TaskBar\\SpotifyNoControl.lnk" << endl;
            wcout << "             or %appdata%\\Microsoft\\Windows\\Start Menu\\Programs\\SpotifyNoControl.lnk" << endl;
            wcout << "  - [Spotify.lnk] is the path to your shortcut pointing to Spotify" << endl;
            wcout << "    defaults to %appdata%\\Microsoft\\Windows\\Start Menu\\Programs\\Spotify.lnk" << endl;
            wcout << "  - [spotify.exe] is the path to Spotify executable" << endl;
            wcout << "    defaults to %appdata%\\Spotify\\spotify.exe" << endl;
        }

        if( argc > 1 ) {
            wcsncpy_s(noControlLnk, argv[1], MAX_PATH);
        }
        else {
            // Default to task bar shortcut
            lstrcpy(noControlLnk, appData);
            lstrcat(noControlLnk, L"\\Microsoft\\Internet Explorer\\Quick Launch\\User Pinned\\TaskBar\\SpotifyNoControl.lnk");
        }
        if( !setAppId(noControlLnk, appName) ) {
            // Retry with start menu shortcut
            lstrcpy(noControlLnk, appData);
            lstrcat(noControlLnk, L"\\Microsoft\\Windows\\Start Menu\\Programs\\SpotifyNoControl.lnk");
            setAppId(noControlLnk, appName);
        }

        if( argc > 2 ) {
            wcsncpy_s(spotifyLnk, argv[2], MAX_PATH);
        }
        else {
            lstrcpy(spotifyLnk, appData);
            lstrcat(spotifyLnk, L"\\Microsoft\\Windows\\Start Menu\\Programs\\Spotify.lnk");
        }
        setAppId(spotifyLnk, appName);

        if (argc > 3) {
            wcsncpy_s(appData, argv[3], MAX_PATH);
        } else {
            lstrcpy(spotifyExe, appData);
            lstrcat(spotifyExe, L"\\Spotify\\spotify.exe");
        }

        LaunchSpotify(spotifyExe);
    }

    while (parentD == 0) {
        stepChild = FindWindow(L"Chrome_WidgetWin_0", NULL);
        parent = GetParent(stepChild);
        parentD = GetWindowThreadProcessId(parent, NULL);
        Sleep(100);
    }

    HWND child1 = GetNextWindow(parent, GW_HWNDPREV);
    HWND child2 = GetNextWindow(child1, GW_HWNDPREV);
    SetWindowPos(child1, (HWND)1, 0, 0, 0, 0, SWP_NOOWNERZORDER);
    SetWindowPos(child2, (HWND)1, 0, 0, 0, 0, SWP_NOOWNERZORDER);
    
    return 0;
}