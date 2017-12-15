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
template <typename MA, typename A, typename B>
auto
Map(std::function<B(const A&)> func, const MA& in)
  -> decltype(MA::bindtype(B()))
{
  typedef decltype(in.bindtype(B())) MB;
  std::function<MB(A)> f2 = [&func,&in](const A& t) -> MB {
    return MB::ret(func(t));
  };
  return in.bind(f2);
}

template <typename MA, typename A>
void
Do(std::function<void(const A&)> func, const MA& in)
{
  std::function<A(const A&)> f2 = [&func, &in](const A& t) -> A {
    func(t);
    return t;
  };
  Map(f2, in);
}
