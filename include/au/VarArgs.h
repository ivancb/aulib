#pragma once

#include <functional>
#include <tuple>

namespace au
{
	namespace detail
	{
		template<int...>
		struct Sequence { };

		template<int m, int... n>
		struct GenerateSequence : GenerateSequence<m - 1, m - 1, n...> {};

		template<int... limit>
		struct GenerateSequence<0, limit...>
		{
			typedef Sequence<limit...> type;
		};

		template<int ...S, typename T, typename... Args>
		T TupleCallImpl(const std::function<T(Args...)>& v, const std::tuple<Args...>& args, Sequence<S...>)
		{
			return v(std::get<S>(args)...);
		}

		template<bool voidData>
		class UnpackCallProxy
		{
		};

		template<>
		class UnpackCallProxy<false>
		{
		public:
			template<typename T, typename... Args>
			static T DoCall(const std::function<T(Args...)>& v, const Args&... args)
			{
				return v(args...);
			}

			template<typename T, typename... Args>
			static T DoCallWithTuple(const std::function<T(Args...)>& v, const std::tuple<Args...>& args)
			{
				return TupleCallImpl(v, args, typename GenerateSequence<sizeof...(Args)>::type());
			}
		};

		template<>
		class UnpackCallProxy<true>
		{
		public:
			template<typename T, typename... Args>
			static T DoCall(const std::function<T(Args...)>& v, const Args... args)
			{
				return v();
			}

			template<typename T, typename... Args>
			static T DoCallWithTuple(const std::function<T(Args...)>& v, const std::tuple<Args...>& args)
			{
				return v();
			}
		};
	}

	template<typename T, typename... Args>
	static T UnpackCall(const std::function<T(Args...)>& v, const Args&... args)
	{
		return detail::UnpackCallProxy<sizeof...(Args) == 0>::DoCall(v, args...);
	}

	template<typename T, typename... Args>
	static T UnpackCallWithTuple(const std::function<T(Args...)>& v, const std::tuple<Args...>& args)
	{
		return detail::UnpackCallProxy<sizeof...(Args) == 0>::DoCallWithTuple(v, args);
	}
}