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
#include<string>

template<typename T, typename E>
class OrError {
 private:
  struct ErrObj {
    ErrObj(const E& e): error(e){}
    const E& error;
  };
public:
  // Explicit constructors.
  static OrError Just(const T&v) {
    return OrError(v);
  }
  static OrError Error(const E& e) {
    return OrError(ErrObj(e));
  }

  // Bind.
  template<typename F>
  auto bind(F&& func) const
    -> decltype(func(T()))
  {
    typedef decltype(func(T())) MB;
    if (is_error_) {
      return MB::Error(error_);
    }
    return func(val_);
  }

  // Ret.
  static OrError ret(const T&v) {
    return OrError(v);
  }

  // Get value.
  T get() const {
    if (is_error_) {
      throw error_;
    }
    return val_;
  }

  const bool is_error() const { return is_error_; }
  const E& error() const { return error_; }

  template <typename B, typename E2>
  friend class OrError;

  // Never called. Only used for type inference.
  template<typename B>
  static OrError<B, E> bindtype(B) {
    throw std::runtime_error("OrError::bindtype() called");
  }
private:
  OrError(const ErrObj& eo)
      :is_error_(true),
       error_(eo.error)
  {
  }
  OrError(const T&v):
      val_(v),
      is_error_(false)
  {
  }

  T val_ = T();
  bool is_error_ = false;
  E error_;
};
