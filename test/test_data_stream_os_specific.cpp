#include "test_data_stream.h"

#if defined(_WIN32)
#include <windows.h>
#elif defined(__APPLE__)
#include <mach-o/getsect.h>
#include <mach-o/loader.h>
#include "resources.h"
#if defined(__LP64__)
extern const mach_header_64 _mh_execute_header;
#else
extern const mach_header _mh_execute_header;
#endif
#elif defined(__linux__)
#include "resources.h"
#endif

const char* TestDataIStream::LoadResourceData(const std::string& resourceName, std::streamsize& dataSize)
{
#if defined(_WIN32)
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
#elif defined(__linux__)
    if (g_binaryDataMap.count(resourceName))
    {
        const auto &buf = g_binaryDataMap.at(resourceName);
        dataSize = buf.end - buf.start;
        return buf.start;
    }

    return nullptr;
#elif defined(__APPLE__)
    const auto it = g_binarySectionMap.find(resourceName);
    if (it == g_binarySectionMap.end())
    {
        return nullptr;
    }

    unsigned long size = 0;
    const auto* data = getsectiondata(&_mh_execute_header, "__YUVTEST", it->second.c_str(), &size);
    if (!data)
    {
        return nullptr;
    }

    dataSize = static_cast<std::streamsize>(size);
    return reinterpret_cast<const char*>(data);
#else
    return nullptr;
#endif
}
