//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP___UTILITY_SWAP_H
#define _LIBCPP___UTILITY_SWAP_H

#include <__assert>
#include <__config>
#include <__type_traits/enable_if.h>
#include <__type_traits/is_assignable.h>
#include <__type_traits/is_constructible.h>
#include <__type_traits/is_nothrow_assignable.h>
#include <__type_traits/is_nothrow_constructible.h>
#include <__type_traits/is_swappable.h>
#include <__type_traits/is_trivially_relocatable.h>
#include <__utility/declval.h>
#include <__utility/is_pointer_in_range.h>
#include <__utility/move.h>
#include <cstddef>
#include <typeinfo>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

#ifndef _LIBCPP_CXX03_LANG
template <class _Tp>
using __swap_result_t = __enable_if_t<is_move_constructible<_Tp>::value && is_move_assignable<_Tp>::value>;
#else
template <class>
using __swap_result_t = void;
#endif

#if _LIBCPP_STD_VER >= 26
template <class _Tp>
  requires std::is_trivially_relocatable_v<_Tp> && std::is_replaceable_v<_Tp>
void swap_value_representations(_Tp& __a, _Tp& __b) noexcept {
  char* __aptr = reinterpret_cast<char*>(&__a);
  char* __bptr = reinterpret_cast<char*>(&__b);

  // If T is a polymorphic type, then T is the most derived type for the objects a and b.
  _LIBCPP_ASSERT_ARGUMENT_WITHIN_DOMAIN(typeid(__a) == typeid(_Tp), "the dynamic type of a is not that of T");
  _LIBCPP_ASSERT_ARGUMENT_WITHIN_DOMAIN(typeid(__b) == typeid(_Tp), "the dynamic type of b is not that of T");

  _LIBCPP_ASSERT_ARGUMENT_WITHIN_DOMAIN(
      !__is_pointer_in_range(__aptr, __aptr + __datasizeof(_Tp), __bptr) &&
          !__is_pointer_in_range(__bptr, __bptr + __datasizeof(_Tp), __aptr),
      "Supporting overlapping objects would prevent optimizations");

  // TODO: research better memswap algorithms
  constexpr size_t __size  = __datasizeof(_Tp) < 256 ? __datasizeof(_Tp) : 256;
  constexpr size_t __chunk = __datasizeof(_Tp) / __size;
  constexpr size_t __rem   = __datasizeof(_Tp) % __size;
  char __buffer[__size];
  if constexpr (__chunk) {
    for (std::size_t n = 0; n < __chunk; n++, __aptr += __size, __bptr += __size) {
      __builtin_memmove(__buffer, __aptr, __size);
      __builtin_memmove(__aptr, __bptr, __size);
      __builtin_memmove(__bptr, __buffer, __size);
    }
  }
  if constexpr (__rem) {
    __builtin_memmove(__buffer, __aptr, __rem);
    __builtin_memmove(__aptr, __bptr, __rem);
    __builtin_memmove(__bptr, __buffer, __rem);
  }
}
#endif

template <class _Tp>
inline _LIBCPP_HIDE_FROM_ABI __swap_result_t<_Tp> _LIBCPP_CONSTEXPR_SINCE_CXX20 swap(_Tp& __x, _Tp& __y)
    _NOEXCEPT_(is_nothrow_move_constructible<_Tp>::value&& is_nothrow_move_assignable<_Tp>::value) {
#if _LIBCPP_STD_VER >= 26
  if !consteval {
    if constexpr (!__is_scalar(_Tp) && std::is_trivially_relocatable_v<_Tp> && std::is_replaceable_v<_Tp> &&
                  sizeof(_Tp) > sizeof(void*)) {
      bool __is_eligible = true;
      if constexpr (__is_polymorphic(_Tp)) {
        if (typeid(__x) != typeid(_Tp) || typeid(__y) != typeid(_Tp))
          __is_eligible = false;
      }
      auto abs = [](auto a) { return a < ptrdiff_t(0) ? -a : a; };
      if (abs(&__x - &__y) < ptrdiff_t(__datasizeof(_Tp))) [[unlikely]]
        __is_eligible = false;
      if (__is_eligible) [[likely]] {
        swap_value_representations(__x, __y);
        return;
      }
    }
  }
#endif
  _Tp __t(std::move(__x));
  __x = std::move(__y);
  __y = std::move(__t);
}

template <class _Tp, size_t _Np, __enable_if_t<__is_swappable_v<_Tp>, int> >
inline _LIBCPP_HIDE_FROM_ABI _LIBCPP_CONSTEXPR_SINCE_CXX20 void swap(_Tp (&__a)[_Np], _Tp (&__b)[_Np])
    _NOEXCEPT_(__is_nothrow_swappable_v<_Tp>) {
  for (size_t __i = 0; __i != _Np; ++__i) {
    swap(__a[__i], __b[__i]);
  }
}

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif // _LIBCPP___UTILITY_SWAP_H
