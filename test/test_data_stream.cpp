#include <fstream>
#include "gtest/gtest.h"
#include "test_data_stream.h"

void TestDataIStream::open(const std::string& filename, std::ios_base::openmode mode)
{
    m_file = filename;
    m_pos = 0;
    m_gcount = 0;

    if (m_data.count(filename))
    {
        return;
    }

    std::streamsize size = 0;
    auto res = LoadResourceData(filename, size);
    if (res != nullptr)
    {
        auto& buf = m_data[filename];
        buf.first = res;
        buf.second = size;
        return;
    }

    std::ifstream ifs;
    ifs.open(filename, mode);
    EXPECT_EQ(ifs.is_open(), true);

    size = GetFileSize(filename);
    EXPECT_NE(size, -1);

    auto& buf = m_data[filename];
    m_buf = std::make_unique<const char[]>(size);
    buf.first = m_buf.get();
    buf.second = size;
    ifs.read(const_cast<char *>(buf.first), size);
}

TestDataIStream& TestDataIStream::read(char* s, std::streamsize n)
{
    auto& buf = m_data[m_file];
    auto sz = std::min(n, static_cast<std::streamsize>(buf.second) - m_pos);

    memcpy(s, buf.first + m_pos, sz);
    m_pos += sz;
    m_gcount = sz;

    return *this;
}

TestDataIStream& TestDataIStream::seekg(std::streampos pos)
{
    EXPECT_EQ(m_data[m_file].second > pos, true);

    m_pos = pos;

    return *this;
}

std::streamsize TestDataIStream::GetFileSize(const std::string& filename) const
{
    std::ifstream file(filename, std::ifstream::ate | std::ifstream::binary);

    EXPECT_EQ(file.is_open(), true);

    return file.tellg();
}

void TestDataOStream::open(const std::string& filename, std::ios_base::openmode mode)
{
    std::ignore = mode;

    m_file = filename;
    _data.clear();
    m_pos = 0;
}

TestDataOStream& TestDataOStream::write(const char* s, std::streamsize n)
{
    if (_data.size() < m_pos + n)
    {
        _data.resize(m_pos + n);
    }

    memcpy(_data.data() + m_pos, s, n);
    m_pos += n;

    return *this;
}

std::vector<char> TestDataOStream::_data;
