#include <algorithm>
#include <cstdint>
#include <iostream>
#include <type_traits>
#include <utility>
#include <vector>
#include <stdexcept>

namespace {
    template<class T>
    struct VectorBase {
        T *data_{nullptr};
        uint64_t capacity_{0};

        explicit VectorBase(uint64_t n = 0) : capacity_(n) {
            data_ = static_cast<T *>(operator new(
                    n * sizeof(T)));
        }

        void swap(VectorBase &other) {
            std::swap(data_, other.data_);
            std::swap(capacity_, other.capacity_);
        }

        ~VectorBase() {
            operator delete(data_);
        }
    };
}

template<class T>
class Vector : private VectorBase<T> {
public:
    Vector() noexcept : size_(0) {
    }

    Vector(const Vector &other);

    Vector(uint64_t n, const T &val = T());

    Vector &operator=(const Vector &other);

    void push_back(const T &value);
    void push_back(T &&value);

    template<class... Args>
    void emplace_back(Args &&... args);

    void pop_back();

    const T &operator[](uint64_t i) const noexcept {
        return data_[i];
    }

    T &operator[](uint64_t i) noexcept {
        return const_cast<T &>(static_cast<const Vector &>(*this)[i]);
    }

    uint64_t size() const noexcept {
        return size_;
    }

    uint64_t capacity() const noexcept {
        return capacity_;
    }

    void reserve(uint64_t new_capacity);

    void resize(uint64_t count, const T &value);

    void resize(uint64_t count);

    void clear() noexcept {
        resize(0);
    }

    void swap(Vector &other) noexcept {
        VectorBase<T>::swap(other);
        std::swap(size_, other.size_);
    }

    T *begin() {
        return data_;
    }

    T *end() {
        return data_ + size_;
    }

    ~Vector() {
        for (uint64_t i = 0; i != size_; i++) {
            data_[i].~T();
        }
    }

    void Print() const {
        for (uint64_t i = 0; i != size_; i++) {
            std::cout << data_[i] << ' ';
        }
        std::cout << '\n';
    }

private:
    uint64_t size_{0};
    using VectorBase<T>::data_;
    using VectorBase<T>::capacity_;

    void reallocate(uint64_t n) {
        VectorBase<T> temp(n);
        uint64_t i = 0;
        try {
            for (;i != size_; i++) {
                new(temp.data_ + i) T(std::move(data_[i]));
            }
        } catch (...) {
            for (uint64_t j = 0; j != i; ++j)
                temp.data_[i].~T();
            throw;
        }
        VectorBase<T>::swap(temp);
        for (uint64_t i = 0; i < size_; i++)
            temp.data_[i].~T();
    }
};

template <class T>
Vector<T>::Vector(const Vector &other) {
    reallocate(other.capacity_);
    size_ = other.size_;
    uint64_t i = 0;
    try {
        for (i = 0; i != size_; i++) {
            new(data_ + i) T(other[i]);
        }
    } catch (...) {
        for (uint64_t j = 0; j != i; ++j)
            data_[i].~T();
        throw;
    }
}

template <class T>
Vector<T>::Vector(uint64_t n, const T &val) : size_(n), VectorBase<T>(n) {
    uint64_t i = 0;
    try {
        for (; i != size_; i++) {
            new(data_ + i) T(val);
        }
    } catch (...) {
        for (uint64_t j = 0; j != i; ++j)
            data_[i].~T();
        throw;
    }
}

template <class T>
Vector<T>& Vector<T>::operator=(const Vector &other) {
    try {
        VectorBase<T> temp(other.capacity_);
        VectorBase<T>::swap(temp);
        for (uint64_t k = 0; k != size_; k++)
            temp.data_[k].~T();
        size_ = other.size_;
        uint64_t i = 0;
        try {
            for (i = 0; i != size_; i++) {
                new(data_ + i) T(other[i]);
            }
        } catch (...) {
            for (uint64_t j = 0; j != i; ++j)
                data_[i].~T();
            throw;
        }
    } catch (...) {
    }
    return *this;
}

template <class T>
void Vector<T>::push_back(const T &value) {
    if (capacity_ == 0) {
        reallocate(1);
    } else if (size_ + 1 >= capacity_) {
        reallocate(capacity_ * 2);
    }
    try {
        new(data_ + size_) T(value);
    } catch (...) {
        data_[size_].~T();
        throw;
    }
    size_++;
}

template <class T>
void Vector<T>::push_back(T &&value) {
    if (capacity_ == 0) {
        reallocate(1);
    } else if (size_ + 1 >= capacity_) {
        reallocate(capacity_ * 2);
    }
    try {
        new(data_ + size_) T(std::forward<T>(value));
    } catch (...) {
        data_[size_].~T();
        throw;
    }
    size_++;
}

template <class T>
template<class... Args>
void Vector<T>::emplace_back(Args &&... args) {
    if (size_ + 1 >= capacity_)
        reallocate((capacity_ + 1) * 2);
    new(data_ + size_) T(std::forward<Args>(args)...);
    size_++;
}

template <class T>
void Vector<T>::pop_back() {
    data_[size_ - 1].~T();
    size_--;
}

template <class T>
void Vector<T>::reserve(uint64_t new_capacity) {
    if (new_capacity > capacity_) {
        try {
            reallocate(new_capacity);
        } catch (...) {
        }
    }
}

template <class T>
void Vector<T>::resize(uint64_t count, const T &value) {
    if (count < size_) {
        for (uint64_t i = count; i < size_; i++) {
            data_[i].~T();
        }
    }
    if (count > size_) {
        if (count > capacity_)
            reallocate(count);
        uint64_t i = 0;
        try {
            for (i = size_; i != count; i++) {
                new(data_ + i) T(value);
            }
        } catch (...) {
            for (uint64_t j = size_; j != i; ++j)
                data_[i].~T();
            throw;
        }
    }
    size_ = count;
}

template <class T>
void Vector<T>::resize(uint64_t count) {
    if (count < size_) {
        for (uint64_t i = count; i < size_; i++) {
            data_[i].~T();
        }
    }
    if (count > size_) {
        if (count > capacity_) {
            VectorBase<T> temp(count);
            uint64_t i;
            try {
                for (i = 0; i != size_; i++) {
                    new(temp.data_ + i) T(data_[i]);
                }
            } catch (...) {
                for (uint64_t j = 0; j != i; ++j)
                    temp.data_[i].~T();
                throw;
            }
            VectorBase<T>::swap(temp);
            try {
                for (i = size_; i != count; i++) {
                    new(data_ + i) T();
                }
            } catch (...) {
                for (uint64_t j = size_; j != i; ++j)
                    data_[i].~T();
                throw;
            }
            for (uint64_t i = 0; i < size_; i++)
                temp.data_[i].~T();
        } else {
            uint64_t i = 0;
            try {
                for (i = size_; i != count; i++) {
                    new(data_ + i) T();
                }
            } catch (...) {
                for (uint64_t j = size_; j != i; ++j)
                    data_[i].~T();
                throw;
            }
        }
    }
    size_ = count;
}