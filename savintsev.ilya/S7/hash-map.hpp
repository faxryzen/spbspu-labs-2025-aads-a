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

  public:

    HashMap(size_t size);

    void insert_data(const Key & k, const T & t);
  };

  template< typename Key, typename T, typename HS1, typename HS2, typename EQ >
  HashMap< Key, T, HS1, HS2, EQ >::HashMap(size_t size):
    t1_(size),
    t2_(size),
    capacity_(size)
  {}
  template< typename Key, typename T, typename HS1, typename HS2, typename EQ >
  void HashMap< Key, T, HS1, HS2, EQ >::insert_data(const Key & k, const T & t)
  {
    Key current_key = k;
    T current_t = t;

    for (size_t i = 0; i < MAX_ITERATIONS; ++i)
    {
      size_t h1 = HS1{}(current_key) % capacity_;
      if (t1_[h1].second)
      {
        t1_[h1] = {{current_key, current_t}, false};
        return;
      }

      auto displaced = t1_[h1];
      t1_[h1] = {{current_key, current_t}, false};
      current_key = displaced.first.first;
      current_t = displaced.first.second;

      size_t h2 = HS2{}(current_key) % capacity_;
      if (t2_[h2].second)
      {
        t2_[h2] = {{current_key, current_t}, false};
        return;
      }

      displaced = t2_[h2];
      t2_[h2] = {{current_key, current_t}, false};
      current_key = displaced.first.first;
      current_t = displaced.first.second;
    }

    throw std::runtime_error("hashmap: cuckoo failed: iteration limit exceeded");
  }
}

#endif
