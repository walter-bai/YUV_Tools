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

TEST_F(FrameConverterTest, ChromaSampling444)
{
    {
        // 444 -> 440
        char* cmdline[] = { "-w", "1920", "-h", "1080", "-i:ayuv", "Test_1920x1080_1frame.ayuv", "-o:i440", "out.yuv" };
        converter::FrameConverter<TestDataIStream, TestDataOStream> cvt;
        cvt.Execute(sizeof(cmdline) / sizeof(cmdline[0]), cmdline);
        EXPECT_EQ(GetSHA256(TestDataOStream::Get()), g_sha256ChromaSampling444.at(FOURCC::I440));
    }
    {
        // 444 -> 422
        char* cmdline[] = { "-w", "1920", "-h", "1080", "-i:ayuv", "Test_1920x1080_1frame.ayuv", "-o:yuyv", "out.yuv" };
        converter::FrameConverter<TestDataIStream, TestDataOStream> cvt;
        cvt.Execute(sizeof(cmdline) / sizeof(cmdline[0]), cmdline);
        EXPECT_EQ(GetSHA256(TestDataOStream::Get()), g_sha256ChromaSampling444.at(FOURCC::YUYV));
    }
    {
        // 444 -> 420
        char* cmdline[] = { "-w", "1920", "-h", "1080", "-i:ayuv", "Test_1920x1080_1frame.ayuv", "-o:nv12", "out.yuv" };
        converter::FrameConverter<TestDataIStream, TestDataOStream> cvt;
        cvt.Execute(sizeof(cmdline) / sizeof(cmdline[0]), cmdline);
        EXPECT_EQ(GetSHA256(TestDataOStream::Get()), g_sha256ChromaSampling444.at(FOURCC::NV12));
    }
    {
        // 444 -> 400
        char* cmdline[] = { "-w", "1920", "-h", "1080", "-i:ayuv", "Test_1920x1080_1frame.ayuv", "-o:i400", "out.yuv" };
        converter::FrameConverter<TestDataIStream, TestDataOStream> cvt;
        cvt.Execute(sizeof(cmdline) / sizeof(cmdline[0]), cmdline);
        EXPECT_EQ(GetSHA256(TestDataOStream::Get()), g_sha256ChromaSampling444.at(FOURCC::I400));
    }
}

TEST_F(FrameConverterTest, ChromaSampling440)
{
    {
        // 440 -> 444
        char* cmdline[] = { "-w", "1918", "-h", "1078", "-i:i440", "Test_1918x1078_1frame.i440", "-o:ayuv", "out.yuv" };
        converter::FrameConverter<TestDataIStream, TestDataOStream> cvt;
        cvt.Execute(sizeof(cmdline) / sizeof(cmdline[0]), cmdline);
        EXPECT_EQ(GetSHA256(TestDataOStream::Get()), g_sha256ChromaSampling440.at(FOURCC::VUYX));
    }
    {
        // 440 -> 422
        char* cmdline[] = { "-w", "1918", "-h", "1078", "-i:i440", "Test_1918x1078_1frame.i440", "-o:yuyv", "out.yuv" };
        converter::FrameConverter<TestDataIStream, TestDataOStream> cvt;
        cvt.Execute(sizeof(cmdline) / sizeof(cmdline[0]), cmdline);
        EXPECT_EQ(GetSHA256(TestDataOStream::Get()), g_sha256ChromaSampling440.at(FOURCC::YUYV));
    }
    {
        // 440 -> 420
        char* cmdline[] = { "-w", "1918", "-h", "1078", "-i:i440", "Test_1918x1078_1frame.i440", "-o:i420", "out.yuv" };
        converter::FrameConverter<TestDataIStream, TestDataOStream> cvt;
        cvt.Execute(sizeof(cmdline) / sizeof(cmdline[0]), cmdline);
        EXPECT_EQ(GetSHA256(TestDataOStream::Get()), g_sha256ChromaSampling440.at(FOURCC::I420));
    }
    {
        // 440 -> 400
        char* cmdline[] = { "-w", "1918", "-h", "1078", "-i:i440", "Test_1918x1078_1frame.i440", "-o:i400", "out.yuv" };
        converter::FrameConverter<TestDataIStream, TestDataOStream> cvt;
        cvt.Execute(sizeof(cmdline) / sizeof(cmdline[0]), cmdline);
        EXPECT_EQ(GetSHA256(TestDataOStream::Get()), g_sha256ChromaSampling440.at(FOURCC::I400));
    }
}

TEST_F(FrameConverterTest, ChromaSampling422)
{
    {
        // 422 -> 444
        char* cmdline[] = { "-w", "1918", "-h", "1078", "-i:yuyv", "Test_1918x1078_1frame.yuyv", "-o:ayuv", "out.yuv" };
        converter::FrameConverter<TestDataIStream, TestDataOStream> cvt;
        cvt.Execute(sizeof(cmdline) / sizeof(cmdline[0]), cmdline);
        EXPECT_EQ(GetSHA256(TestDataOStream::Get()), g_sha256ChromaSampling422.at(FOURCC::VUYX));
    }
    {
        // 422 -> 440
        char* cmdline[] = { "-w", "1918", "-h", "1078", "-i:yuyv", "Test_1918x1078_1frame.yuyv", "-o:i440", "out.yuv" };
        converter::FrameConverter<TestDataIStream, TestDataOStream> cvt;
        cvt.Execute(sizeof(cmdline) / sizeof(cmdline[0]), cmdline);
        EXPECT_EQ(GetSHA256(TestDataOStream::Get()), g_sha256ChromaSampling422.at(FOURCC::I440));
    }
    {
        // 422 -> 420
        char* cmdline[] = { "-w", "1918", "-h", "1078", "-i:yuyv", "Test_1918x1078_1frame.yuyv", "-o:i420", "out.yuv" };
        converter::FrameConverter<TestDataIStream, TestDataOStream> cvt;
        cvt.Execute(sizeof(cmdline) / sizeof(cmdline[0]), cmdline);
        EXPECT_EQ(GetSHA256(TestDataOStream::Get()), g_sha256ChromaSampling422.at(FOURCC::I420));
    }
    {
        // 422 -> 400
        char* cmdline[] = { "-w", "1918", "-h", "1078", "-i:yuyv", "Test_1918x1078_1frame.yuyv", "-o:i400", "out.yuv" };
        converter::FrameConverter<TestDataIStream, TestDataOStream> cvt;
        cvt.Execute(sizeof(cmdline) / sizeof(cmdline[0]), cmdline);
        EXPECT_EQ(GetSHA256(TestDataOStream::Get()), g_sha256ChromaSampling422.at(FOURCC::I400));
    }
}

TEST_F(FrameConverterTest, ChromaSampling420)
{
    {
        // 420 -> 444
        char* cmdline[] = { "-w", "3840", "-h", "2160", "-i:nv12", "Test_3840x2160_1frame.nv12", "-o:ayuv", "out.yuv" };
        converter::FrameConverter<TestDataIStream, TestDataOStream> cvt;
        cvt.Execute(sizeof(cmdline) / sizeof(cmdline[0]), cmdline);
        EXPECT_EQ(GetSHA256(TestDataOStream::Get()), g_sha256ChromaSampling420.at(FOURCC::VUYX));
    }
    {
        // 420 -> 440
        char* cmdline[] = { "-w", "3840", "-h", "2160", "-i:nv12", "Test_3840x2160_1frame.nv12", "-o:i440", "out.yuv" };
        converter::FrameConverter<TestDataIStream, TestDataOStream> cvt;
        cvt.Execute(sizeof(cmdline) / sizeof(cmdline[0]), cmdline);
        EXPECT_EQ(GetSHA256(TestDataOStream::Get()), g_sha256ChromaSampling420.at(FOURCC::I440));
    }
    {
        // 420 -> 422
        char* cmdline[] = { "-w", "3840", "-h", "2160", "-i:nv12", "Test_3840x2160_1frame.nv12", "-o:i422", "out.yuv" };
        converter::FrameConverter<TestDataIStream, TestDataOStream> cvt;
        cvt.Execute(sizeof(cmdline) / sizeof(cmdline[0]), cmdline);
        EXPECT_EQ(GetSHA256(TestDataOStream::Get()), g_sha256ChromaSampling420.at(FOURCC::I422));
    }
    {
        // 420 -> 400
        char* cmdline[] = { "-w", "3840", "-h", "2160", "-i:nv12", "Test_3840x2160_1frame.nv12", "-o:i400", "out.yuv" };
        converter::FrameConverter<TestDataIStream, TestDataOStream> cvt;
        cvt.Execute(sizeof(cmdline) / sizeof(cmdline[0]), cmdline);
        EXPECT_EQ(GetSHA256(TestDataOStream::Get()), g_sha256ChromaSampling420.at(FOURCC::I400));
    }
}

TEST_F(FrameConverterTest, ChromaSampling400)
{
    {
        // 400 -> 444
        char* cmdline[] = { "-w", "3840", "-h", "2160", "-i:i400", "Test_3840x2160_1frame.i400", "-o:ayuv", "out.yuv" };
        converter::FrameConverter<TestDataIStream, TestDataOStream> cvt;
        cvt.Execute(sizeof(cmdline) / sizeof(cmdline[0]), cmdline);
        EXPECT_EQ(GetSHA256(TestDataOStream::Get()), g_sha256ChromaSampling400.at(FOURCC::VUYX));
    }
    {
        // 400 -> 440
        char* cmdline[] = { "-w", "3840", "-h", "2160", "-i:i400", "Test_3840x2160_1frame.i400", "-o:i440", "out.yuv" };
        converter::FrameConverter<TestDataIStream, TestDataOStream> cvt;
        cvt.Execute(sizeof(cmdline) / sizeof(cmdline[0]), cmdline);
        EXPECT_EQ(GetSHA256(TestDataOStream::Get()), g_sha256ChromaSampling400.at(FOURCC::I440));
    }
    {
        // 400 -> 422
        char* cmdline[] = { "-w", "3840", "-h", "2160", "-i:i400", "Test_3840x2160_1frame.i400", "-o:i422", "out.yuv" };
        converter::FrameConverter<TestDataIStream, TestDataOStream> cvt;
        cvt.Execute(sizeof(cmdline) / sizeof(cmdline[0]), cmdline);
        EXPECT_EQ(GetSHA256(TestDataOStream::Get()), g_sha256ChromaSampling400.at(FOURCC::I422));
    }
    {
        // 400 -> 420
        char* cmdline[] = { "-w", "3840", "-h", "2160", "-i:i400", "Test_3840x2160_1frame.i400", "-o:p010", "out.yuv" };
        converter::FrameConverter<TestDataIStream, TestDataOStream> cvt;
        cvt.Execute(sizeof(cmdline) / sizeof(cmdline[0]), cmdline);
        EXPECT_EQ(GetSHA256(TestDataOStream::Get()), g_sha256ChromaSampling400.at(FOURCC::P010));
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
