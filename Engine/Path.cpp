#include "Engine/pch.h"

#include "Shlwapi.h"

#include "Engine/Path.h"


namespace engine
{


std::wstring Path::getExecutable()
{
    std::wstring result;

    {
        wchar_t buffer[MAX_PATH + 2];
        DWORD length = GetModuleFileNameW(NULL, buffer, MAX_PATH + 1);

        if (length == 0) {
            throw std::runtime_error("Can't get module file name.");
        }

        if (length <= MAX_PATH) {
            buffer[MAX_PATH + 1] = 0;
            result = buffer;
        }
    }

    if (result.empty()) {
        // MAX_PATH sized buffer wasn't large enough to contain the full path, use heap
        wchar_t* buffer = 0;
        int i = 1;
        size_t size;
        DWORD length;

        do {
            ++i;
            size = MAX_PATH * i;
            buffer = reinterpret_cast<wchar_t*>(realloc(buffer, (size + 1) * sizeof(wchar_t)));
            if (buffer != NULL) {
                length = GetModuleFileNameW(NULL, buffer, DWORD(size));
            }
            else {
                throw std::bad_alloc();
            }
        } while (buffer != NULL && length == size);

        *(buffer + size) = 0;

        result = buffer;
        free(buffer);
    }

    return result;
}


std::wstring Path::getExecutableDirPath()
{
    std::wstring executablePath = getExecutable();
    PathRemoveFileSpec(const_cast<wchar_t*>(executablePath.c_str()));

    return executablePath.c_str();
}


}//namespace engine
