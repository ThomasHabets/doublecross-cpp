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
#include<cstdio>
#include<cstring>
#include<fcntl.h>
#include<iostream>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>

#include"list.h"
#include"maybe.h"
#include"monad.h"
#include"orerror.h"

template<typename T>
T
mul3(const T& in)
{
  return in*3;
}

template<typename T>
class Mul : public std::function<T(const T&)> {
public:
  Mul(const T& v): val_(v) {}
  T operator()(const T&in) const
  {
    return in * val_;
  }
private:
  const T val_;
};

float
conv(int in)
{
  return static_cast<float>(in);
}

void
maybe_monad()
{
  Maybe<int> foo = Maybe<int>::Just(1);
  std::cout << foo.get() << std::endl;

  // Multiply int by 3.
  {
    std::function<int(const int&)> func = mul3<int>;
    Maybe<int> bar = Map(func, foo);
    std::cout << bar.get() << std::endl;
  }

  // Convert to float.
  auto c = Maybe<float>::Nothing();
  {
    std::function<float(const int&)> conv_func = conv;
    c = Map(conv_func, foo);
    printf("%f\n", c.get());
  }

  // Multiply float.
  {
    Maybe<float> c2 = Map(Mul<float>(3.5), c);
    c2 = Map(Mul<float>(10.2), c2);
    printf("%f\n", c2.get());
  }

  // Make sure getting Nothing throws.
  {
    bool threw = false;
    std::cout << "Should throw:\n";
    Maybe<int> n = Maybe<int>::Nothing();
    try {
      n.get();
    } catch(const std::runtime_error& e) {
      std::cout << "  Exception: " << e.what() << std::endl;
      threw = true;
    }
    if (!threw) {
      std::cout << "ERROR: didn't throw\n";
    }
  }
}

// Create and map a list.
void
list_monad()
{
  auto a = List<int>::One(1);
  a.append(2);
  a = Map(Mul<int>(1000), a);
  std::cout << "List:\n";
  {
    std::function<int(const int&)> f = [](const int &v) -> int{
      std::cout << "  Entry: " << v << std::endl;
      return v;
    };
    Map(f, a);
  }

  std::cout << "List:\n";
  a = Map(Mul<int>(10), a);
  {
    std::function<void(const int&)> f = [](const int &v) {
      std::cout << "  Entry: " << v << std::endl;
    };
    Do(f, a);
  }
}

int
main(int argc, char** argv)
{
  maybe_monad();
  list_monad();
}
