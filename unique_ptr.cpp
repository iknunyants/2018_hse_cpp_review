#include <iostream>
#include <cstddef>
#include <tuple>
#include <memory>

template<class T, class Deleter = std::default_delete<T>>
class UniquePtr {
private:
    std::tuple<T *, Deleter> ptr_{{nullptr}, Deleter()};

public:
    UniquePtr(T *ptr = nullptr) noexcept : ptr_{ptr, Deleter()} {
    }

    UniquePtr(T* ptr, Deleter del) noexcept: ptr_{ptr, del} {
    }

    UniquePtr(UniquePtr &&other) noexcept: ptr_{other.ptr_} {
        std::get<0>(other.ptr_) = nullptr;
    }

    UniquePtr(UniquePtr &) = delete;

    UniquePtr &operator=(std::nullptr_t) {
        reset();
        return *this;
    }

    UniquePtr &operator=(UniquePtr &) = delete;

    UniquePtr &operator=(UniquePtr &&other) noexcept {
        reset();
        ptr_ = other.ptr_;
        std::get<0>(other.ptr_) = nullptr;
        return *this;
    }

    T *operator->() const noexcept {
        return std::get<0>(ptr_);
    }

    const T &operator*() const noexcept {
        return *std::get<0>(ptr_);
    }

    T &operator*() noexcept {
        return *std::get<0>(ptr_);
    }

    T *release() noexcept {
        auto temp_ptr = get();
        std::get<0>(ptr_) = nullptr;
        return temp_ptr;
    }


    void reset(T *ptr = nullptr) noexcept {
        get_deleter()(std::get<0>(ptr_));
        std::get<0>(ptr_) = ptr;
    }

    void swap(UniquePtr &other) noexcept {
        std::swap(ptr_, other.ptr_);
    }

    T *get() const noexcept {
        return std::get<0>(ptr_);
    }

    explicit operator bool() const noexcept {
        return std::get<0>(ptr_) != nullptr;
    }

    ~UniquePtr() noexcept {
        reset();
    }

    Deleter &get_deleter() {
        return std::get<1>(ptr_);
    }

    const Deleter &get_deleter() const {
        return std::get<1>(ptr_);
    }
};