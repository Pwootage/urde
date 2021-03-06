#ifndef __RSTL_HPP__
#define __RSTL_HPP__

#include <vector>
#include <stdlib.h>
#include "optional.hpp"

namespace rstl
{

template <typename T>
using optional_object = std::experimental::optional<T>;

/**
 * @brief Vector reserved on construction
 */
template <class T, size_t N>
class reserved_vector : public std::vector<T>
{
public:
    reserved_vector() { this->reserve(N); }
    reserved_vector(size_t n, const T& val) : std::vector<T>(n, val) {}
};

template <class T, size_t N>
class prereserved_vector
{
    size_t x0_size = 1;
    T x4_data[N];

public:
    void set_size(size_t n)
    {
        if (n <= N)
            x0_size = n;
    }

    void set_data(const T* data) { memmove(x4_data, data, sizeof(T) * x0_size); }

    size_t size() const { return x0_size; }

    T& back() const { x4_data[(x0_size == 0) ? 0 : x0_size - 1]; }
    T& front() const { return x4_data[0]; }
};
}

#endif // __RSTL_HPP__
