#ifndef TENSOR_HPP_INCLUDED
#define TENSOR_HPP_INCLUDED

#include "vec.hpp"

template<int... N>
constexpr int N_to_size()
{
    return (N * ...);
}

template<typename T, size_t size, size_t... sizes>
struct md_array_impl
{
    using type = std::array<typename md_array_impl<T, sizes...>::type, size>;
};

template<typename T, size_t size>
struct md_array_impl<T, size>
{
    using type = std::array<T, size>;
};

template<typename T, size_t... sizes>
using md_array = typename md_array_impl<T, sizes...>::type;

template<typename T, typename Next>
constexpr
auto& index_md_array(T& arr, Next v)
{
    return arr[v];
}

template<typename T, typename Next, typename... Rest>
constexpr
auto& index_md_array(T& arr, Next v, Rest... r)
{
    return index_md_array(arr[v], r...);
}

/*
void metric_inverse(const float m[16], float invOut[16])
{
    float inv[16], det;
    int i;

    inv[0] = m[5] * m[10] * m[15] -
             m[5] * m[11] * m[11] -
             m[6] * m[6]  * m[15] +
             m[6] * m[7]  * m[11] +
             m[7] * m[6]  * m[11] -
             m[7] * m[7]  * m[10];

    inv[1] = -m[1] * m[10] * m[15] +
              m[1] * m[11] * m[11] +
              m[6] * m[2] * m[15] -
              m[6] * m[3] * m[11] -
              m[7] * m[2] * m[11] +
              m[7] * m[3] * m[10];

    inv[5] = m[0] * m[10] * m[15] -
             m[0] * m[11] * m[11] -
             m[2] * m[2] * m[15] +
             m[2] * m[3] * m[11] +
             m[3] * m[2] * m[11] -
             m[3] * m[3] * m[10];


    inv[2] = m[1] * m[6] * m[15] -
             m[1] * m[7] * m[11] -
             m[5] * m[2] * m[15] +
             m[5] * m[3] * m[11] +
             m[7] * m[2] * m[7] -
             m[7] * m[3] * m[6];

    inv[6] = -m[0] * m[6] * m[15] +
              m[0] * m[7] * m[11] +
              m[1] * m[2] * m[15] -
              m[1] * m[3] * m[11] -
              m[3] * m[2] * m[7] +
              m[3] * m[3] * m[6];

    inv[10] = m[0] * m[5] * m[15] -
              m[0] * m[7] * m[7] -
              m[1] * m[1] * m[15] +
              m[1] * m[3] * m[7] +
              m[3] * m[1] * m[7] -
              m[3] * m[3] * m[5];

    inv[3] = -m[1] * m[6] * m[11] +
              m[1] * m[7] * m[10] +
              m[5] * m[2] * m[11] -
              m[5] * m[3] * m[10] -
              m[6] * m[2] * m[7] +
              m[6] * m[3] * m[6];

    inv[7] = m[0] * m[6] * m[11] -
             m[0] * m[7] * m[10] -
             m[1] * m[2] * m[11] +
             m[1] * m[3] * m[10] +
             m[2] * m[2] * m[7] -
             m[2] * m[3] * m[6];

    inv[11] = -m[0] * m[5] * m[11] +
               m[0] * m[7] * m[6] +
               m[1] * m[1] * m[11] -
               m[1] * m[3] * m[6] -
               m[2] * m[1] * m[7] +
               m[2] * m[3] * m[5];

    inv[15] = m[0] * m[5] * m[10] -
              m[0] * m[6] * m[6] -
              m[1] * m[1] * m[10] +
              m[1] * m[2] * m[6] +
              m[2] * m[1] * m[6] -
              m[2] * m[2] * m[5];

    inv[4] = inv[1];
    inv[8] = inv[2];
    inv[12] = inv[3];
    inv[9] = inv[6];
    inv[13] = inv[7];
    inv[14] = inv[11];

    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    det = 1.0 / det;

    for (i = 0; i < 16; i++)
        invOut[i] = inv[i] * det;
}*/

template<typename T, int... N>
struct tensor
{
    md_array<T, N...> data;

    template<typename... V>
    T& idx(V... vals)
    {
        static_assert(sizeof...(V) == sizeof...(N));

        return index_md_array(data, vals...);
    }

    template<typename... V>
    const T& idx(V... vals) const
    {
        static_assert(sizeof...(V) == sizeof...(N));

        return index_md_array(data, vals...);
    }

    T det() const
    {
        assert(sizeof...(N) == 2);
        assert(((N == 3) && ...));

        T a11 = idx(0, 0);
        T a12 = idx(0, 1);
        T a13 = idx(0, 2);

        T a21 = idx(1, 0);
        T a22 = idx(1, 1);
        T a23 = idx(1, 2);

        T a31 = idx(2, 0);
        T a32 = idx(2, 1);
        T a33 = idx(2, 2);

        return a11*a22*a33 + a21*a32*a13 + a31*a12*a23 - a11*a32*a23 - a31*a22*a13 - a21*a12*a33;
    }

    tensor<T, N...> unit_invert() const
    {
        assert(sizeof...(N) == 2);
        assert(((N == 3) && ...));

        T a11 = idx(0, 0);
        T a12 = idx(0, 1);
        T a13 = idx(0, 2);

        T a21 = idx(1, 0);
        T a22 = idx(1, 1);
        T a23 = idx(1, 2);

        T a31 = idx(2, 0);
        T a32 = idx(2, 1);
        T a33 = idx(2, 2);

        T x0 = (a22 * a33 - a23 * a32);
        T y0 = (a13 * a32 - a12 * a33);
        T z0 = (a12 * a23 - a13 * a22);

        T x1 = (a23 * a31 - a21 * a33);
        T y1 = (a11 * a33 - a13 * a31);
        T z1 = (a13 * a21 - a11 * a23);

        T x2 = (a21 * a32 - a22 * a31);
        T y2 = (a12 * a31 - a11 * a32);
        T z2 = (a11 * a22 - a12 * a21);

        tensor<T, N...> ret;

        ret.idx(0, 0) = x0;
        ret.idx(0, 1) = y0;
        ret.idx(0, 2) = z0;
        ret.idx(1, 0) = x1;
        ret.idx(1, 1) = y1;
        ret.idx(1, 2) = z1;
        ret.idx(2, 0) = x2;
        ret.idx(2, 1) = y2;
        ret.idx(2, 2) = z2;

        return ret;
    }

    tensor<T, N...> invert() const
    {
        assert(sizeof...(N) == 2);
        assert((((N == 3) && ...)) || ((N == 4) && ...));

        if constexpr(((N == 3) && ...))
        {
            T d = 1/det();

            tensor<T, N...> ret = unit_invert();

            ret.idx(0, 0) = ret.idx(0, 0) * d;
            ret.idx(0, 1) = ret.idx(0, 1) * d;
            ret.idx(0, 2) = ret.idx(0, 2) * d;
            ret.idx(1, 0) = ret.idx(1, 0) * d;
            ret.idx(1, 1) = ret.idx(1, 1) * d;
            ret.idx(1, 2) = ret.idx(1, 2) * d;
            ret.idx(2, 0) = ret.idx(2, 0) * d;
            ret.idx(2, 1) = ret.idx(2, 1) * d;
            ret.idx(2, 2) = ret.idx(2, 2) * d;

            return ret;
        }

        if constexpr(((N == 4) && ...))
        {
            ///[0, 1, 2, 3]
            ///[4, 5, 6, 7]
            ///[8, 9, 10,11]
            ///[12,13,14,15]

            std::array<T, 16> m;
            m[0] = idx(0, 0);
            m[1] = idx(0, 1);
            m[2] = idx(0, 2);
            m[3] = idx(0, 3);
            m[4] = idx(1, 0);
            m[5] = idx(1, 1);
            m[6] = idx(1, 2);
            m[7] = idx(1, 3);
            m[8] = idx(2, 0);
            m[9] = idx(2, 1);
            m[10] = idx(2, 2);
            m[11] = idx(2, 3);
            m[12] = idx(3, 0);
            m[13] = idx(3, 1);
            m[14] = idx(3, 2);
            m[15] = idx(3, 3);

            std::array<T, 16> inv;

            T det = T();
            tensor<T, N...> ret;

            inv[0] = m[5] * m[10] * m[15] -
                     m[5] * m[11] * m[11] -
                     m[6] * m[6]  * m[15] +
                     m[6] * m[7]  * m[11] +
                     m[7] * m[6]  * m[11] -
                     m[7] * m[7]  * m[10];

            inv[1] = -m[1] * m[10] * m[15] +
                      m[1] * m[11] * m[11] +
                      m[6] * m[2] * m[15] -
                      m[6] * m[3] * m[11] -
                      m[7] * m[2] * m[11] +
                      m[7] * m[3] * m[10];

            inv[5] = m[0] * m[10] * m[15] -
                     m[0] * m[11] * m[11] -
                     m[2] * m[2] * m[15] +
                     m[2] * m[3] * m[11] +
                     m[3] * m[2] * m[11] -
                     m[3] * m[3] * m[10];


            inv[2] = m[1] * m[6] * m[15] -
                     m[1] * m[7] * m[11] -
                     m[5] * m[2] * m[15] +
                     m[5] * m[3] * m[11] +
                     m[7] * m[2] * m[7] -
                     m[7] * m[3] * m[6];

            inv[6] = -m[0] * m[6] * m[15] +
                      m[0] * m[7] * m[11] +
                      m[1] * m[2] * m[15] -
                      m[1] * m[3] * m[11] -
                      m[3] * m[2] * m[7] +
                      m[3] * m[3] * m[6];

            inv[10] = m[0] * m[5] * m[15] -
                      m[0] * m[7] * m[7] -
                      m[1] * m[1] * m[15] +
                      m[1] * m[3] * m[7] +
                      m[3] * m[1] * m[7] -
                      m[3] * m[3] * m[5];

            inv[3] = -m[1] * m[6] * m[11] +
                      m[1] * m[7] * m[10] +
                      m[5] * m[2] * m[11] -
                      m[5] * m[3] * m[10] -
                      m[6] * m[2] * m[7] +
                      m[6] * m[3] * m[6];

            inv[7] = m[0] * m[6] * m[11] -
                     m[0] * m[7] * m[10] -
                     m[1] * m[2] * m[11] +
                     m[1] * m[3] * m[10] +
                     m[2] * m[2] * m[7] -
                     m[2] * m[3] * m[6];

            inv[11] = -m[0] * m[5] * m[11] +
                       m[0] * m[7] * m[6] +
                       m[1] * m[1] * m[11] -
                       m[1] * m[3] * m[6] -
                       m[2] * m[1] * m[7] +
                       m[2] * m[3] * m[5];

            inv[15] = m[0] * m[5] * m[10] -
                      m[0] * m[6] * m[6] -
                      m[1] * m[1] * m[10] +
                      m[1] * m[2] * m[6] +
                      m[2] * m[1] * m[6] -
                      m[2] * m[2] * m[5];

            inv[4] = inv[1];
            inv[8] = inv[2];
            inv[12] = inv[3];
            inv[9] = inv[6];
            inv[13] = inv[7];
            inv[14] = inv[11];

            det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

            det = 1.0f / det;

            for(int x=0; x < 4; x++)
            {
                for(int y=0; y < 4; y++)
                {
                    ret.idx(y, x) = inv[y * 4 + x] * det;
                }
            }

            return ret;
        }

        assert(false);
    }
};

template<int N>
inline
int get_first_of()
{
    return N;
}

template<int... M>
inline
int get_first_of()
{
    return get_first_of<M...>();
}

template<int N, int M>
inline
int get_second_of()
{
    return M;
}

template<int... M>
inline
int get_second_of()
{
    return get_second_of<M...>();
}

template<int N, int M, int O>
inline
int get_third_of()
{
    return O;
}

template<int... M>
inline
int get_third_of()
{
    return get_third_of<M...>();
}

template<typename U, typename T, int... N>
inline
tensor<T, N...> tensor_for_each_unary(const tensor<T, N...>& v, U&& u)
{
    tensor<T, N...> ret;

    if constexpr(sizeof...(N) == 1)
    {
        int len = get_first_of<N...>();

        for(int i=0; i < len; i++)
        {
            ret.idx(i) = u(v.idx(i));
        }
    }
    else if constexpr(sizeof...(N) == 2)
    {
        int l1 = get_first_of<N...>();
        int l2 = get_second_of<N...>();

        for(int i=0; i < l1; i++)
        {
            for(int j=0; j < l2; j++)
            {
                ret.idx(i, j) = u(v.idx(i, j));
            }
        }
    }
    else if constexpr(sizeof...(N) == 3)
    {
        int l1 = get_first_of<N...>();
        int l2 = get_second_of<N...>();
        int l3 = get_third_of<N...>();

        for(int i=0; i < l1; i++)
        {
            for(int j=0; j < l2; j++)
            {
                for(int k=0; k < l3; k++)
                {
                    ret.idx(i, j, k) = u(v.idx(i, j, k));
                }
            }
        }
    }
    else
    {
        assert(false);
    }

    return ret;
}

template<typename T, int... N>
struct inverse_metric : tensor<T, N...>
{

};

template<typename T, int... N>
struct metric : tensor<T, N...>
{
    virtual inverse_metric<T, N...> invert() const
    {
        inverse_metric<T, N...> r;
        r.data = tensor<T, N...>::invert().data;

        return r;
    }
};

template<typename T, int... N>
struct unit_metric : metric<T, N...>
{
    virtual inverse_metric<T, N...> invert() const override
    {
        inverse_metric<T, N...> r;
        r.data = tensor<T, N...>::unit_invert().data;

        return r;
    }
};

/*template<typename T>
vec<3, T> operator*(const mat<3, T> m, const vec<3, T>& other)
{
    vec<3, T> val;

    val.v[0] = m.v[0][0] * other.v[0] + m.v[0][1] * other.v[1] + m.v[0][2] * other.v[2];
    val.v[1] = m.v[1][0] * other.v[0] + m.v[1][1] * other.v[1] + m.v[1][2] * other.v[2];
    val.v[2] = m.v[2][0] * other.v[0] + m.v[2][1] * other.v[1] + m.v[2][2] * other.v[2];

    return val;
}*/

#endif
