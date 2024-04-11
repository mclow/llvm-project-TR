//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP___TYPE_TRAITS_IS_TRIVIALLY_RELOCATABLE_H
#define _LIBCPP___TYPE_TRAITS_IS_TRIVIALLY_RELOCATABLE_H

#include <__config>
#include <__type_traits/enable_if.h>
#include <__type_traits/integral_constant.h>
#include <__type_traits/is_same.h>
#include <__type_traits/is_trivially_copyable.h>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

_LIBCPP_BEGIN_NAMESPACE_STD

template <class _Tp, class = void>
struct __libcpp_is_trivially_relocatable : integral_constant<bool, __is_cpp_trivially_relocatable(_Tp)> {};

template <class _Tp>
struct __libcpp_is_trivially_relocatable<_Tp,
                                         __enable_if_t<is_same<_Tp, typename _Tp::__trivially_relocatable>::value> >
    : true_type {};

template <class _Tp>
struct is_trivially_relocatable : integral_constant<bool, __is_cpp_trivially_relocatable(_Tp)> {};

template <class _Tp>
inline constexpr bool is_trivially_relocatable_v = __is_cpp_trivially_relocatable(_Tp);

_LIBCPP_END_NAMESPACE_STD

#endif // _LIBCPP___TYPE_TRAITS_IS_TRIVIALLY_RELOCATABLE_H
