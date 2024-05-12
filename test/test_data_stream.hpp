#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "gtest/gtest.h"

class TestDataIStream final
{
public:
    void open(const std::string& filename, std::ios_base::openmode mode)
    {
        m_file = filename;
        m_pos = 0;
        m_gcount = 0;

        if (m_data.count(filename))
        {
            return;
        }

        std::ifstream ifs;
        ifs.open(filename, mode);
        EXPECT_EQ(ifs.is_open(), true);

        auto size = GetFileSize(filename);
        EXPECT_NE(size, -1);

        auto& buf = m_data[filename];
        buf.resize(size, 0);
        ifs.read(buf.data(), size);
    }

    TestDataIStream& read(char* s, std::streamsize n)
    {
        auto& buf = m_data[m_file];
        auto sz = std::min(n, static_cast<std::streamsize>(buf.size()) - m_pos);

        memcpy_s(s, sz, buf.data() + m_pos, sz);
        m_pos += sz;
        m_gcount = sz;

        return *this;
    }

    TestDataIStream& seekg(std::streampos pos)
    {
        EXPECT_EQ(m_data[m_file].size() > pos, true);

        m_pos = pos;

        return *this;
    }

    std::streamsize gcount() const
    {
        return m_gcount;
    }

    operator bool() const
    {
        return !m_data.at(m_file).empty();
    }

private:
    std::streamsize GetFileSize(const std::string& filename) const
    {
        std::ifstream file(filename, std::ifstream::ate | std::ifstream::binary);
        if (!file)
        {
            std::cerr << "Cannot open the file: " << filename << std::endl;
            return -1;
        }

        return file.tellg();
    }

private:
    std::string m_file;
    std::map<std::string, std::vector<char>> m_data;
    std::streamsize m_pos = 0;
    std::streamsize m_gcount = 0;
};

class TestDataOStream final
{
public:
    void open(const std::string& filename, std::ios_base::openmode mode)
    {
        std::ignore = mode;

        m_file = filename;
        m_data.clear();
        m_pos = 0;
    }

    TestDataOStream& write(const char* s, std::streamsize n)
    {
        if (m_data.size() < m_pos + n)
        {
            m_data.resize(m_pos + n);
        }

        memcpy_s(m_data.data() + m_pos, n, s, n);
        m_pos += n;

        return *this;
    }

    operator bool() const
    {
        return !m_file.empty();
    }

    static const std::vector<char>& Get()
    {
        return m_data;
    }

private:
    static std::vector<char> m_data;
    std::string m_file;
    std::streamsize m_pos = 0;
};

std::vector<char> TestDataOStream::m_data;
