#pragma once

#include <iomanip>
#include <map>
#include <memory>
#include <string>
#include <vector>

class TestDataIStream final
{
public:
    void open(const std::string& filename, std::ios_base::openmode mode);
    TestDataIStream& read(char* s, std::streamsize n);
    TestDataIStream& seekg(std::streampos pos);

    std::streamsize gcount() const
    {
        return m_gcount;
    }

    operator bool() const
    {
        return m_data.at(m_file).first != nullptr;
    }

private:
    std::streamsize GetFileSize(const std::string& filename) const;
    const char* LoadResourceData(const std::string& resourceName, std::streamsize& dataSize);

private:
    std::string m_file;
    std::map<std::string, std::pair<const char*, std::streamsize>> m_data;
    std::streamsize m_pos = 0;
    std::streamsize m_gcount = 0;
    std::unique_ptr<const char[]> m_buf;
};

class TestDataOStream final
{
public:
    void open(const std::string& filename, std::ios_base::openmode mode);
    TestDataOStream& write(const char* s, std::streamsize n);

    operator bool() const
    {
        return !m_file.empty();
    }

    static const std::vector<char>& Get()
    {
        return _data;
    }

private:
    static std::vector<char> _data;
    std::string m_file;
    std::streamsize m_pos = 0;
};
