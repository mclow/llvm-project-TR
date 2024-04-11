#ifndef _LIBCPP___MEMORY_RELOCATE_H
#define _LIBCPP___MEMORY_RELOCATE_H

#include <__config>
#include <__type_traits/is_const.h>
#include <__type_traits/is_trivially_relocatable.h>
#include <cstddef>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

_LIBCPP_BEGIN_NAMESPACE_STD

#if _LIBCPP_STD_VER >= 20

template <class T>
  requires(is_trivially_relocatable_v<T> && !is_const_v<T>)
_LIBCPP_EXPORTED_FROM_ABI T* trivially_relocate(T* begin, T* end, T* new_location) {
  return __builtin_trivially_relocate(new_location, begin, end - begin);
}

#endif

_LIBCPP_END_NAMESPACE_STD

#endif // _LIBCPP___MEMORY_RELOCATE_H
