#include "test_data_stream.h"

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <mach-o/getsect.h>
#include <mach-o/loader.h>
#include "resources.h"
#if defined(__LP64__)
extern const mach_header_64 _mh_execute_header;
#else
extern const mach_header _mh_execute_header;
#endif
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

#ifdef __APPLE__
    const auto it = g_binarySectionMap.find(resourceName);
    if (it == g_binarySectionMap.end())
    {
        return nullptr;
    }

    unsigned long size = 0;
#if defined(__LP64__)
    const auto* data = getsectiondata(&_mh_execute_header, "__YUVTEST", it->second.c_str(), &size);
#else
    const auto* data = getsectiondata(&_mh_execute_header, "__YUVTEST", it->second.c_str(), &size);
#endif
    if (!data)
    {
        return nullptr;
    }

    dataSize = static_cast<std::streamsize>(size);
    return reinterpret_cast<const char*>(data);
#endif

    return nullptr;
}
