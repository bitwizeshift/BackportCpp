////////////////////////////////////////////////////////////////////////////////
/// \file chrono.hpp
///
/// \brief This header provides definitions from the C++ header <chrono>
////////////////////////////////////////////////////////////////////////////////

/*
  The MIT License (MIT)

  Copyright (c) 2020 Matthew Rodusek All rights reserved.

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/
#ifndef BPSTD_CHRONO_HPP
#define BPSTD_CHRONO_HPP

#include "detail/config.hpp"

#include <chrono>  // std::chrono::duration, std::chrono::system_clock, etc
#include <cstdint> // std::int32_t

BPSTD_COMPILER_DIAGNOSTIC_PREAMBLE

namespace bpstd {
  namespace chrono {

    template <typename Rep, typename Period = std::ratio<1>>
    using duration = std::chrono::duration<Rep,Period>;

    template <typename Clock, typename Duration = typename Clock::duration>
    using time_point = std::chrono::time_point<Clock, Duration>;

    using nanoseconds  = std::chrono::nanoseconds;
    using microseconds = std::chrono::microseconds;
    using milliseconds = std::chrono::milliseconds;
    using seconds      = std::chrono::seconds;
    using minutes      = std::chrono::minutes;
    using hours        = std::chrono::hours;
    using days         = std::chrono::duration<std::int32_t, std::ratio<86400>>;
    using weeks        = std::chrono::duration<std::int32_t, std::ratio<604800>>;
    using months       = std::chrono::duration<std::int32_t, std::ratio<2629746>>;
    using years        = std::chrono::duration<std::int32_t, std::ratio<31556952>>;

    using steady_clock = std::chrono::steady_clock;
    using system_clock = std::chrono::system_clock;

    template <typename Duration>
    using sys_time    = time_point<system_clock, Duration>;
    using sys_seconds = sys_time<seconds>;
    using sys_days    = sys_time<days>;


  } // namespace chrono

  //============================================================================
  // non-member functions
  //============================================================================

  //----------------------------------------------------------------------------
  // Literals
  //----------------------------------------------------------------------------

  inline namespace literals {
    inline namespace chrono_literals {

      constexpr std::chrono::hours operator""_h(unsigned long long x);
      constexpr std::chrono::duration<double, std::ratio<3600,1>> operator""_h(long double x);

      constexpr chrono::minutes operator""_min(unsigned long long x);
      constexpr chrono::duration<double, std::ratio<60>> operator""_min(long double x);

      constexpr chrono::seconds operator""_s(unsigned long long x);
      constexpr chrono::duration<double> operator""_s(long double x);

      constexpr chrono::milliseconds operator""_ms(unsigned long long x);
      constexpr chrono::duration<double, std::milli> operator""_ms(long double x);

      constexpr chrono::microseconds operator""_us(unsigned long long x);
      constexpr chrono::duration<double, std::micro> operator""_us(long double x);

      constexpr chrono::nanoseconds operator""_ns(unsigned long long x);
      constexpr chrono::duration<double, std::nano> operator""_ns(long double x);

    } // inline namespace chrono_literals
  } // inline namespace chrono
} // namespace bpstd

inline BPSTD_INLINE_VISIBILITY constexpr
std::chrono::hours
  bpstd::literals::chrono_literals::operator""_h(unsigned long long x)
{
  return chrono::hours{x};
}

inline BPSTD_INLINE_VISIBILITY constexpr
std::chrono::duration<double, std::ratio<3600,1>>
  bpstd::literals::chrono_literals::operator""_h(long double x)
{
  return chrono::duration<double, std::ratio<3600,1>>{x};
}

inline BPSTD_INLINE_VISIBILITY constexpr
bpstd::chrono::minutes
  bpstd::literals::chrono_literals::operator""_min(unsigned long long x)
{
  return chrono::minutes{x};
}
inline BPSTD_INLINE_VISIBILITY constexpr
bpstd::chrono::duration<double, std::ratio<60>>
  bpstd::literals::chrono_literals::operator""_min(long double x)
{
  return chrono::duration<double, std::ratio<60>>{x};
}

inline BPSTD_INLINE_VISIBILITY constexpr
bpstd::chrono::seconds
  bpstd::literals::chrono_literals::operator""_s(unsigned long long x)
{
  return chrono::seconds{x};
}

inline BPSTD_INLINE_VISIBILITY constexpr
bpstd::chrono::duration<double>
  bpstd::literals::chrono_literals::operator""_s(long double x)
{
  return chrono::duration<double>{x};
}

inline BPSTD_INLINE_VISIBILITY constexpr
bpstd::chrono::milliseconds
  bpstd::literals::chrono_literals::operator""_ms(unsigned long long x)
{
  return chrono::milliseconds{x};
}

inline BPSTD_INLINE_VISIBILITY constexpr
bpstd::chrono::duration<double, std::milli>
  bpstd::literals::chrono_literals::operator""_ms(long double x)
{
  return chrono::duration<double, std::milli>{x};
}

inline BPSTD_INLINE_VISIBILITY constexpr
bpstd::chrono::microseconds
  bpstd::literals::chrono_literals::operator ""_us(unsigned long long x)
{
  return chrono::microseconds{x};
}

inline BPSTD_INLINE_VISIBILITY constexpr
bpstd::chrono::duration<double, std::micro>
  bpstd::literals::chrono_literals::operator""_us(long double x)
{
  return chrono::duration<double, std::micro>{x};
}

inline BPSTD_INLINE_VISIBILITY constexpr
bpstd::chrono::nanoseconds
  bpstd::literals::chrono_literals::operator""_ns(unsigned long long x)
{
  return chrono::nanoseconds{x};
}

inline BPSTD_INLINE_VISIBILITY constexpr
bpstd::chrono::duration<double, std::nano>
  bpstd::literals::chrono_literals::operator""_ns(long double x)
{
  return chrono::duration<double, std::nano>{x};
}

BPSTD_COMPILER_DIAGNOSTIC_POSTAMBLE

#endif /* BPSTD_CHRONO_HPP */
