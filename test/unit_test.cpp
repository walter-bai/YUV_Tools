#include "../src/frame_converter.hpp"
#include "picosha2.h"
#include "sha256.h"
#include "test_data_stream.hpp"

class FrameConverterTest : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}

    std::string GetSHA256(const std::vector<char>& data)
    {
        picosha2::hash256_one_by_one hasher;
        hasher.process(data.begin(), data.end());
        hasher.finish();

        std::vector<unsigned char> hash(picosha2::k_digest_size);
        hasher.get_hash_bytes(hash.begin(), hash.end());

        return picosha2::get_hash_hex_string(hasher);
    }
};

TEST_F(FrameConverterTest, SelfConversion)
{
    char* cmdline[] = { "-w", "1920", "-h", "1080", "-i:y410", "Test_1920x1080_1frame.y410", "-o:y410", "out.yuv" };

    converter::FrameConverter<TestDataIStream, TestDataOStream> cvt;
    cvt.Execute(sizeof(cmdline) / sizeof(cmdline[0]), cmdline);
    EXPECT_EQ(GetSHA256(TestDataOStream::Get()), g_sha256Input.at(FOURCC::Y410));
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
