#pragma once

#include <cstdint>
#include <vector>

#pragma pack(push, 1)

namespace frame
{
    struct Raw
    {
        using value_t = uint64_t;

        std::vector<value_t> A;
        std::vector<value_t> Y;
        std::vector<value_t> U;
        std::vector<value_t> V;
    };

    class Frame
    {
    public:
        Frame(size_t w, size_t h) : m_w(w), m_h(h) {}
        virtual ~Frame() = default;

        virtual size_t FrameSize() const = 0;
        virtual void ReadFrame(const void* data) = 0;
        virtual void WriteFrame(void* data) const = 0;

    protected:
        size_t m_w = 0;
        size_t m_h = 0;
        Raw m_raw;
    };

    class Y410 : public Frame
    {
    protected:
        struct Pixel
        {
            uint32_t U : 10;
            uint32_t Y : 10;
            uint32_t V : 10;
            uint32_t A : 2;
        };

    public:
        Y410(size_t w, size_t h) : Frame(w, h)
        {
            auto numPixel = w * h;

            m_raw.A.resize(numPixel, 0);
            m_raw.Y.resize(numPixel, 0);
            m_raw.U.resize(numPixel, 0);
            m_raw.V.resize(numPixel, 0);
        }

        size_t FrameSize() const override
        {
            return m_raw.A.size() * sizeof(Pixel);
        }

        void ReadFrame(const void* data) override
        {
            auto p = reinterpret_cast<const Pixel*>(data);
            for (size_t i = 0; i < m_raw.A.size(); ++i)
            {
                m_raw.A[i] = p[i].A;
                m_raw.Y[i] = p[i].Y;
                m_raw.U[i] = p[i].U;
                m_raw.V[i] = p[i].V;
            }
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

        void WriteFrameP010(void* data) const
        {
            auto planeY = reinterpret_cast<uint16_t*>(data);
            auto planeUV = planeY + m_raw.Y.size();
            for (size_t i = 0; i < m_raw.Y.size(); ++i)
            {
                *planeY++ = m_raw.Y[i] << 6;
                if (i / m_w % 2 == 0 && i % m_w % 2 == 0)
                {
                    *planeUV++ = (m_raw.U[i] + m_raw.U[i + 1] + m_raw.U[i + m_w] + m_raw.U[i + 1 + m_w]) << 4;
                    *planeUV++ = (m_raw.V[i] + m_raw.V[i + 1] + m_raw.V[i + m_w] + m_raw.V[i + 1 + m_w]) << 4;
                }
            }
        }
    };

    class P010 : public Frame
    {
    public:
        P010(size_t w, size_t h) : Frame(w, h)
        {
            auto numPixel = w * h;

            m_raw.Y.resize(numPixel, 0);
            m_raw.U.resize(numPixel / 4, 0);
            m_raw.V.resize(numPixel / 4, 0);
        }

        void ReadFrame(const void* data) override
        {
            auto p = reinterpret_cast<const uint16_t*>(data);
            for (size_t i = 0; i < m_raw.Y.size(); ++i)
            {
                m_raw.Y[i] = p[i] >> 6;
            }

            p += m_raw.Y.size();
            for (size_t i = 0; i < m_raw.U.size(); i += 2)
            {
                m_raw.U[i] = p[i] >> 6;
                m_raw.V[i] = p[i + 1] >> 6;
            }
        }

        void WriteFrame(void* data) const override
        {
            auto p = reinterpret_cast<uint16_t*>(data);
            for (size_t i = 0; i < m_raw.Y.size(); ++i)
            {
                p[i] = m_raw.Y[i] << 6;
            }

            p += m_raw.Y.size();
            for (size_t i = 0; i < m_raw.U.size(); i += 2)
            {
                p[i] = m_raw.U[i] << 6;
                p[i + 1] = m_raw.V[i] << 6;
            }
        }
    };
}

#pragma pack(pop)
