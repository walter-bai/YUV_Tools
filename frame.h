#pragma once

#include <iostream>
#include <cstdint>
#include <vector>
#include <exception>

enum class CHROMA_IDC
{
    IDC_400,
    IDC_420,
    IDC_422,
    IDC_444,
};

enum class FOURCC
{
    UNDEF,
    NV12,
    P010,
    Y410,
    YUV44410P,
    YUV4208P,
    Y210,
};

#pragma pack(push, 1)

namespace frame
{
    struct Raw
    {
        using value_t = uint16_t;

        std::vector<value_t> A;
        std::vector<value_t> Y;
        std::vector<value_t> U;
        std::vector<value_t> V;
    };

    class Frame
    {
    public:
        Frame(size_t w, size_t h, size_t padding = 0) : m_w(w), m_h(h)
        {
            if (padding)
            {
                m_wPadded = (m_w + padding - 1) / padding * padding;
                m_hPadded = (m_h + padding - 1) / padding * padding;
            }
            else
            {
                m_wPadded = m_w;
                m_hPadded = m_h;
            }

            std::cout << "Padded width is: " << m_wPadded << ", height is: " << m_hPadded << std::endl;
        }

        void ConvertFrom(const Frame &frame)
        {
            if (GetChromaIDC() != frame.GetChromaIDC() ||
                m_w != frame.m_w || m_wPadded != frame.m_wPadded ||
                m_h != frame.m_h || m_hPadded != frame.m_hPadded)
            {
                std::exception e("Incompatible frame type!");
                throw e;
            }

            auto depthSrc = frame.GetBitDepth();
            auto depthTarget = GetBitDepth();

            if (depthSrc == depthTarget)
            {
                m_raw = frame.m_raw;
            }
            else
            {
                m_raw.A = frame.m_raw.A;

                bool rShift = depthSrc > depthTarget;
                auto shift = rShift ? depthSrc - depthTarget : depthTarget - depthSrc;

                m_raw.Y.resize(frame.m_raw.Y.size());
                for (size_t i = 0; i < m_raw.Y.size(); i++)
                {
                    m_raw.Y[i] = rShift ? frame.m_raw.Y[i] >> shift : frame.m_raw.Y[i] << shift;
                }

                m_raw.U.resize(frame.m_raw.U.size());
                m_raw.V.resize(frame.m_raw.V.size());
                for (size_t i = 0; i < m_raw.U.size(); i++)
                {
                    m_raw.U[i] = rShift ? frame.m_raw.U[i] >> shift : frame.m_raw.U[i] << shift;
                    m_raw.V[i] = rShift ? frame.m_raw.V[i] >> shift : frame.m_raw.V[i] << shift;
                }
            }
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
        virtual CHROMA_IDC GetChromaIDC() const = 0;
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

            switch (GetChromaIDC())
            {
            case CHROMA_IDC::IDC_420:
                pixelChroma = pixelLuma / 2;
                break;
            case CHROMA_IDC::IDC_422:
                pixelChroma = pixelLuma;
                break;
            case CHROMA_IDC::IDC_444:
                pixelChroma = pixelLuma * 2;
                break;
            case CHROMA_IDC::IDC_400:
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

            switch (GetChromaIDC())
            {
            case CHROMA_IDC::IDC_420:
            case CHROMA_IDC::IDC_422:
                widthChroma = widthLuma / 2;
                break;
            case CHROMA_IDC::IDC_444:
                widthChroma = widthLuma;
                break;
            case CHROMA_IDC::IDC_400:
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

            switch (GetChromaIDC())
            {
            case CHROMA_IDC::IDC_420:
                heightChroma = heightLuma / 2;
                break;
            case CHROMA_IDC::IDC_422:
            case CHROMA_IDC::IDC_444:
                heightChroma = heightLuma;
                break;
            case CHROMA_IDC::IDC_400:
            default:
                heightChroma = 0;
                break;
            }

            return heightChroma;
        }

        void ReplicateBoundary()
        {
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
        Raw m_raw;
    };

    template <typename pixel_t, CHROMA_IDC IDC, uint8_t DEPTH>
    class FrameNonPacked : public Frame
    {
    public:
        FrameNonPacked(size_t w, size_t h, size_t padding = 0) : Frame(w, h, padding) {}

        size_t FrameSize(bool padded) const override
        {
            return (PixelLuma(padded) + PixelChroma(padded)) * sizeof(pixel_t);
        }

        CHROMA_IDC GetChromaIDC() const override
        {
            return IDC;
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

    template <typename pixel_t, CHROMA_IDC IDC, uint8_t DEPTH, uint8_t SHIFT = 0>
    class FramePlanar : public FrameNonPacked<pixel_t, IDC, DEPTH>
    {
    public:
        FramePlanar(size_t w, size_t h, size_t padding = 0) : FrameNonPacked(w, h, padding) {}

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

    template <typename pixel_t, CHROMA_IDC IDC, uint8_t DEPTH, uint8_t SHIFT = 0>
    class FrameInterleaved : public FrameNonPacked<pixel_t, IDC, DEPTH>
    {
    public:
        FrameInterleaved(size_t w, size_t h, size_t padding = 0) : FrameNonPacked(w, h, padding) {}

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

    using NV12 = FrameInterleaved<uint8_t, CHROMA_IDC::IDC_420, 8>;
    using P010 = FrameInterleaved<uint16_t, CHROMA_IDC::IDC_420, 10, 6>;
    using YUV4208P = FramePlanar<uint8_t, CHROMA_IDC::IDC_420, 8>;
    using YUV44410P = FramePlanar<uint16_t, CHROMA_IDC::IDC_444, 10>;

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
        Y410(size_t w, size_t h, size_t padding = 0) : Frame(w, h, padding) {}

        size_t FrameSize(bool padded) const override
        {
            return (padded ? m_wPadded * m_hPadded : m_w * m_h) * sizeof(Pixel);
        }

        CHROMA_IDC GetChromaIDC() const override
        {
            return CHROMA_IDC::IDC_444;
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
        Y210(size_t w, size_t h, size_t padding = 0) : Frame(w, h, padding) {}

        size_t FrameSize(bool padded) const override
        {
            return (padded ? m_wPadded * m_hPadded : m_w * m_h) * sizeof(Pixel);
        }

        CHROMA_IDC GetChromaIDC() const override
        {
            return CHROMA_IDC::IDC_422;
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
                m_raw.Y[i + skipped] = p[i].Y;
                if ((i + skipped) % 2 == 0)
                {
                    m_raw.U[(i + skipped) / 2] = p[i].Chroma;
                }
                else
                {
                    m_raw.V[(i + skipped) / 2] = p[i].Chroma;
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
                p[i].Y = m_raw.Y[i];
                if (i % 2 == 0)
                {
                    p[i].Chroma = m_raw.U[i / 2];
                }
                else
                {
                    p[i].Chroma = m_raw.V[i / 2];
                }
            }
        }
    };

    Frame* CreateFrame(FOURCC fourcc, size_t w, size_t h, size_t padding)
    {
        switch (fourcc)
        {
        case FOURCC::NV12:
            return new NV12(w, h, padding);
        case FOURCC::P010:
            return new P010(w, h, padding);
        case FOURCC::Y410:
            return new Y410(w, h, padding);
        case FOURCC::YUV44410P:
            return new YUV44410P(w, h, padding);
        case FOURCC::YUV4208P:
            return new YUV4208P(w, h, padding);
        case FOURCC::Y210:
            return new Y210(w, h, padding);
        case FOURCC::UNDEF:
        default:
            return nullptr;
        }
    }
}

#pragma pack(pop)
