#pragma once

namespace tito {
  template <typename T, size_t N>
  class Collection {
    public:
      Collection() :
        _pointers(nullptr),
        _values({})
      { }

      void setValue(size_t index, T value)
      {
        if (index >= N) {
          return;
        }

        _pointers[index] = (T*) &_values[index * sizeof(T)];
        *_pointers[index] = value;
      }

      T* operator[](size_t index) const
      {
        if (index >= N) {
          return nullptr;
        }

        return _pointers[index];
      }

    private:
      T* _pointers[N];
      unsigned char _values[N * sizeof(T)];
  };
}
