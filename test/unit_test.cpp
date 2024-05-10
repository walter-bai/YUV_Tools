#include <fstream>
#include <memory>
#include <vector>
#include <string>
#include <gtest/gtest.h>
#include "picosha2.h"
#include "sha256.h"
#include "../src/frame_converter.hpp"

class IStream
{
public:
    void open(const std::string& filename, std::ios_base::openmode mode)
    {
        if (m_data.count(filename))
        {
            return;
        }

        std::ifstream ifs;
        ifs.open(filename, mode);
    }

private:
    std::map<std::string, std::vector<char>> m_data;
    std::string m_file;
};

class FrameConverterTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        if (m_read)
        {
            return;
        }

        auto size = GetFileSize(InputName);
        EXPECT_NE(size, -1);

        std::vector<char> data;
        data.resize(size, 0);
        std::ifstream file(InputName, std::ifstream::binary);
        file.read(data.data(), size);

        m_inputFrame.Allocate();
        m_inputFrame.ReadFrame(data.data());

        m_read = true;
    }

    void TearDown() override
    {
        m_outputFrame.release();
    }

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

    std::string GetSHA256(const std::vector<char>& data)
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
    static constexpr size_t InputW = 1920;
    static constexpr size_t InputH = 1080;
    static constexpr FOURCC InputFourCC = FOURCC::Y410;
    static constexpr const char* InputName = "Test_1920x1080_1frame.y410";
    bool m_read = false;
    InputFrame m_inputFrame{ InputW, InputH, "in" };
    std::unique_ptr<frame::Frame> m_outputFrame;
};

TEST_F(FrameConverterTest, SelfConversion)
{
    m_outputFrame = std::make_unique<InputFrame>(InputW, InputH, "out");
    m_outputFrame->ConvertFrom(m_inputFrame);

    std::vector<char> data(m_outputFrame->FrameSize(true));
    EXPECT_EQ(data.size(), m_inputFrame.FrameSize(false));  // out frame should have no padding

    m_outputFrame->WriteFrame(data.data());
    EXPECT_EQ(GetSHA256(data), g_sha256.at(InputFourCC));
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
