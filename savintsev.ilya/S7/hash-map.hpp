#ifndef HASH_MAP_HPP
#define HASH_MAP_HPP
#include <dynamic-array.hpp>

namespace savintsev
{
  template< typename Key, typename T, typename HS1, typename HS2, typename EQ >
  class HashMap
  {
  private:
    Array< std::pair< std::pair< Key, T >, bool > > t1_;
    Array< std::pair< std::pair< Key, T >, bool > > t2_;

    size_t capacity_;
    size_t size_ = 0;

    static constexpr size_t MAX_ITERATIONS = 100;

    T & insert_data(const Key & k);
    void delete_data(const Key & k);
    bool lookup_data(const Key & k);
    std::pair< T &, bool> get_data(const Key & k);
  public:

    HashMap(size_t size);

    T & operator[](const Key & k);
  };

  template< typename Key, typename T, typename HS1, typename HS2, typename EQ >
  HashMap< Key, T, HS1, HS2, EQ >::HashMap(size_t size):
    t1_(size),
    t2_(size),
    capacity_(size)
  {
    for (size_t i = 0; i < capacity_; ++i)
    {
      t1_[i].second = true;
      t2_[i].second = true;
    }
  }
  template< typename Key, typename T, typename HS1, typename HS2, typename EQ >
  T & HashMap< Key, T, HS1, HS2, EQ >::operator[](const Key & k)
  {
    auto data = get_data(k);
    if (data.second)
    {
      return data.first;
    }
    return insert_data(k);
  }
  template< typename Key, typename T, typename HS1, typename HS2, typename EQ >
  T & HashMap< Key, T, HS1, HS2, EQ >::insert_data(const Key & k)
  {
    Key current_key = k;
    T current_t = T{};

    for (size_t i = 0; i < MAX_ITERATIONS; ++i)
    {
      size_t h1 = HS1{}(current_key) % capacity_;
      if (t1_[h1].second)
      {
        t1_[h1] = {{current_key, current_t}, false};
        return t1_[h1].first.second;
      }

      auto displaced = t1_[h1];
      t1_[h1] = {{current_key, current_t}, false};
      current_key = displaced.first.first;
      current_t = displaced.first.second;

      size_t h2 = HS2{}(current_key) % capacity_;
      if (t2_[h2].second)
      {
        t2_[h2] = {{current_key, current_t}, false};
        return t2_[h2].first.second;;
      }

      displaced = t2_[h2];
      t2_[h2] = {{current_key, current_t}, false};
      current_key = displaced.first.first;
      current_t = displaced.first.second;
    }

    throw std::runtime_error("hashmap: cuckoo failed: iteration limit exceeded");
  }
  template< typename Key, typename T, typename HS1, typename HS2, typename EQ >
  void HashMap< Key, T, HS1, HS2, EQ >::delete_data(const Key & k)
  {
    Key current_key = k;

    size_t h1 = HS1{}(current_key) % capacity_;
    if (!t1_[h1].second)
    {
      Key old_key = t1_[h1].first.first;
      if (!(EQ{}(current_key, old_key) || EQ{}(old_key, current_key)))
      {
        t1_[h1].second = true;
        --size_;
        return;
      }
    }

    size_t h2 = HS2{}(current_key) % capacity_;
    if (!t2_[h2].second)
    {
      Key old_key = t2_[h2].first.first;
      if (!(EQ{}(current_key, old_key) || EQ{}(old_key, current_key)))
      {
        t2_[h2].second = true;
        --size_;
        return;
      }
    }
  }
  template< typename Key, typename T, typename HS1, typename HS2, typename EQ >
  bool HashMap< Key, T, HS1, HS2, EQ >::lookup_data(const Key & k)
  {
    return get_data(k).second;
  }
  template< typename Key, typename T, typename HS1, typename HS2, typename EQ >
  std::pair< T &, bool> HashMap< Key, T, HS1, HS2, EQ >::get_data(const Key & k)
  {
    Key current_key = k;

    size_t h1 = HS1{}(current_key) % capacity_;
    if (!t1_[h1].second)
    {
      Key old_key = t1_[h1].first.first;
      if (!(EQ{}(current_key, old_key) || EQ{}(old_key, current_key)))
      {
        return {t1_[h1].first.second, true};
      }
    }

    size_t h2 = HS2{}(current_key) % capacity_;
    if (!t2_[h2].second)
    {
      Key old_key = t2_[h2].first.first;
      if (!(EQ{}(current_key, old_key) || EQ{}(old_key, current_key)))
      {
        return {t2_[h2].first.second, true};
      }
    }
    return {T{}, false};
  }
}

#endif
