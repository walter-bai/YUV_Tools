#include "test_data_stream.h"

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __linux__
#include "resources.h"
#endif

const char* TestDataIStream::LoadResourceData(const std::string& resourceName, std::streamsize& dataSize)
{
#ifdef _WIN32
    HMODULE handle = GetModuleHandle(NULL);
    HRSRC res = FindResource(handle, resourceName.c_str(), RT_RCDATA);
    if (!res)
    {
        return nullptr;
    }

    HGLOBAL resHandle = LoadResource(handle, res);
    if (!resHandle)
    {
        return nullptr;
    }

    dataSize = SizeofResource(handle, res);
    return static_cast<const char*>(LockResource(resHandle));
#endif

#ifdef __linux__
    if (g_binaryDataMap.count(resourceName))
    {
        const auto &buf = g_binaryDataMap.at(resourceName);
        dataSize = buf.end - buf.start;
        return buf.start;
    }

    return nullptr;
#endif
}
