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
#include<iostream>
#include<unistd.h>
#include<cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include"maybe.h"
#include"list.h"
#include"monad.h"

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
    } catch(const std::exception& e) {
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

class FDWrap {
public:
  FDWrap() {}
  FDWrap(const FDWrap& rhs) {
    fd_ = dup(rhs.fd_);
  }
  FDWrap(FDWrap&& rhs) {
    rhs.fd_ = fd_;
    fd_ = -1;
  }
  FDWrap(int fd): fd_(fd) {
  }
  ~FDWrap()
  {
    if (fd_ > -1) {
      close(fd_);
    }
  }
  int get() const { return fd_; }
private:
  int fd_ = -1;
};

Maybe<int>
maybe_open(const char* fn)
{
  int fd = open(fn, O_RDONLY);
  if (fd < 0) {
    return Maybe<int>::Nothing();
  }
  return Maybe<int>::Just(fd);
}

Maybe<std::string>
maybe_read(FDWrap fd)
{
  std::cout << "Reading using fd " << fd.get() << std::endl;
  struct stat st;
  {
    const int rc = fstat(fd.get(), &st);
    if (0 > rc) {
      return Maybe<std::string>::Nothing();
    }
  }
  std::vector<char> buf(st.st_size);
  const int rc = read(fd.get(), buf.data(), buf.size());
  if (0 > rc) {
    return Maybe<std::string>::Nothing();
  }
  return Maybe<std::string>::Just(std::string(buf.begin(), buf.end()));
}

void
cat(const char* fn)
{
  {
    auto fd = maybe_open(fn);
    auto fdw = fd.bind([](int n) -> Maybe<FDWrap> {
        return Maybe<FDWrap>::Just(n);
    });
    auto data = fdw.bind(&maybe_read);
    std::cout << data.get() << std::endl;
  }

  std::cout << maybe_open(fn).bind(&maybe_read).get() << std::endl;
}

int
main(int argc, char** argv)
{
  maybe_monad();
  list_monad();
  cat(argv[1]);
}
