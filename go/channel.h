// -*- c++ -*-
// Copyright 2018 Google Inc.
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
#include<utility>
#include<queue>
#include<mutex>
#include<condition_variable>

namespace internal {
template<typename T>
class InternalChannel {
protected:
  std::queue<T> queue_;
  const int capacity_;
  std::mutex m_;
  std::condition_variable cv_;
  bool closed_ = false;

  std::mutex zerom_;
  bool zerogotten_ = false;
public:
  InternalChannel(int capacity)
      :capacity_(capacity)
  {
  }

  T get()
  {
    std::unique_lock<std::mutex> lk(m_);
    if (closed_ && queue_.empty()) {
      return T();
    }
    cv_.wait(lk, [this]{
        return !queue_.empty();
    });
    T tmp = queue_.front();
    queue_.pop();
    if (capacity_ == 0) {
      zerogotten_ = true;
    }
    cv_.notify_one();
    return std::move(tmp);
  }

  void close()
  {
    std::unique_lock<std::mutex> lk(m_);
    closed_ = true;
  }

  void put(const T& v)
  {
    if (capacity_ == 0) {
      std::unique_lock<std::mutex> lk2(zerom_);
      {
        std::unique_lock<std::mutex> lk(m_);
        if (closed_) {
          throw std::runtime_error("writing to closed channel");
        }
        zerogotten_ = false;
        queue_.push(v);
        cv_.notify_one();
        cv_.wait(lk, [this]{
            return zerogotten_;
        });
      }
    } else {
      std::unique_lock<std::mutex> lk(m_);
      if (closed_) {
        throw std::runtime_error("writing to closed channel");
      }
      cv_.wait(lk, [this]{
          return queue_.size() < capacity_;
        });
      queue_.push(v);
    }
  }
};
}

template<typename T>
class ReadChannel {
  internal::InternalChannel<T>& ch_;
public:
  ReadChannel(internal::InternalChannel<T>& ch)
      :ch_(ch)
  {
  }

  T get() { return ch_.get(); }
};

template<typename T>
class WriteChannel {
  internal::InternalChannel<T>& ch_;
public:
  WriteChannel(internal::InternalChannel<T>& ch)
      :ch_(ch)
  {
  }

  void put(T&&v) { return ch_.put(v); }
  void close() { ch_.close(); }
};


template<typename T>
class Channel: public ReadChannel<T>, public WriteChannel<T> {
  internal::InternalChannel<T> ch_;
public:
  Channel(int capacity = 0)
      :ReadChannel<T>(ch_),
      WriteChannel<T>(ch_),
      ch_(capacity)
  {
  }
};
