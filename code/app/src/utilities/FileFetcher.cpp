#include "utilities/FileFetcher.hpp"
#include "Windows.h"
#include "commdlg.h"

namespace NRenderer
{
    using namespace std;
    optional<string> FileFetcher::fetch(const char* filter) const
    {
        OPENFILENAME ofn;              // common dialog box structure
        TCHAR szFile[260];             // buffer for file name
        HWND hwnd = GetActiveWindow(); // owner window
        HANDLE hf;                     // file handle

        // Initialize OPENFILENAME
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = hwnd;
        ofn.lpstrFile = szFile;
        // Set lpstrFile[0] to '\0' so that GetOpenFileName does not
        // use the contents of szFile to initialize itself.
        ofn.lpstrFile[0] = '\0';
        ofn.nMaxFile = sizeof(szFile);
        // "All(*.obj *.scn)\0 *.obj;*.scn\0.obj file\0*.obj\0custom scene file\0*.scn\0"
        ofn.lpstrFilter = (filter);
        // ofn.lpstrFilter = "All(*.obj *.scn)\0 *.obj;*.scn\0.obj file\0*.obj\0custom scene file\0*.scn\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = NULL;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        if (GetOpenFileName(&ofn) == TRUE)
        {
            string filePath{(char *)ofn.lpstrFile};
            return filePath;
        }
        else {
            return nullopt;
        }
    }
}