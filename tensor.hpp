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

template<template<typename T, int... N> typename Concrete, typename T, int... N>
struct tensor_base;

template<typename T, int... N>
struct tensor;

template<template<typename T, int... N> typename Concrete, typename U, typename T, int... N>
inline
Concrete<T, N...> tensor_for_each_binary(const Concrete<T, N...>& v1, const Concrete<T, N...>& v2, U&& u);

template<template<typename T, int... N> typename Concrete, typename U, typename T, int... N>
inline
Concrete<T, N...> tensor_for_each_unary(const Concrete<T, N...>& v, U&& u);

template<template<typename T, int... N> typename Concrete, typename T, int... N>
struct tensor_base
{
    template<typename U, int... M>
    using concrete_t = Concrete<U, M...>;

    template<typename U>
    using value_t = T;

    md_array<T, N...> data;

    Concrete<T, N...> to_concrete()
    {
        return *this;
    }

    tensor<T, N...> to_tensor()
    {
        tensor<T, N...> ret;

        ret.data = data;

        return ret;
    }

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

    Concrete<T, N...> unit_invert() const
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

        Concrete<T, N...> ret;

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

    Concrete<T, N...> invert() const
    {
        assert(sizeof...(N) == 2);
        assert((((N == 3) && ...)) || ((N == 4) && ...));

        if constexpr(((N == 3) && ...))
        {
            T d = 1/det();

            Concrete<T, N...> ret = unit_invert();

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
            Concrete<T, N...> ret;

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

    friend Concrete<T, N...> operator+(const Concrete<T, N...>& t1, const Concrete<T, N...>& t2)
    {
        return tensor_for_each_binary(t1, t2, [](const T& v1, const T& v2){return v1 + v2;});
    }

    friend Concrete<T, N...>& operator+=(Concrete<T, N...>& t1, const Concrete<T, N...>& t2)
    {
        t1 = t1 + t2;

        return t1;
    }

    friend Concrete<T, N...> operator-(const Concrete<T, N...>& t1, const Concrete<T, N...>& t2)
    {
        return tensor_for_each_binary(t1, t2, [](const T& v1, const T& v2){return v1 - v2;});
    }

    friend Concrete<T, N...> operator*(const Concrete<T, N...>& t1, const Concrete<T, N...>& t2)
    {
        return tensor_for_each_binary(t1, t2, [](const T& v1, const T& v2){return v1 * v2;});
    }

    friend Concrete<T, N...> operator/(const Concrete<T, N...>& t1, const Concrete<T, N...>& t2)
    {
        return tensor_for_each_binary(t1, t2, [](const T& v1, const T& v2){return v1 / v2;});
    }

    friend Concrete<T, N...> operator+(const Concrete<T, N...>& t1, const T& v2)
    {
        return tensor_for_each_unary(t1, [&](const T& v1){return v1 + v2;});
    }

    friend Concrete<T, N...> operator-(const Concrete<T, N...>& t1, const T& v2)
    {
        return tensor_for_each_unary(t1, [&](const T& v1){return v1 - v2;});
    }

    friend Concrete<T, N...> operator*(const Concrete<T, N...>& t1, const T& v2)
    {
        return tensor_for_each_unary(t1, [&](const T& v1){return v1 * v2;});
    }

    friend Concrete<T, N...> operator/(const Concrete<T, N...>& t1, const T& v2)
    {
        return tensor_for_each_unary(t1, [&](const T& v1){return v1 / v2;});
    }

    friend Concrete<T, N...> operator+(const T& v1, const Concrete<T, N...>& t2)
    {
        return tensor_for_each_unary(t2, [&](const T& v2){return v1 + v2;});
    }

    friend Concrete<T, N...> operator-(const T& v1, const Concrete<T, N...>& t2)
    {
        return tensor_for_each_unary(t2, [&](const T& v2){return v1 - v2;});
    }

    friend Concrete<T, N...> operator*(const T& v1, const Concrete<T, N...>& t2)
    {
        return tensor_for_each_unary(t2, [&](const T& v2){return v1 * v2;});
    }

    friend Concrete<T, N...> operator/(const T& v1, const Concrete<T, N...>& t2)
    {
        return tensor_for_each_unary(t2, [&](const T& v2){return v1 / v2;});
    }

    friend Concrete<T, N...> operator-(const Concrete<T, N...>& t1)
    {
        return tensor_for_each_unary(t1, [](const T& v1){return -v1;});
    }
};

template<typename T, int... N>
struct tensor : tensor_base<tensor, T, N...>
{

};

template<typename TestTensor, typename T, int... N>
concept SizedTensor = std::is_base_of_v<tensor_base<TestTensor::template concrete_t, T, N...>, TestTensor>;

template<typename T, int N>
inline
T sum_multiply(const tensor<T, N>& t1, const tensor<T, N>& t2)
{
    T ret = 0;

    for(int i=0; i < N; i++)
    {
        ret += t1.idx(i) * t2.idx(i);
    }

    return ret;
}

template<typename T, int N>
inline
T sum_multiply(const tensor<T, N, N>& t1, const tensor<T, N, N>& t2)
{
    T ret = 0;

    for(int i=0; i < N; i++)
    {
        for(int j=0; j < N; j++)
        {
            ret += t1.idx(i, j) * t2.idx(i, j);
        }
    }

    return ret;
}

template<typename T, int N>
inline
T sum(const tensor<T, N>& t1)
{
    T ret = 0;

    for(int i=0; i < N; i++)
    {
        ret = ret + t1.idx(i);
    }

    return ret;
}

template<int N, int... M>
inline
int get_first_of()
{
    return N;
}

template<int N, int... M>
inline
int get_second_of()
{
    return get_first_of<M...>();
}

template<int N, int... M>
inline
int get_third_of()
{
    return get_second_of<M...>();
}

template<template<typename T, int... N> typename Concrete, typename U, typename T, int... N>
inline
Concrete<T, N...> tensor_for_each_unary(const Concrete<T, N...>& v, U&& u)
{
    Concrete<T, N...> ret;

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

template<template<typename T, int... N> typename Concrete, typename U, typename T, int... N>
inline
Concrete<T, N...> tensor_for_each_binary(const Concrete<T, N...>& v1, const Concrete<T, N...>& v2, U&& u)
{
    Concrete<T, N...> ret;

    if constexpr(sizeof...(N) == 1)
    {
        int len = get_first_of<N...>();

        for(int i=0; i < len; i++)
        {
            ret.idx(i) = u(v1.idx(i), v2.idx(i));
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
                ret.idx(i, j) = u(v1.idx(i, j), v2.idx(i, j));
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
                    ret.idx(i, j, k) = u(v1.idx(i, j, k), v2.idx(i, j, k));
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

template<int... Indices>
struct tensor_indices
{
    std::array<int, sizeof...(Indices)> indices = {Indices...};
};

#if 0
template<typename T, int... N, int... M, int... N1, int... M1>
inline
auto sum_multiply_fat(const tensor<T, N...>& t1, const tensor<T, M...>& t2, const tensor_indices<N1...>& b1, const tensor_indices<M1...>& b2)
{
    constexpr int total_dimensionality = sizeof...(N) + sizeof...(M);
    constexpr int argument_summation = sizeof...(N1) + sizeof...(M1);

    constexpr int return_dimensions = total_dimensionality - argument_summation;

    ///need to turn return_dimensions into a parameter pack of return_dimensions long, where each element has a value of the components of N...

    //tensor<T, return_dimension>
}
#endif // 0

template<typename T, int... N>
struct inverse_metric : tensor_base<inverse_metric, T, N...>
{

};

template<template<typename T, int... N> typename Concrete, typename T, int... N>
struct metric_base : tensor_base<Concrete, T, N...>
{
    virtual inverse_metric<T, N...> invert() const
    {
        inverse_metric<T, N...> r;
        r.data = tensor_base<Concrete, T, N...>::invert().data;

        return r;
    }
};

template<typename T, int... N>
struct metric : metric_base<metric, T, N...>
{

};

template<typename T, int... N>
struct unit_metric : metric<T, N...>
{
    virtual inverse_metric<T, N...> invert() const override
    {
        inverse_metric<T, N...> r;
        r.data = tensor_base<metric, T, N...>::unit_invert().data;

        return r;
    }
};

template<typename TestTensor, typename T, int... N>
concept MetricTensor = std::is_base_of_v<metric_base<TestTensor::template concrete_t, T, N...>, TestTensor>;

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
