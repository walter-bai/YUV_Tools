#pragma once

#include <cstdint>
#include <exception>
#include <iostream>
#include <string>
#include <vector>
#include "chroma_format.h"
#include "fourcc.h"

#define CREATE_FRAME(fourcc, width, height, name) \
  new frame::##fourcc(width, height, name)

#pragma pack(push, 1)

namespace frame
{
    class Frame
    {
    protected:
        struct Raw
        {
            using value_t = uint16_t;

            std::vector<value_t> A;
            std::vector<value_t> Y;
            std::vector<value_t> U;
            std::vector<value_t> V;
        };

    public:
        static void EnableLog(bool en)
        {
            _logEnable = en;
        }

    public:
        Frame(size_t w, size_t h, const std::string &name = "") : m_w(w), m_h(h), m_wPadded(w), m_hPadded(h), m_name(name) {}

        void SetPadding(size_t align, bool replicate)
        {
            if (_logEnable)
            {
                std::cout << std::endl << "Setting padding for \"" << m_name << "\".." << std::endl;
                std::cout << "  > Using " << (replicate ? "boundary replication" : "zero") << " padding with " << align << " aligned.." << std::endl;
            }

            if (align < 2 || (align & 1) != 0)
            {
                if (align < 2)
                {
                    return;
                }

                std::exception e("The alignment used for padding must be an even number!");
                throw e;
            }

            m_replic = replicate;
            m_wPadded = (m_w + align - 1) / align * align;
            m_hPadded = (m_h + align - 1) / align * align;

            if (_logEnable)
            {
                std::cout << "  > Original width is: " << m_w << ", height is: " << m_h << ".." << std::endl;
                std::cout << "  > Padded   width is: " << m_wPadded << ", height is: " << m_hPadded << "." << std::endl << std::endl;
            }
        }

        void ConvertFrom(const Frame& frame)
        {
            if (m_w != frame.m_w || m_wPadded != frame.m_wPadded ||
                m_h != frame.m_h || m_hPadded != frame.m_hPadded)
            {
                std::exception e("Incompatible frame type!");
                throw e;
            }

            auto depthSrc = frame.GetBitDepth();
            auto depthTarget = GetBitDepth();
            auto chromaFmtSrc = frame.GetChromaFmt();
            auto chromaFmtTarget = GetChromaFmt();

            if (HasAChannel())
            {
                if (frame.m_raw.A.empty())
                {
                    m_raw.A.resize(frame.m_raw.Y.size(), 0);
                }
                else
                {
                    m_raw.A = frame.m_raw.A;
                }
            }
            else
            {
                m_raw.A.clear();
            }

            bool rShift = depthSrc > depthTarget;
            auto shift = rShift ? depthSrc - depthTarget : depthTarget - depthSrc;

#define GET_SRC_PIXEL(PLANE, idx) (rShift ? frame.m_raw.PLANE[idx] >> shift : frame.m_raw.PLANE[idx] << shift)

            m_raw.Y.resize(frame.m_raw.Y.size());
            for (size_t i = 0; i < m_raw.Y.size(); i++)
            {
                m_raw.Y[i] = GET_SRC_PIXEL(Y, i);
            }

            size_t pixelChroma = PixelChroma(true);
            auto widthChromaPadded = WidthChroma(true);
            auto heightChromaPadded = HeightChroma(true);
            m_raw.U.resize(pixelChroma / 2, 0);
            m_raw.V.resize(pixelChroma / 2, 0);
            if (chromaFmtSrc == CHROMA_FORMAT::YUV_400 || chromaFmtTarget == CHROMA_FORMAT::YUV_400)
            {
                return;
            }
            else if (chromaFmtSrc == chromaFmtTarget)
            {
                for (size_t i = 0; i < m_raw.U.size(); i++)
                {
                    m_raw.U[i] = GET_SRC_PIXEL(U, i);
                    m_raw.V[i] = GET_SRC_PIXEL(V, i);
                }
            }
            else if (chromaFmtSrc == CHROMA_FORMAT::YUV_420 && chromaFmtTarget == CHROMA_FORMAT::YUV_422)
            {
                for (size_t h = 0; h < heightChromaPadded / 2; h++)
                {
                    for (size_t w = 0; w < widthChromaPadded; w++)
                    {
                        auto iSrc = h * widthChromaPadded + w;
                        auto iDst = 2 * h * widthChromaPadded + w;
                        m_raw.U[iDst] = GET_SRC_PIXEL(U, iSrc);
                        m_raw.U[iDst + widthChromaPadded] = m_raw.U[iDst];
                        m_raw.V[iDst] = GET_SRC_PIXEL(V, iSrc);
                        m_raw.V[iDst + widthChromaPadded] = m_raw.V[iDst];
                    }
                }
            }
            else if (chromaFmtSrc == CHROMA_FORMAT::YUV_420 && chromaFmtTarget == CHROMA_FORMAT::YUV_444)
            {
                for (size_t h = 0; h < heightChromaPadded / 2; h++)
                {
                    for (size_t w = 0; w < widthChromaPadded / 2; w++)
                    {
                        auto iSrc = h * widthChromaPadded / 2 + w;
                        auto iDst = 2 * h * widthChromaPadded + 2 * w;
                        m_raw.U[iDst] = GET_SRC_PIXEL(U, iSrc);
                        m_raw.U[iDst + 1] = m_raw.U[iDst];
                        m_raw.U[iDst + widthChromaPadded] = m_raw.U[iDst];
                        m_raw.U[iDst + widthChromaPadded + 1] = m_raw.U[iDst];
                        m_raw.V[iDst] = GET_SRC_PIXEL(V, iSrc);
                        m_raw.V[iDst + 1] = m_raw.V[iDst];
                        m_raw.V[iDst + widthChromaPadded] = m_raw.V[iDst];
                        m_raw.V[iDst + widthChromaPadded + 1] = m_raw.V[iDst];
                    }
                }
            }
            else if (chromaFmtSrc == CHROMA_FORMAT::YUV_422 && chromaFmtTarget == CHROMA_FORMAT::YUV_444)
            {
                for (size_t h = 0; h < heightChromaPadded; h++)
                {
                    for (size_t w = 0; w < widthChromaPadded / 2; w++)
                    {
                        auto iSrc = h * widthChromaPadded / 2 + w;
                        auto iDst = h * widthChromaPadded + 2 * w;
                        m_raw.U[iDst] = GET_SRC_PIXEL(U, iSrc);
                        m_raw.U[iDst + 1] = m_raw.U[iDst];
                        m_raw.V[iDst] = GET_SRC_PIXEL(V, iSrc);
                        m_raw.V[iDst + 1] = m_raw.V[iDst];
                    }
                }
            }
            else if (chromaFmtSrc == CHROMA_FORMAT::YUV_422 && chromaFmtTarget == CHROMA_FORMAT::YUV_420)
            {
                for (size_t h = 0; h < heightChromaPadded * 2; h += 2)
                {
                    for (size_t w = 0; w < widthChromaPadded; w++)
                    {
                        auto iSrc0 = h * widthChromaPadded + w;
                        auto iSrc1 = (h + 1) * widthChromaPadded + w;
                        auto iDst = h / 2 * widthChromaPadded + w;
                        m_raw.U[iDst] = (GET_SRC_PIXEL(U, iSrc0) + GET_SRC_PIXEL(U, iSrc1)) / 2;
                        m_raw.V[iDst] = (GET_SRC_PIXEL(V, iSrc0) + GET_SRC_PIXEL(V, iSrc1)) / 2;
                    }
                }
            }
            else if (chromaFmtSrc == CHROMA_FORMAT::YUV_444 && chromaFmtTarget == CHROMA_FORMAT::YUV_422)
            {
                for (size_t h = 0; h < heightChromaPadded; h++)
                {
                    for (size_t w = 0; w < widthChromaPadded * 2; w += 2)
                    {
                        auto iSrc0 = h * widthChromaPadded * 2 + w;
                        auto iSrc1 = h * widthChromaPadded * 2 + w + 1;
                        auto iDst = h * widthChromaPadded + w / 2;
                        m_raw.U[iDst] = (GET_SRC_PIXEL(U, iSrc0) + GET_SRC_PIXEL(U, iSrc1)) / 2;
                        m_raw.V[iDst] = (GET_SRC_PIXEL(V, iSrc0) + GET_SRC_PIXEL(V, iSrc1)) / 2;
                    }
                }
            }
            else  // chromaFmtSrc == CHROMA_FORMAT::YUV_444 && chromaFmtTarget == CHROMA_FORMAT::YUV_420
            {
                for (size_t h = 0; h < heightChromaPadded * 2; h += 2)
                {
                    for (size_t w = 0; w < widthChromaPadded * 2; w += 2)
                    {
                        auto iSrc0 = h * widthChromaPadded * 2 + w;
                        auto iSrc1 = h * widthChromaPadded * 2 + w + 1;
                        auto iSrc2 = (h + 1) * widthChromaPadded * 2 + w;
                        auto iSrc3 = (h + 1) * widthChromaPadded * 2 + w + 1;
                        auto iDst = h / 2 * widthChromaPadded + w / 2;
                        m_raw.U[iDst] = (GET_SRC_PIXEL(U, iSrc0) + GET_SRC_PIXEL(U, iSrc1) + GET_SRC_PIXEL(U, iSrc2) + GET_SRC_PIXEL(U, iSrc3)) / 4;
                        m_raw.V[iDst] = (GET_SRC_PIXEL(V, iSrc0) + GET_SRC_PIXEL(V, iSrc1) + GET_SRC_PIXEL(V, iSrc2) + GET_SRC_PIXEL(V, iSrc3)) / 4;
                    }
                }
            }

#undef GET_SRC_PIXEL
        }

        void Allocate()
        {
            size_t pixelLuma = PixelLuma(true);
            size_t pixelChroma = PixelChroma(true);

            if (HasAChannel())
            {
                m_raw.A.resize(pixelLuma, 0);
            }

            m_raw.Y.resize(pixelLuma, 0);
            m_raw.U.resize(pixelChroma / 2, 0);
            m_raw.V.resize(pixelChroma / 2, 0);
        }

        virtual ~Frame() = default;
        virtual size_t FrameSize(bool padded) const = 0;
        virtual CHROMA_FORMAT GetChromaFmt() const = 0;
        virtual uint8_t GetBitDepth() const = 0;
        virtual bool HasAChannel() const = 0;
        virtual void ReadFrame(const void* data) = 0;
        virtual void WriteFrame(void* data) const = 0;

    protected:
        size_t PixelLuma(bool padded) const
        {
            auto w = padded ? m_wPadded : m_w;
            auto h = padded ? m_hPadded : m_h;

            return w * h;
        }

        size_t PixelChroma(bool padded) const
        {
            size_t pixelLuma = PixelLuma(padded);
            size_t pixelChroma;

            switch (GetChromaFmt())
            {
            case CHROMA_FORMAT::YUV_420:
                pixelChroma = pixelLuma / 2;
                break;
            case CHROMA_FORMAT::YUV_422:
                pixelChroma = pixelLuma;
                break;
            case CHROMA_FORMAT::YUV_444:
                pixelChroma = pixelLuma * 2;
                break;
            case CHROMA_FORMAT::YUV_400:
            default:
                pixelChroma = 0;
                break;
            }

            return pixelChroma;
        }

        size_t WidthChroma(bool padded) const
        {
            size_t widthLuma = padded ? m_wPadded : m_w;
            size_t widthChroma;

            switch (GetChromaFmt())
            {
            case CHROMA_FORMAT::YUV_420:
            case CHROMA_FORMAT::YUV_422:
                widthChroma = widthLuma / 2;
                break;
            case CHROMA_FORMAT::YUV_444:
                widthChroma = widthLuma;
                break;
            case CHROMA_FORMAT::YUV_400:
            default:
                widthChroma = 0;
                break;
            }

            return widthChroma;
        }

        size_t HeightChroma(bool padded) const
        {
            size_t heightLuma = padded ? m_hPadded : m_h;
            size_t heightChroma;

            switch (GetChromaFmt())
            {
            case CHROMA_FORMAT::YUV_420:
                heightChroma = heightLuma / 2;
                break;
            case CHROMA_FORMAT::YUV_422:
            case CHROMA_FORMAT::YUV_444:
                heightChroma = heightLuma;
                break;
            case CHROMA_FORMAT::YUV_400:
            default:
                heightChroma = 0;
                break;
            }

            return heightChroma;
        }

        void ReplicateBoundary()
        {
            if (!m_replic)
            {
                return;
            }

            if (HasAChannel())
            {
                for (size_t h = 0; h < m_h; h++)
                {
                    for (size_t w = m_w; w < m_wPadded; w++)
                    {
                        m_raw.A[h * m_wPadded + w] = m_raw.A[h * m_wPadded + m_w - 1];
                    }
                }
                for (size_t h = m_h; h < m_hPadded; h++)
                {
                    for (size_t w = 0; w < m_wPadded; w++)
                    {
                        m_raw.A[h * m_wPadded + w] = m_raw.A[(m_h - 1) * m_wPadded + w];
                    }
                }
            }

            for (size_t h = 0; h < m_h; h++)
            {
                for (size_t w = m_w; w < m_wPadded; w++)
                {
                    m_raw.Y[h * m_wPadded + w] = m_raw.Y[h * m_wPadded + m_w - 1];
                }
            }
            for (size_t h = m_h; h < m_hPadded; h++)
            {
                for (size_t w = 0; w < m_wPadded; w++)
                {
                    m_raw.Y[h * m_wPadded + w] = m_raw.Y[(m_h - 1) * m_wPadded + w];
                }
            }

            auto widthChroma = WidthChroma(false);
            auto widthChromaPadded = WidthChroma(true);
            auto heightChroma = HeightChroma(false);
            auto heightChromaPadded = HeightChroma(true);

            for (size_t h = 0; h < heightChroma; h++)
            {
                for (size_t w = widthChroma; w < widthChromaPadded; w++)
                {
                    m_raw.U[h * widthChromaPadded + w] = m_raw.U[h * widthChromaPadded + widthChroma - 1];
                    m_raw.V[h * widthChromaPadded + w] = m_raw.V[h * widthChromaPadded + widthChroma - 1];
                }
            }
            for (size_t h = heightChroma; h < heightChromaPadded; h++)
            {
                for (size_t w = 0; w < widthChromaPadded; w++)
                {
                    m_raw.U[h * widthChromaPadded + w] = m_raw.U[(heightChroma - 1) * widthChromaPadded + w];
                    m_raw.V[h * widthChromaPadded + w] = m_raw.V[(heightChroma - 1) * widthChromaPadded + w];
                }
            }
        }

    protected:
        size_t m_w = 0;
        size_t m_wPadded = 0;
        size_t m_h = 0;
        size_t m_hPadded = 0;
        bool m_replic = false;
        Raw m_raw;

        // logging
        std::string m_name;
        static bool _logEnable;
    };

    bool Frame::_logEnable = false;

    template <typename pixel_t, CHROMA_FORMAT FMT, uint8_t DEPTH>
    class FrameNonPacked : public Frame
    {
    public:
        FrameNonPacked(size_t w, size_t h, const std::string& name = "") : Frame(w, h, name) {}

        size_t FrameSize(bool padded) const override
        {
            return (PixelLuma(padded) + PixelChroma(padded)) * sizeof(pixel_t);
        }

        CHROMA_FORMAT GetChromaFmt() const override
        {
            return FMT;
        }

        uint8_t GetBitDepth() const override
        {
            return DEPTH;
        }

        bool HasAChannel() const override
        {
            return false;
        }
    };

    template <typename pixel_t, CHROMA_FORMAT FMT, uint8_t DEPTH, uint8_t SHIFT = 0>
    class FramePlanar : public FrameNonPacked<pixel_t, FMT, DEPTH>
    {
    public:
        FramePlanar(size_t w, size_t h, const std::string& name = "") : FrameNonPacked(w, h, name) {}

        void ReadFrame(const void* data) override
        {
            auto p = reinterpret_cast<const pixel_t*>(data);
            size_t skipped = 0;
            for (size_t i = 0; i < PixelLuma(false); i++)
            {
                m_raw.Y[i + skipped] = p[i] >> SHIFT;
                if (i % m_w == m_w - 1)
                {
                    skipped += m_wPadded - m_w;
                }
            }

            auto widthChroma = WidthChroma(false);
            auto widthChromaPadded = WidthChroma(true);

            p += PixelLuma(false);
            skipped = 0;
            size_t pixelU = PixelChroma(false) / 2;
            for (size_t i = 0; i < pixelU; i++)
            {
                m_raw.U[i + skipped] = p[i] >> SHIFT;
                m_raw.V[i + skipped] = p[i + pixelU] >> SHIFT;
                if (i % widthChroma == widthChroma - 1)
                {
                    skipped += widthChromaPadded - widthChroma;
                }
            }

            ReplicateBoundary();
        }

        void WriteFrame(void* data) const override
        {
            auto p = reinterpret_cast<pixel_t*>(data);
            for (size_t i = 0; i < m_raw.Y.size(); i++)
            {
                p[i] = static_cast<pixel_t>(m_raw.Y[i] << SHIFT);
            }

            p += m_raw.Y.size();
            size_t pixelU = m_raw.U.size();
            for (size_t i = 0; i < pixelU; i++)
            {
                p[i] = static_cast<pixel_t>(m_raw.U[i] << SHIFT);
                p[i + pixelU] = static_cast<pixel_t>(m_raw.V[i] << SHIFT);
            }
        }
    };

    template <typename pixel_t, CHROMA_FORMAT FMT, uint8_t DEPTH, uint8_t SHIFT = 0>
    class FrameInterleaved : public FrameNonPacked<pixel_t, FMT, DEPTH>
    {
    public:
        FrameInterleaved(size_t w, size_t h, const std::string& name = "") : FrameNonPacked(w, h, name) {}

        void ReadFrame(const void* data) override
        {
            auto p = reinterpret_cast<const pixel_t*>(data);
            size_t skipped = 0;
            for (size_t i = 0; i < PixelLuma(false); i++)
            {
                m_raw.Y[i + skipped] = p[i] >> SHIFT;
                if (i % m_w == m_w - 1)
                {
                    skipped += m_wPadded - m_w;
                }
            }

            auto widthChroma = WidthChroma(false);
            auto widthChromaPadded = WidthChroma(true);

            p += PixelLuma(false);
            skipped = 0;
            for (size_t i = 0; i < PixelChroma(false) / 2; i++)
            {
                m_raw.U[i + skipped] = p[2 * i] >> SHIFT;
                m_raw.V[i + skipped] = p[2 * i + 1] >> SHIFT;
                if (i % widthChroma == widthChroma - 1)
                {
                    skipped += widthChromaPadded - widthChroma;
                }
            }

            ReplicateBoundary();
        }

        void WriteFrame(void* data) const override
        {
            auto p = reinterpret_cast<pixel_t*>(data);
            for (size_t i = 0; i < m_raw.Y.size(); i++)
            {
                p[i] = static_cast<pixel_t>(m_raw.Y[i] << SHIFT);
            }

            p += m_raw.Y.size();
            for (size_t i = 0; i < m_raw.U.size(); i++)
            {
                p[2 * i] = static_cast<pixel_t>(m_raw.U[i] << SHIFT);
                p[2 * i + 1] = static_cast<pixel_t>(m_raw.V[i] << SHIFT);
            }
        }
    };

    using NV12 = FrameInterleaved<uint8_t, CHROMA_FORMAT::YUV_420, 8>;
    using P010 = FrameInterleaved<uint16_t, CHROMA_FORMAT::YUV_420, 10, 6>;
    using I420 = FramePlanar<uint8_t, CHROMA_FORMAT::YUV_420, 8>;
    using IYUV = I420;
    using GREY = FramePlanar<uint8_t, CHROMA_FORMAT::YUV_400, 8>;

    class Y410 : public Frame
    {
    public:
        struct Pixel
        {
            uint32_t U : 10;
            uint32_t Y : 10;
            uint32_t V : 10;
            uint32_t A : 2;
        };

    public:
        Y410(size_t w, size_t h, const std::string &name = "") : Frame(w, h, name) {}

        size_t FrameSize(bool padded) const override
        {
            return (padded ? m_wPadded * m_hPadded : m_w * m_h) * sizeof(Pixel);
        }

        CHROMA_FORMAT GetChromaFmt() const override
        {
            return CHROMA_FORMAT::YUV_444;
        }

        uint8_t GetBitDepth() const override
        {
            return 10;
        }

        bool HasAChannel() const override
        {
            return true;
        }

        void ReadFrame(const void* data) override
        {
            auto p = reinterpret_cast<const Pixel*>(data);
            size_t skipped = 0;
            for (size_t i = 0; i < m_w * m_h; ++i)
            {
                m_raw.A[i + skipped] = p[i].A;
                m_raw.Y[i + skipped] = p[i].Y;
                m_raw.U[i + skipped] = p[i].U;
                m_raw.V[i + skipped] = p[i].V;
                if (i % m_w == m_w - 1)
                {
                    skipped += m_wPadded - m_w;
                }
            }

            ReplicateBoundary();
        }

        void WriteFrame(void* data) const override
        {
            auto p = reinterpret_cast<Pixel*>(data);
            for (size_t i = 0; i < m_raw.A.size(); ++i)
            {
                p[i].A = m_raw.A[i];
                p[i].Y = m_raw.Y[i];
                p[i].U = m_raw.U[i];
                p[i].V = m_raw.V[i];
            }
        }
    };

    class Y210 : public Frame
    {
    public:
        struct Pixel
        {
            uint32_t Y : 16;
            uint32_t Chroma : 16;  // even for U, odd for V
        };

    public:
        Y210(size_t w, size_t h, const std::string& name = "") : Frame(w, h, name) {}

        size_t FrameSize(bool padded) const override
        {
            return (padded ? m_wPadded * m_hPadded : m_w * m_h) * sizeof(Pixel);
        }

        CHROMA_FORMAT GetChromaFmt() const override
        {
            return CHROMA_FORMAT::YUV_422;
        }

        uint8_t GetBitDepth() const override
        {
            return 10;
        }

        bool HasAChannel() const override
        {
            return false;
        }

        void ReadFrame(const void* data) override
        {
            auto p = reinterpret_cast<const Pixel*>(data);
            size_t skipped = 0;
            for (size_t i = 0; i < m_w * m_h; ++i)
            {
                m_raw.Y[i + skipped] = p[i].Y >> 6;
                if ((i + skipped) % 2 == 0)
                {
                    m_raw.U[(i + skipped) / 2] = p[i].Chroma >> 6;
                }
                else
                {
                    m_raw.V[(i + skipped) / 2] = p[i].Chroma >> 6;
                }
                if (i % m_w == m_w - 1)
                {
                    skipped += m_wPadded - m_w;
                }
            }

            ReplicateBoundary();
        }

        void WriteFrame(void* data) const override
        {
            auto p = reinterpret_cast<Pixel*>(data);
            for (size_t i = 0; i < m_raw.Y.size(); ++i)
            {
                p[i].Y = m_raw.Y[i] << 6;
                if (i % 2 == 0)
                {
                    p[i].Chroma = m_raw.U[i / 2] << 6;
                }
                else
                {
                    p[i].Chroma = m_raw.V[i / 2] << 6;
                }
            }
        }
    };
}

#pragma pack(pop)
