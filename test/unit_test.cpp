#include <fstream>
#include <vector>
#include <string>
#include <gtest/gtest.h>
#include "picosha2.h"
#include "sha256.h"
#include "../src/frame.hpp"

class Tests : public ::testing::Test
{
protected:
    void SetUp() override
    {
        if (!m_inputData.empty())
        {
            return;
        }

        auto size = GetFileSize(m_inputName);
        EXPECT_NE(size, -1);

        m_inputData.resize(size, 0);
        std::ifstream file(m_inputName, std::ifstream::binary);
        file.read(m_inputData.data(), size);
    }

    void TearDown() override {}

    std::streamsize GetFileSize(const std::string& filename)
    {
        std::ifstream file(filename, std::ifstream::ate | std::ifstream::binary);
        if (!file)
        {
            std::cerr << "Cannot open the file: " << filename << std::endl;
            return -1;
        }

        return file.tellg();
    }

    std::string GetSHA256(const std::vector<char> &data)
    {
        picosha2::hash256_one_by_one hasher;
        hasher.process(data.begin(), data.end());
        hasher.finish();

        std::vector<unsigned char> hash(picosha2::k_digest_size);
        hasher.get_hash_bytes(hash.begin(), hash.end());

        return picosha2::get_hash_hex_string(hasher);
    }
protected:
    using InputFrame = frame::Y410;
    static constexpr size_t m_inputW = 1920;
    static constexpr size_t m_inputH = 1080;
    static constexpr FOURCC m_inputFourCC = FOURCC::Y410;
    const std::string m_inputName = "Test_1920x1080_1frame.y410";
    std::vector<char> m_inputData;
};

TEST_F(Tests, SelfConversion)
{
    InputFrame in(m_inputW, m_inputH, "in");
    in.Allocate();
    in.ReadFrame(m_inputData.data());
    
    InputFrame out(m_inputW, m_inputH, "out");
    out.ConvertFrom(in);

    std::vector<char> data(out.FrameSize(true));
    EXPECT_EQ(data.size(), in.FrameSize(false));  // out frame should have no padding

    out.WriteFrame(data.data());
    EXPECT_EQ(GetSHA256(data), g_sha256.at(m_inputFourCC));
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
