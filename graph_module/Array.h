#ifndef ARRAY_H
#define ARRAY_H
#include <cstddef>
#include <cstdlib>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <ostream>
#include <vector>

namespace lib {

constexpr const size_t DEFAULT_BLOCK_SIZE = 10;

template <typename T>
concept ArrayElement =
    (std::copy_constructible<T> || std::move_constructible<T> ||
     std::default_initializable<T>) &&
    std::destructible<T>;

template <ArrayElement T> class Array;
template <ArrayElement T>
std::ostream &operator<<(std::ostream &, const Array<T> &);
template <ArrayElement T> void swap(Array<T> &, Array<T> &) noexcept;

template <ArrayElement T> class Array {
public:
  using value_type = T;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference = T &;
  using const_reference = const T &;
  using pointer = T *;
  using const_pointer = const T *;

private:
  std::vector<pointer> data_;
  size_type block_size_;
  size_type current_size_;
  size_type current_capacity_;

  pointer get_slot(size_type index) const;

public:
  Array(size_type block_size = DEFAULT_BLOCK_SIZE);
  Array(size_type count, const_reference item);
  Array(std::initializer_list<T> items);

  template <std::input_iterator InputIterator>
  Array(InputIterator first, InputIterator last);

  Array(const Array &other);
  Array(Array &&other) noexcept;
  Array &operator=(const Array &other);
  Array &operator=(Array &&other) noexcept;
  ~Array() noexcept;

  void add_block();
  void remove_block();
  // I wasn't sure if the aliases are only for the user or for us too
  // so I tried using them in some of the declarations
  size_type size() const;
  size_type capacity() const;
  bool empty() const;

  void print(std::ostream &stream = std::cout) const;

  template <typename Source>
    requires std::constructible_from<T, Source &&>
  void push_back(Source &&item);

  template <typename... Args>
    requires std::constructible_from<T, Args &&...>
  T &emplace_back(Args &&...args);

  void pop_back();
  void clear() noexcept;

  reference at(size_type index);
  const_reference at(size_type index) const;
  reference operator[](size_type index);
  const_reference operator[](size_type index) const;

  friend std::ostream &operator<< <T>(std::ostream &stream,
                                      const Array<T> &array);
  friend void swap<T>(Array<T> &a, Array<T> &b) noexcept;

  template <bool Constant> class iterator_base {
    friend class Array<T>;

    using array_pointer =
        std::conditional_t<Constant, const Array<T> *, Array<T> *>;

    array_pointer parent_;
    size_type index_;

  public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = std::conditional_t<Constant, const T, T>;
    using difference_type = std::ptrdiff_t;
    using pointer = std::conditional_t<Constant, const T *, T *>;
    using reference = std::conditional_t<Constant, const T &, T &>;

    iterator_base() : parent_(nullptr), index_(0) {}
    iterator_base(array_pointer parent, size_type index)
        : parent_(parent), index_(index) {}

    operator iterator_base<true>() const {
      return iterator_base<true>(parent_, index_);
    }

    bool operator==(const iterator_base &other) const {
      return parent_ == other.parent_ && index_ == other.index_;
    }
    bool operator!=(const iterator_base &other) const {
      return !(*this == other);
    }

    iterator_base &operator++() {
      ++index_;
      return *this;
    }

    iterator_base operator++(int) {
      iterator_base tmp = *this;
      ++index_;
      return tmp;
    }
    iterator_base &operator--() {
      --index_;
      return *this;
    }
    iterator_base operator--(int) {
      iterator_base tmp = *this;
      --index_;
      return tmp;
    }

    reference operator*() const { return (*parent_)[index_]; }

    pointer operator->() const { return &((*parent_)[index_]); }

    iterator_base operator+(difference_type n) const {
      return iterator_base(parent_, index_ + n);
    }

    iterator_base operator-(difference_type n) const {
      return iterator_base(parent_, index_ - n);
    }

    difference_type operator-(const iterator_base &other) const {
      return static_cast<difference_type>(index_) -
             static_cast<difference_type>(other.index_);
    }

    iterator_base &operator+=(difference_type n) {
      index_ += n;
      return *this;
    }

    iterator_base &operator-=(difference_type n) {
      index_ -= n;
      return *this;
    }

    reference operator[](difference_type n) const { return *(*this + n); }

    bool operator<(const iterator_base &other) const {
      return index_ < other.index_;
    }
    bool operator<=(const iterator_base &other) const {
      return index_ <= other.index_;
    }
    bool operator>(const iterator_base &other) const {
      return index_ > other.index_;
    }
    bool operator>=(const iterator_base &other) const {
      return index_ >= other.index_;
    }

    friend iterator_base operator+(difference_type n, const iterator_base &it) {
      return it + n;
    }
  };
  using iterator = iterator_base<false>;
  using const_iterator = iterator_base<true>;

  inline iterator begin() { return iterator(this, 0); }
  inline iterator end() { return iterator(this, current_size_); }

  inline const_iterator begin() const { return const_iterator(this, 0); }
  inline const_iterator end() const {
    return const_iterator(this, current_size_);
  }

  inline const_iterator cbegin() const { return const_iterator(this, 0); }
  inline const_iterator cend() const {
    return const_iterator(this, current_size_);
  }
  void resize(size_t size);
  iterator erase(const_iterator first, const_iterator last);
};

template <ArrayElement T> void Array<T>::resize(size_t size) {
  static_assert(
      std::default_initializable<T>,
      "resize() requires T to be default initializable to fill new slots");

  if (size < current_size_) {
    while (current_size_ > size) {
      pop_back();
    }
  } else if (size > current_size_) {
    size_type original_size = current_size_;
    try {
      while (current_size_ < size) {
        emplace_back();
      }
    } catch (...) {
      while (current_size_ > original_size) {
        pop_back();
      }
      throw;
    }
  }
}

template <ArrayElement T>
typename Array<T>::iterator Array<T>::erase(const_iterator first,
                                            const_iterator last) {
  static_assert(std::assignable_from<T &, T &&>, "T must be move-assignable");
  static_assert(
      requires(T &t, T &&s) {
        { t = std::move(s) } noexcept;
      }, "T move assignment must be noexcept for exception-safe erase");

  difference_type num_to_erase = last - first;
  if (num_to_erase <= 0) {
    return begin() + (first - cbegin());
  }
  size_type start_index = first.index_;
  size_type end_index = last.index_;

  for (size_type i = end_index; i < current_size_; ++i) {
    (*this)[i - num_to_erase] = std::move((*this)[i]);
  }
  for (difference_type i = 0; i < num_to_erase; ++i) {
    pop_back();
  }
  return begin() + start_index;
}

template <ArrayElement T> T *Array<T>::get_slot(size_type index) const {
  return &(data_[index / block_size_][index % block_size_]);
}

template <ArrayElement T>
Array<T>::Array(size_type block_size)
    : block_size_(block_size), current_size_(0), current_capacity_(0) {}
template <ArrayElement T> Array<T>::~Array() noexcept { clear(); }
template <ArrayElement T> void Array<T>::add_block() {
  pointer new_block =
      static_cast<pointer>(::operator new(block_size_ * sizeof(T)));
  try {
    data_.push_back(new_block);
  } catch (const std::bad_alloc &e) {
    ::operator delete(new_block);
    throw;
  }
  current_capacity_ += block_size_;
}

template <ArrayElement T> void Array<T>::remove_block() {
  ::operator delete(data_.back()); // Free the memory of the last block
  data_.pop_back();                // Remove the block from the vector
  current_capacity_ -= block_size_;
}

template <ArrayElement T>
template <typename Source>
  requires std::constructible_from<T, Source &&>
void Array<T>::push_back(Source &&item) {
  emplace_back(std::forward<Source>(item));
}

template <ArrayElement T>
template <typename... Args>
  requires std::constructible_from<T, Args &&...>
T &Array<T>::emplace_back(Args &&...args) {
  bool added = false;
  if (current_size_ == current_capacity_) {
    add_block();
    added = true;
  }
  try {
    T *target = get_slot(current_size_);
    new (target) T(std::forward<Args>(args)...);
    ++current_size_;
    return *target;
  } catch (...) {
    if (added)
      remove_block();
    throw;
  }
}

template <ArrayElement T> inline size_t Array<T>::size() const {
  return current_size_;
}

template <ArrayElement T> inline size_t Array<T>::capacity() const {
  return current_capacity_;
}

template <ArrayElement T> inline bool Array<T>::empty() const {
  return current_size_ == 0;
}

template <ArrayElement T> void Array<T>::print(std::ostream &stream) const {
  stream << "[";
  for (size_type i = 0; i < current_size_; ++i) {
    if (i > 0) {
      stream << ", ";
    }
    stream << *get_slot(i);
  }
  stream << "]";
}

template <ArrayElement T>
std::ostream &operator<<(std::ostream &stream, const Array<T> &array) {
  array.print(stream);
  return stream;
}

template <ArrayElement T> void Array<T>::pop_back() {
#ifdef ARRAY_DEBUG_MODE
  if (empty()) {
    throw std::invalid_argument("Empty array");
  }
#endif
  pointer target = get_slot(current_size_ - 1);
  target->~T();

  --current_size_;

  if (current_size_ % block_size_ == 0) {
    remove_block();
  }
}

template <ArrayElement T> void Array<T>::clear() noexcept {
  for (size_type i = current_size_; i > 0; --i) {
    pop_back();
  }
}

template <ArrayElement T>
Array<T>::Array(size_type count, const_reference item)
    : block_size_(DEFAULT_BLOCK_SIZE), current_size_(0), current_capacity_(0) {
  static_assert(std::copy_constructible<T>,
                "Repeating constructor requires T to be copy constructible");
  try {
    for (size_type i = 0; i < count; ++i) {
      push_back(item);
    }
  } catch (...) {
    clear();
    throw;
  }
}

template <ArrayElement T>
Array<T>::Array(std::initializer_list<T> items)
    : block_size_(DEFAULT_BLOCK_SIZE), current_size_(0), current_capacity_(0) {
  static_assert(
      std::copy_constructible<T>,
      "Initializer list constructor requires T to be copy constructible");
  try {
    for (const auto &item : items) {
      push_back(item);
    }
  } catch (...) {
    clear();
    throw;
  }
}

template <ArrayElement T>
template <std::input_iterator InputIterator>
Array<T>::Array(InputIterator first, InputIterator last)
    : block_size_(DEFAULT_BLOCK_SIZE), current_size_(0), current_capacity_(0) {
  using IterRef = typename std::iterator_traits<InputIterator>::reference;
  static_assert(std::constructible_from<T, IterRef>,
                "T must be constructible from the iterator's reference type");
  try {
    for (auto it = first; it != last; ++it) {
      push_back(*it);
    }
  } catch (...) {
    clear();
    throw;
  }
}

template <ArrayElement T> T &Array<T>::at(size_type index) {
  if (index >= current_size_) {
    throw std::out_of_range("Invalid index");
  }
  return data_[index / block_size_][index % block_size_];
}

template <ArrayElement T> const T &Array<T>::at(size_type index) const {
  if (index >= current_size_) {
    throw std::out_of_range("Invalid index");
  }
  return data_[index / block_size_][index % block_size_];
}

template <ArrayElement T> T &Array<T>::operator[](size_type index) {
#ifdef ARRAY_DEBUG_MODE
  if (index >= current_size_) {
    throw std::out_of_range("Invalid index");
  }
#endif
  return data_[index / block_size_][index % block_size_];
}

template <ArrayElement T> const T &Array<T>::operator[](size_type index) const {
#ifdef ARRAY_DEBUG_MODE
  if (index >= current_size_) {
    throw std::out_of_range("Invalid index");
  }
#endif
  return data_[index / block_size_][index % block_size_];
}

template <ArrayElement T> void swap(Array<T> &a, Array<T> &b) noexcept {
  using std::swap;
  swap(a.data_, b.data_);
  swap(a.block_size_, b.block_size_);
  swap(a.current_size_, b.current_size_);
  swap(a.current_capacity_, b.current_capacity_);
}

template <ArrayElement T>
Array<T>::Array(const Array &other)
    : block_size_(other.block_size_), current_size_(0), current_capacity_(0) {
  static_assert(
      std::copy_constructible<T>,
      "Array::Array(const Array&) requires T to be copy constructible");
  try {
    for (size_type i = 0; i < other.current_size_; ++i) {
      push_back(other.at(i));
    }
  } catch (...) {
    clear();
    throw;
  }
}

template <ArrayElement T>
Array<T>::Array(Array &&other) noexcept
    : block_size_(other.block_size_), current_size_(0), current_capacity_(0) {
  swap(*this, other);
}

template <ArrayElement T> Array<T> &Array<T>::operator=(const Array &other) {
  static_assert(std::copy_constructible<T>,
                "Array::operator= requires T to be copy constructible");
  if (this != &other) {
    Array temp(other);
    swap(*this, temp);
  }
  return *this;
}

template <ArrayElement T>
Array<T> &Array<T>::operator=(Array &&other) noexcept {
  if (this != &other) {
    clear();
    block_size_ = other.block_size_;
    swap(*this, other);
  }
  return *this;
}

} // namespace lib

#endif // ARRAY_H
