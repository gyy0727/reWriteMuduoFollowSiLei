#pragma once

#include <boost/exception/exception.hpp>
#include <memory>
namespace Sylar {
template <class T, class X = void, int N = 0> class Singleton {


public:
  static T *getInstance() {


    static T v;
    return &v;
  }

};
template <class T, class X = void, int N = 0> class SingletonPtr {
public:
  static std::shared_ptr<T> getInstance() {
    static std::shared_ptr<T> t(new T());
    return t;
  }
};
} // namespace Sylar