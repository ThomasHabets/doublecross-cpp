// -*- c++ -*-
// Copyright 2017 Google Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
#include<functional>
#include<type_traits>

namespace internal {

template<typename... T>
struct func_type_impl;

// Lots of brute force here. Magic left to explain BEGIN.
template<typename Callable>
struct func_type_impl<Callable>
{
  using type = typename func_type_impl<decltype(&Callable::operator())>::type;
};
template<typename C, typename Ret, typename... Args>
struct func_type_impl<Ret(C::*)(Args...) const>
{
  using type = std::function<Ret(Args...)>;
};

// Specialization for function pointers
template<typename Ret, typename... Args>
struct func_type_impl<Ret(*)(Args...)>
{
  using type = std::function<Ret(Args...)>;
};
// Magic left to explain END.

template<typename... T>
using func_type = typename func_type_impl<typename std::decay<T>::type...>::type;

// For `B Func(A)` and `M<A>`, return `B` type.
// We need MA because possible `Func` overloading. (I think)
//
// Type requirements:
//   * MA type needs to have `type` typedef to `A`.
template<typename Func, typename MA>
using map_b_type = typename std::result_of<Func(const typename MA::type&)>::type;

// For `B Func(A)` and `M<A>`, return `M<B>` type.
//
// Type requirements:
//   * MA type needs to have `type` typedef to `A`.
//   * MA type needs to have `bindtype<B>` typedef to `M<B>`.
template<typename Func, typename MA>
using map_ret_type = typename MA::template bindtype<map_b_type<Func, MA>>;

}  // namespace internal


/*
 * M<B> Map([callable], const M<A>&)
 */
template<typename Func, typename MA>
auto
Map(Func&& func, const MA& in)
    -> internal::map_ret_type<Func, MA>
{
  typedef typename MA::type A;
  typedef internal::map_ret_type<Func, MA> MB;
  const internal::func_type<Func> f = std::forward<Func>(func);
  const std::function<MB(A)> f2 = [&f](const A& t) -> MB {
    return MB::ret(f(t));
  };
  return in.bind(f2);
}

/*
 * void Do([callable], const M<A>&)
 *
 * Do is just a simplified Map() whose supplied function returns void.
 */
template<typename Func, typename MA>
void
Do(Func&& func, const MA& in)
{
  typedef typename MA::type A;
  const std::function<A(const A&)> f2 = [&func](const A& t) -> A {
    func(t);
    return t;
  };
  Map(f2, in);
}
