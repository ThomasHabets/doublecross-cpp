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
#include<vector>
#include<algorithm>
template<typename T>
class List {
public:
  // Explicit constructors.
  static List Empty() {
    return List();
  }
  static List One(const T&v) {
    return List(v);
  }

  // Other functions.
  void append(const T&v)  { val_.push_back(v); }
  void append(const T&&v) { val_.push_back(std::move(v)); }

  // Bind.
  template<typename B>
  List<B> bind(std::function<List<B>(T)> func) const {
    List<B> ret;
    for (const auto& t : val_) {
      const auto a = func(t).val_;
      for (const auto& t2 : a) {
        ret.val_.push_back(t2);
      }
    }
    return ret;
  }

  // Ret.
  static List ret(const T&v) {
    return List(v);
  }

  template <typename B>
  friend class List;

  // Never called. Only used for type inference.
  template<typename B>
  static List<B> bindtype(B) {
    throw std::runtime_error("List::bindtype() called");
  }
private:
  List() {}
  List(const T&v):val_{v} {}

  std::vector<T> val_;
};
