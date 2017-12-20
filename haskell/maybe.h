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

template<typename A>
class Maybe {
public:
  // Explicit constructors.
  static Maybe Just(const A&v) {
    return Maybe(v);
  }
  static Maybe Nothing() {
    return Maybe();
  }

  //////////////////////////////////
  // BEGIN Monad required interface
  //
  // * M<B> bind(M<B> func(const &A))
  // * M<A> ret(const A&)
  // * Friend yourself
  // * type typedef
  // * bindtype typedef
  //
  //////////////////////////////////

  // Bind.
  // Takes `M<B> func(const A&)` and applies it to embedded value.
  template<typename F>
  auto bind(F&& func) const
    -> decltype(func(A()))
  {
    typedef decltype(func(A())) MB;
    if (nothing_) {
      return MB::Nothing();
    }
    return func(val_);
  }

  // Ret. Just embed the value.
  static Maybe ret(const A&v)
  {
    return Maybe(v);
  }

  // Friend yourself.
  template <typename B>
  friend class Maybe;

  // Used for type conversions.
  typedef A type;
  template<typename B> using bindtype = Maybe<B>;

  ///////////////////////////
  // END Monad interface
  ///////////////////////////

  // Get value.
  A get() const
  {
    if (nothing_) {
      throw std::runtime_error("Maybe was Nothing");
    }
    return val_;
  }

  // Check for 'nothing'ness.
  bool is_nothing() const
  {
    return nothing_;
  }

private:
  Maybe() {}
  Maybe(const A&v):val_(v), nothing_(false) {}

  // TODO: this means that A needs to have a default constructor.
  // Maybe good to get around this by making it a pointer?
  A val_ = A();
  bool nothing_ = true;
};
