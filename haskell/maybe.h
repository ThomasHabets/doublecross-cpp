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

template<typename T>
class Maybe {
public:
  // Explicit constructors.
  static Maybe Just(const T&v) {
    return Maybe(v);
  }
  static Maybe Nothing() {
    return Maybe();
  }

  // Bind.
  template<typename F>
  auto bind(F&& func) const
    -> decltype(func(T()))
  {
    typedef decltype(func(T())) MB;
    if (nothing_) {
      return MB::Nothing();
    }
    return func(val_);
  }

  // Ret.
  static Maybe ret(const T&v) {
    return Maybe(v);
  }

  // Get value.
  T get() const {
    if (nothing_) {
      throw std::runtime_error("Maybe was Nothing");
    }
    return val_;
  }

  template <typename B>
  friend class Maybe;

  // Never called. Only used for type inference.
  template<typename B>
  static Maybe<B> bindtype(B) {
    throw std::runtime_error("Maybe::bindtype() called");
  }
private:
  Maybe() {}
  Maybe(const T&v):val_(v), nothing_(false) {}

  T val_ = T();
  bool nothing_ = true;
};
