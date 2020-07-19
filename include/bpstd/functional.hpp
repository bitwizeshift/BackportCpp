////////////////////////////////////////////////////////////////////////////////
/// \file functional.hpp
///
/// \brief This header provides definitions from the C++ header <functional>
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
#ifndef BPSTD_FUNCTIONAL_HPP
#define BPSTD_FUNCTIONAL_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "detail/config.hpp"
#include "type_traits.hpp"
#include "utility.hpp"
#include "detail/invoke.hpp"

#include <functional> // to proxy API

BPSTD_COMPILER_DIAGNOSTIC_PREAMBLE

namespace bpstd {

  /// \brief Invoke the Callable object \p function with the parameters \p args.
  ///
  /// As by \c INVOKE(std::forward<F>(f), std::forward<Args>(args)...)
  ///
  /// \param function Callable object to be invoked
  /// \param args     arguments to pass to \p function
  template <typename Func, typename... Args>
  constexpr invoke_result_t<Func,Args...> invoke(Func&& function, Args&&... args)
    noexcept(is_nothrow_invocable<Func,Args...>::value);

  namespace detail {
    template <typename Fn>
    struct not_fn_t
    {
      Fn fn;

      template <typename...Args>
      inline BPSTD_INLINE_VISIBILITY BPSTD_CPP14_CONSTEXPR
      auto operator()(Args&&... args) &
        noexcept(noexcept(!::bpstd::invoke(fn, ::bpstd::forward<Args>(args)...)))
        -> decltype(!::bpstd::invoke(fn, ::bpstd::forward<Args>(args)...))
      {
        return !::bpstd::invoke(fn, bpstd::forward<Args>(args)...);
      }

      template <typename...Args>
      inline BPSTD_INLINE_VISIBILITY BPSTD_CPP14_CONSTEXPR
      auto operator()(Args&&... args) &&
        noexcept(noexcept(!::bpstd::invoke(std::move(fn), ::bpstd::forward<Args>(args)...)))
        -> decltype(!::bpstd::invoke(std::move(fn), ::bpstd::forward<Args>(args)...))
      {
        return !::bpstd::invoke(std::move(fn), bpstd::forward<Args>(args)...);
      }

      template <typename...Args>
      inline BPSTD_INLINE_VISIBILITY constexpr
      auto operator()(Args&&... args) const&
        noexcept(noexcept(!::bpstd::invoke(fn, ::bpstd::forward<Args>(args)...)))
        -> decltype(!::bpstd::invoke(fn, ::bpstd::forward<Args>(args)...))
      {
        return !::bpstd::invoke(fn, bpstd::forward<Args>(args)...);
      }

      template <typename...Args>
      inline BPSTD_INLINE_VISIBILITY constexpr
      auto operator()(Args&&... args) const&&
        noexcept(noexcept(!::bpstd::invoke(std::move(fn), ::bpstd::forward<Args>(args)...)))
        -> decltype(!::bpstd::invoke(std::move(fn), ::bpstd::forward<Args>(args)...))
      {
        return !::bpstd::invoke(std::move(fn), bpstd::forward<Args>(args)...);
      }
    };
  } // namespace detail

  /// \brief Creates a forwarding call wrapper that returns the negation of the
  ///        callable object it holds.
  ///
  /// \param fn the object from which the Callable object held by the wrapper
  ///           is constructed
  /// \return the negated object
  template <typename Fn>
  constexpr detail::not_fn_t<decay_t<Fn>> not_fn(Fn&& fn);

  //============================================================================
  // struct : plus
  //============================================================================

  template <typename T = void>
  struct plus
  {
    inline BPSTD_INLINE_VISIBILITY constexpr
    T operator()(const T& lhs, const T& rhs)
      const
    {
      return lhs + rhs;
    }
  };

  template <>
  struct plus<void>
  {
    using is_transparent = true_type;

    template <typename T, typename U>
    inline BPSTD_INLINE_VISIBILITY constexpr
    auto operator()(T&& lhs, U&& rhs) const
      -> decltype(::bpstd::forward<T>(lhs) + ::bpstd::forward<U>(rhs))
    {
      return bpstd::forward<T>(lhs) + bpstd::forward<U>(rhs);
    }
  };

  //============================================================================
  // struct : minus
  //============================================================================

  template <typename T = void>
  struct minus
  {
    inline BPSTD_INLINE_VISIBILITY constexpr
    T operator()(const T& lhs, const T& rhs)
      const
    {
      return lhs - rhs;
    }
  };

  template <>
  struct minus<void>
  {
    using is_transparent = true_type;

    template <typename T, typename U>
    inline BPSTD_INLINE_VISIBILITY constexpr
    auto operator()(T&& lhs, U&& rhs) const
      -> decltype(::bpstd::forward<T>(lhs) - ::bpstd::forward<U>(rhs))
    {
      return bpstd::forward<T>(lhs) - bpstd::forward<U>(rhs);
    }
  };

  //============================================================================
  // struct : multiplies
  //============================================================================

  template <typename T = void>
  struct multiplies
  {
    inline BPSTD_INLINE_VISIBILITY constexpr
    T operator()(const T& lhs, const T& rhs)
      const
    {
      return lhs * rhs;
    }
  };

  template <>
  struct multiplies<void>
  {
    using is_transparent = true_type;

    template <typename T, typename U>
    inline BPSTD_INLINE_VISIBILITY constexpr
    auto operator()(T&& lhs, U&& rhs) const
      -> decltype(::bpstd::forward<T>(lhs) * ::bpstd::forward<U>(rhs))
    {
      return bpstd::forward<T>(lhs) * bpstd::forward<U>(rhs);
    }
  };

  //============================================================================
  // struct : divides
  //============================================================================

  template <typename T = void>
  struct divides
  {
    inline BPSTD_INLINE_VISIBILITY constexpr
    T operator()(const T& lhs, const T& rhs)
      const
    {
      return lhs / rhs;
    }
  };

  template <>
  struct divides<void>
  {
    using is_transparent = true_type;

    template <typename T, typename U>
    inline BPSTD_INLINE_VISIBILITY constexpr
    auto operator()(T&& lhs, U&& rhs) const
      -> decltype(::bpstd::forward<T>(lhs) / ::bpstd::forward<U>(rhs))
    {
      return bpstd::forward<T>(lhs) / bpstd::forward<U>(rhs);
    }
  };

  //============================================================================
  // struct : modulus
  //============================================================================

  template <typename T = void>
  struct modulus
  {
    inline BPSTD_INLINE_VISIBILITY constexpr
    T operator()(const T& lhs, const T& rhs)
      const
    {
      return lhs % rhs;
    }
  };

  template <>
  struct modulus<void>
  {
    using is_transparent = true_type;

    template <typename T, typename U>
    inline BPSTD_INLINE_VISIBILITY constexpr
    auto operator()(T&& lhs, U&& rhs) const
      -> decltype(::bpstd::forward<T>(lhs) % ::bpstd::forward<U>(rhs))
    {
      return bpstd::forward<T>(lhs) % bpstd::forward<U>(rhs);
    }
  };

  //============================================================================
  // struct : negate
  //============================================================================

  template <typename T = void>
  struct negate
  {
    inline BPSTD_INLINE_VISIBILITY constexpr
    T operator()(const T& arg)
      const
    {
      return -arg;
    }
  };

  template <>
  struct negate<void>
  {
    using is_transparent = true_type;

    template <typename T>
    inline BPSTD_INLINE_VISIBILITY constexpr
    auto operator()(T&& arg) const
      -> decltype(-::bpstd::forward<T>(arg))
    {
      return -bpstd::forward<T>(arg);
    }
  };

  //============================================================================
  // struct : equal_to
  //============================================================================

  template <typename T = void>
  struct equal_to
  {
    inline BPSTD_INLINE_VISIBILITY constexpr
    bool operator()(const T& lhs, const T& rhs)
      const
    {
      return lhs == rhs;
    }
  };

  template <>
  struct equal_to<void>
  {
    using is_transparent = true_type;

    template <typename T, typename U>
    inline BPSTD_INLINE_VISIBILITY constexpr
    auto operator()(T&& lhs, U&& rhs) const
      -> decltype(::bpstd::forward<T>(lhs) == ::bpstd::forward<U>(rhs))
    {
      return bpstd::forward<T>(lhs) == bpstd::forward<U>(rhs);
    }
  };

  //============================================================================
  // struct : not_equal_to
  //============================================================================

  template <typename T = void>
  struct not_equal_to
  {
    inline BPSTD_INLINE_VISIBILITY constexpr
    bool operator()(const T& lhs, const T& rhs)
      const
    {
      return lhs != rhs;
    }
  };

  template <>
  struct not_equal_to<void>
  {
    using is_transparent = true_type;

    template <typename T, typename U>
    inline BPSTD_INLINE_VISIBILITY constexpr
    auto operator()(T&& lhs, U&& rhs) const
      -> decltype(::bpstd::forward<T>(lhs) != ::bpstd::forward<U>(rhs))
    {
      return bpstd::forward<T>(lhs) != bpstd::forward<U>(rhs);
    }
  };

  //============================================================================
  // struct : greater
  //============================================================================

  template <typename T = void>
  struct greater
  {
    inline BPSTD_INLINE_VISIBILITY constexpr
    bool operator()(const T& lhs, const T& rhs)
      const
    {
      return lhs > rhs;
    }
  };

  template <>
  struct greater<void>
  {
    using is_transparent = true_type;

    template <typename T, typename U>
    inline BPSTD_INLINE_VISIBILITY constexpr
    auto operator()(T&& lhs, U&& rhs) const
      -> decltype(::bpstd::forward<T>(lhs) > ::bpstd::forward<U>(rhs))
    {
      return bpstd::forward<T>(lhs) > bpstd::forward<U>(rhs);
    }
  };

  //============================================================================
  // struct : greater_equal
  //============================================================================

  template <typename T = void>
  struct greater_equal
  {
    inline BPSTD_INLINE_VISIBILITY constexpr
    bool operator()(const T& lhs, const T& rhs)
      const
    {
      return lhs >= rhs;
    }
  };

  template <>
  struct greater_equal<void>
  {
    using is_transparent = true_type;

    template <typename T, typename U>
    inline BPSTD_INLINE_VISIBILITY constexpr
    auto operator()(T&& lhs, U&& rhs) const
      -> decltype(::bpstd::forward<T>(lhs) >= ::bpstd::forward<U>(rhs))
    {
      return bpstd::forward<T>(lhs) >= bpstd::forward<U>(rhs);
    }
  };

  //============================================================================
  // struct : less
  //============================================================================

  template <typename T = void>
  struct less
  {
    inline BPSTD_INLINE_VISIBILITY constexpr
    bool operator()(const T& lhs, const T& rhs)
      const
    {
      return lhs < rhs;
    }
  };

  template <>
  struct less<void>
  {
    using is_transparent = true_type;

    template <typename T, typename U>
    inline BPSTD_INLINE_VISIBILITY constexpr
    auto operator()(T&& lhs, U&& rhs) const
      -> decltype(::bpstd::forward<T>(lhs) < ::bpstd::forward<U>(rhs))
    {
      return bpstd::forward<T>(lhs) < bpstd::forward<U>(rhs);
    }
  };

  //============================================================================
  // struct : less_equal
  //============================================================================

  template <typename T = void>
  struct less_equal
  {
    inline BPSTD_INLINE_VISIBILITY constexpr
    bool operator()(const T& lhs, const T& rhs)
      const
    {
      return lhs <= rhs;
    }
  };

  template <>
  struct less_equal<void>
  {
    using is_transparent = true_type;

    template <typename T, typename U>
    inline BPSTD_INLINE_VISIBILITY constexpr
    auto operator()(T&& lhs, U&& rhs) const
      -> decltype(::bpstd::forward<T>(lhs) <= ::bpstd::forward<U>(rhs))
    {
      return bpstd::forward<T>(lhs) <= bpstd::forward<U>(rhs);
    }
  };

  //============================================================================
  // struct : logical_and
  //============================================================================

  template <typename T = void>
  struct logical_and
  {
    inline BPSTD_INLINE_VISIBILITY constexpr
    bool operator()(const T& lhs, const T& rhs)
      const
    {
      return lhs && rhs;
    }
  };

  template <>
  struct logical_and<void>
  {
    using is_transparent = true_type;

    template <typename T, typename U>
    inline BPSTD_INLINE_VISIBILITY constexpr
    auto operator()(T&& lhs, U&& rhs) const
      -> decltype(::bpstd::forward<T>(lhs) && ::bpstd::forward<U>(rhs))
    {
      return bpstd::forward<T>(lhs) && bpstd::forward<U>(rhs);
    }
  };

  //============================================================================
  // struct : logical_or
  //============================================================================

  template <typename T = void>
  struct logical_or
  {
    inline BPSTD_INLINE_VISIBILITY constexpr
    bool operator()(const T& lhs, const T& rhs)
      const
    {
      return lhs || rhs;
    }
  };

  template <>
  struct logical_or<void>
  {
    using is_transparent = true_type;

    template <typename T, typename U>
    inline BPSTD_INLINE_VISIBILITY constexpr
    auto operator()(T&& lhs, U&& rhs) const
      -> decltype(::bpstd::forward<T>(lhs) || ::bpstd::forward<U>(rhs))
    {
      return bpstd::forward<T>(lhs) || bpstd::forward<U>(rhs);
    }
  };

  //============================================================================
  // struct : logical_not
  //============================================================================

  template <typename T = void>
  struct logical_not
  {
    inline BPSTD_INLINE_VISIBILITY constexpr
    bool operator()(const T& arg)
      const
    {
      return !arg;
    }
  };

  template <>
  struct logical_not<void>
  {
    using is_transparent = true_type;

    template <typename T>
    inline BPSTD_INLINE_VISIBILITY constexpr
    auto operator()(T&& arg) const
      -> decltype(!::bpstd::forward<T>(arg))
    {
      return !bpstd::forward<T>(arg);
    }
  };

  //============================================================================
  // struct : bit_and
  //============================================================================

  template <typename T = void>
  struct bit_and
  {
    inline BPSTD_INLINE_VISIBILITY constexpr
    T operator()(const T& lhs, const T& rhs)
      const
    {
      return lhs & rhs;
    }
  };

  template <>
  struct bit_and<void>
  {
    using is_transparent = true_type;

    template <typename T, typename U>
    inline BPSTD_INLINE_VISIBILITY constexpr
    auto operator()(T&& lhs, U&& rhs) const
      -> decltype(::bpstd::forward<T>(lhs) & ::bpstd::forward<U>(rhs))
    {
      return bpstd::forward<T>(lhs) & bpstd::forward<U>(rhs);
    }
  };

  //============================================================================
  // struct : bit_or
  //============================================================================

  template <typename T = void>
  struct bit_or
  {
    inline BPSTD_INLINE_VISIBILITY constexpr
    T operator()(const T& lhs, const T& rhs)
      const
    {
      return lhs | rhs;
    }
  };

  template <>
  struct bit_or<void>
  {
    using is_transparent = true_type;

    template <typename T, typename U>
    inline BPSTD_INLINE_VISIBILITY constexpr
    auto operator()(T&& lhs, U&& rhs) const
      -> decltype(::bpstd::forward<T>(lhs) | ::bpstd::forward<U>(rhs))
    {
      return bpstd::forward<T>(lhs) | bpstd::forward<U>(rhs);
    }
  };

  //============================================================================
  // struct : bit_xor
  //============================================================================

  template <typename T = void>
  struct bit_xor
  {
    inline BPSTD_INLINE_VISIBILITY constexpr
    T operator()(const T& lhs, const T& rhs)
      const
    {
      return lhs ^ rhs;
    }
  };

  template <>
  struct bit_xor<void>
  {
    using is_transparent = true_type;

    template <typename T, typename U>
    inline BPSTD_INLINE_VISIBILITY constexpr
    auto operator()(T&& lhs, U&& rhs) const
      -> decltype(::bpstd::forward<T>(lhs) ^ ::bpstd::forward<U>(rhs))
    {
      return bpstd::forward<T>(lhs) ^ bpstd::forward<U>(rhs);
    }
  };

  //============================================================================
  // struct : bit_not
  //============================================================================

  template <typename T = void>
  struct bit_not
  {
    inline BPSTD_INLINE_VISIBILITY constexpr
    T operator()(const T& arg)
      const
    {
      return ~arg;
    }
  };

  template <>
  struct bit_not<void>
  {
    using is_transparent = true_type;

    template <typename T>
    inline BPSTD_INLINE_VISIBILITY constexpr
    auto operator()(T&& arg) const
      -> decltype(~::bpstd::forward<T>(arg))
    {
      return ~bpstd::forward<T>(arg);
    }
  };

} // namespace bpstd

//==============================================================================
// definition : invoke
//==============================================================================

template <typename Func, typename... Args>
inline BPSTD_INLINE_VISIBILITY constexpr
bpstd::invoke_result_t<Func,Args...> bpstd::invoke(Func&& function, Args&&... args)
  noexcept(is_nothrow_invocable<Func,Args...>::value)
{
  return detail::INVOKE(bpstd::forward<Func>(function), bpstd::forward<Args>(args)...);
}


//==============================================================================
// definition : not_fn
//==============================================================================

template <typename Fn>
inline BPSTD_INLINE_VISIBILITY constexpr
bpstd::detail::not_fn_t<bpstd::decay_t<Fn>> bpstd::not_fn(Fn&& fn)
{
  static_assert(
    is_move_constructible<Fn>::value,
    "Fn must be move constructible"
  );
  static_assert(
    is_constructible<decay_t<Fn>,Fn>::value,
    "Fn must be constructible from an instance of fn"
  );

  return { bpstd::forward<Fn>(fn) };
}

BPSTD_COMPILER_DIAGNOSTIC_POSTAMBLE

#endif /* BPSTD_FUNCTIONAL_HPP */
