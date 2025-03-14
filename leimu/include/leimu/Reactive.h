#pragma once

#include "framework.h"

namespace leimu {
  template<typename T>
  class Reactive {
    T _value;
    std::vector<std::function<void(const Reactive<T>* sender)>> _event;

  public:
    template<typename... TArgs>
    Reactive(TArgs&&... args) : _value(std::forward<TArgs>(args)...) {}

    T operator()() const {
      return _value;
    }

    Reactive<T>& operator()(T value) {
      _value = value;
      for (const auto &item: _event) {
        item(this);
      }
      return *this;
    }
  };
}