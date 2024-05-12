#pragma once

#include <map>
#include <string>
#include "../src/fourcc.h"

static const std::map<FOURCC, std::string> g_sha256Input = 
{
    {FOURCC::Y410, "202c0ab21698dd6801b407b0b821f25fda084bdee94e1d7ddded87805d354f41"},
    {FOURCC::VUYX, "52c15c56128bf3d43c1905aaa8098797478dbccb41cf0c37f9db99ea24ffc30e"},
    {FOURCC::I440, "d733d96f2486113d061bc13dd955389f80c3db6019113027128a4a58349e3b0d"},
    {FOURCC::YUYV, "6c4736681aad7f344e04060cf90815ca84f6d852fa53d91cad6d69baaf5286fb"},
    {FOURCC::NV12, "946b2078e9cf099191c8d2ac7cca414fb831fc2e9476d28af61502f2cf1e09e7"},
    {FOURCC::I400, "46e6b657334581df1360ca77b634c7379be79497c7e9efb9c8262b94eb8b50f0"},
};

static const std::map<FOURCC, std::string> g_sha256ChromaSampling444 =
{
    {FOURCC::I440, "aca5cd9abee869919f40bc48788da7d75224c4fa8cf32e4ece61c63f63c7c1ff"},
    {FOURCC::YUYV, "5e370ab5aa5a40c6b09baf7f1d29950a4aa2dd38d34797615bac1dbaf5268dd4"},
    {FOURCC::NV12, "21a8fb32b720d8a85c6bad465d8300f2a32763eddce78f388405f23f4d7359f6"},
    {FOURCC::I400, "f38287dd89307b538a954f8ce3626c236750079b64d25e14188a6765036d0a3d"},
};

static const std::map<FOURCC, std::string> g_sha256ChromaSampling440 =
{
    {FOURCC::VUYX, "d1e3da00982e31c4184e73864e656d3d6793199fcb20539da86ee22a2b5a33bf"},
    {FOURCC::YUYV, "6c4736681aad7f344e04060cf90815ca84f6d852fa53d91cad6d69baaf5286fb"},
    {FOURCC::I420, "553d4775e1c68415eec86df3673b330a3e852491569bd3babc9c51445a493a03"},
    {FOURCC::I400, "c0e6376a22746edb5db07b96841bd267344464e08d3d5eb8cc8b071db4cf95bd"},
};

static const std::map<FOURCC, std::string> g_sha256ChromaSampling422 =
{
    {FOURCC::VUYX, "95b4d6db819c0cc776e99e8e744be5bbb7dcd0487852abb2f49f2e5e66be0545"},
    {FOURCC::I440, "d733d96f2486113d061bc13dd955389f80c3db6019113027128a4a58349e3b0d"},
    {FOURCC::I420, "553d4775e1c68415eec86df3673b330a3e852491569bd3babc9c51445a493a03"},
    {FOURCC::I400, "c0e6376a22746edb5db07b96841bd267344464e08d3d5eb8cc8b071db4cf95bd"},
};

static const std::map<FOURCC, std::string> g_sha256ChromaSampling420 =
{
    {FOURCC::VUYX, "1b87ba66a0c09407ece74ec56580c4a83e835abc915eca1a42878cb14bd32d44"},
    {FOURCC::I440, "1cb0ca70986d3c74a97df6b41254469fd2f225f259c6f4885b8a1f452e575497"},
    {FOURCC::I422, "7cb42c8aef1296b8bf72316eb5e39de2f44fd46e210b55e3708007a49769baba"},
    {FOURCC::I400, "46e6b657334581df1360ca77b634c7379be79497c7e9efb9c8262b94eb8b50f0"},
};

static const std::map<FOURCC, std::string> g_sha256ChromaSampling400 =
{
    {FOURCC::I444, "e043ca69f227d4d28ef51aa0aa6478e5b57f5f4ce2813ef9f7c89a85ffc43a76"},
    {FOURCC::I440, "4cca98f536ad22666c8cec1e2f407619360fd2964b4315fad15a7e1415dbfe4b"},
    {FOURCC::I422, "4cca98f536ad22666c8cec1e2f407619360fd2964b4315fad15a7e1415dbfe4b"},
    {FOURCC::I420, "7addc83f60ddc9ac2126856cd08f60b6bbf8bae63758b902f45a0e006987212b"},
};
