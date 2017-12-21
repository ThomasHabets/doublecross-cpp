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
#include<algorithm>
#include<cstring>
#include<fcntl.h>
#include<iostream>
#include<string>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#include<vector>

#include"fdwrap.h"
#include"monad.h"
#include"orerror.h"

namespace orerror_test {

OrError<int, std::string>
open(const std::string& fn)
{
  typedef OrError<int, std::string> Ret;
  int fd = ::open(fn.c_str(), O_RDONLY);
  if (fd < 0) {
    return Ret::Error("open(" + fn + "): " + strerror(errno));
  }
  return Ret::Just(fd);
}

OrError<std::string, std::string>
read(FDWrap fd)
{
  typedef OrError<std::string, std::string> Ret;
  struct stat st;
  {
    const int rc = ::fstat(fd.get(), &st);
    if (0 > rc) {
      return Ret::Error(std::string("fstat(): ") + strerror(errno));
    }
  }
  std::vector<char> buf(st.st_size);
  const int rc = ::read(fd.get(), buf.data(), buf.size());
  if (0 > rc) {
    return Ret::Error(std::string("read(): ") + strerror(errno));
  }
  return Ret::Just(std::string(buf.begin(), buf.end()));
}

int
print(const OrError<std::string, std::string>& data)
{
  if (data.is_error()) {
    std::cerr << data.error() << std::endl;
    return 1;
  }
  std::cout << data.get() << std::endl;
  return 0;
}

std::string
reverse(const std::string&in)
{
  std::vector<char> buf;
  std::reverse_copy(in.begin(), in.end(), std::back_inserter(buf));
  return std::string(buf.begin(), buf.end());
}

// Bind operator.
template<typename MA, typename F>
auto operator>>(MA ma, F&& func)
    -> typename std::result_of<F(const typename MA::type&)>::type
{
  return ma.bind(std::forward<F>(func));
}

int
cat(const std::string& fn)
{
  auto lambda = [](const std::string& in) -> std::string {
    std::vector<char> buf;
    std::reverse_copy(in.begin(), in.end(), std::back_inserter(buf));
    return std::string(buf.begin(), buf.end());
  };
  std::function<std::string(const std::string&)> std_f = lambda;

  // .bind() syntax.
  auto data = open(fn).bind(&read);

  // bind operator syntax.
  data = open(fn) >> read;

#if 1
  data = Map(lambda, data);   // Lambda.
  data = Map(std_f, data);    // std::function.
  data = Map(reverse, data);  // Function pointer.
#endif
  return print(data);
}
}

int
main(int argc, char** argv)
{
  return orerror_test::cat(argv[1]);
}
