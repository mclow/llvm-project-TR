#ifndef _LIBCPP___MEMORY_RELOCATE_H
#define _LIBCPP___MEMORY_RELOCATE_H

#include <__config>
#include <__functional/operations.h>
#include <__type_traits/is_const.h>
#include <__type_traits/is_trivially_relocatable.h>
#include <cstddef>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

_LIBCPP_BEGIN_NAMESPACE_STD

#if _LIBCPP_STD_VER >= 20

template <class _Tp>
_LIBCPP_EXPORTED_FROM_ABI _Tp* trivially_relocate(_Tp* __begin, _Tp* __end, _Tp* __new_location) noexcept {
    static_assert(is_trivially_relocatable_v<_Tp> && !is_const_v<_Tp>);
	(void) __builtin_trivially_relocate(__new_location, __begin, sizeof(_Tp) * (__end - __begin));
	return __new_location + (__end - __begin);
}

template <class T>
constexpr
T* relocate(T* __begin, T* __end, T* __new_location)
{
    static_assert(is_trivially_relocatable_v<T> || is_nothrow_move_constructible_v<T>);
//	When relocating to the same location, do nothing.
	if (__begin == __new_location || __begin == __end)
      return __new_location + (__end - __begin);

//	Then, if we are not evaluating at compile time and the type supports
//	trivial relocation, delegate to `trivially_relocate`.
	if ! consteval {
		if constexpr (is_trivially_relocatable_v<T>)
			return std::trivially_relocate(__begin, __end, __new_location);
	}

	if constexpr (is_move_constructible_v<T>) {
	//	For nontrivial relocatable types or any time during constant
	//	evaluation, we must detect overlapping ranges and act accordingly,
	//	which can be done only if the type is movable.
		if ! consteval {
		//	At run time, when there is no overlap, we can, using other Standard
		//	Library algorithms, do all moves at once followed by all destructions.
		if (less{}(__end,__new_location) || less{}(__new_location + (__end-__begin), __begin)) {
			T* result = uninitialized_move(__begin, __end, __new_location);
			std::destroy(__begin,__end);
			return result;
			}
		}

		if (less{}(__new_location,__begin) || less{}(__end,__new_location)) {
	//	Any move to a lower address in memory or any nonoverlapping move can be
	//	done by iterating forward through the range.
			T* next = __begin;
			T* dest = __new_location;
			while (next != __end) {
				::new(dest) T(std::move(*next));
				next->~T();
				++next; ++dest;
			}
		}
		else {
		//	When moving to a higher address that overlaps, we must go backward through
		//	the range.
			T* next = __end;
			T* dest = __new_location + (__end-__begin);
			while (next != __begin) {
				--next;  --dest;
				::new(dest) T(std::move(*next));
				next->~T();
			}
		}
	return __new_location + (__end - __begin);
	}

//	The only way to reach this point is during constant evaluation where type `T`
//	is trivially relocatable but not move constructible. Such cases are not supported
//	so we mark this branch as unreachable.
	unreachable();
}

#endif

_LIBCPP_END_NAMESPACE_STD

#endif // _LIBCPP___MEMORY_RELOCATE_H
