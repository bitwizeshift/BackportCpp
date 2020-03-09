////////////////////////////////////////////////////////////////////////////////
/// \file enable_overload.hpp
///
/// \brief This internal header provides the definition of a SFINAE utility
///        for conditionally enabling overloads
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
#ifndef BPSTD_DETAIL_ENABLE_OVERLOAD_HPP
#define BPSTD_DETAIL_ENABLE_OVERLOAD_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

namespace bpstd {
  namespace detail {

    /// \brief Similar to enable_if, but doesn't sfinae-away a type; instead
    ///        produces an uninstantiable unique type when true
    ///
    /// This is used to selectively disable constructors, since sfinae doesn't
    /// work for copy/move constructors
    template <bool b, typename T>
    struct enable_overload_if
    {
      using type = T;
    };

    template <typename T>
    struct enable_overload_if<false,T>
    {
      class type{ type() = delete; ~type() = delete; };
    };

    template <bool B, typename T>
    using enable_overload_if_t = typename enable_overload_if<B,T>::type;

    /// \brief Inverse of enable_overload_if
    template <bool B, typename T>
    using disable_overload_if = enable_overload_if<!B,T>;

    /// \brief Convenience alias to retrieve the ::type member of
    ///        disable_overload_if
    template <bool B, typename T>
    using disable_overload_if_t = typename disable_overload_if<B,T>::type;

  } // namespace detail
} // namespace bpstd

#endif /* BPSTD_DETAIL_ENABLE_OVERLOAD_HPP */
