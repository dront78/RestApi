/*
 * File:   shared_mutex.h
 * regerence based http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2406.html#shared_mutex
 */

#ifndef SHARED_MUTEX_H
#define SHARED_MUTEX_H

#include <mutex>
#include <condition_variable>
#include <chrono>
#include <climits>
#include <system_error>

class shared_mutex {
    typedef std::mutex mutex_t;
    typedef std::condition_variable cond_t;
    typedef unsigned count_t;

    mutex_t mut_;
    cond_t gate1_;
    cond_t gate2_;
    count_t state_;

    static const count_t write_entered_ = 1U << (sizeof (count_t) * CHAR_BIT - 1);
    static const count_t n_readers_ = ~write_entered_;

public:
    shared_mutex();
    ~shared_mutex();

    shared_mutex(const shared_mutex&) = delete;
    shared_mutex& operator=(const shared_mutex&) = delete;

    // Exclusive ownership

    void lock();
    bool try_lock();

    template <class Rep, class Period>
    bool try_lock_for(const std::chrono::duration<Rep, Period>& rel_time) {
        return try_lock_until(std::chrono::steady_clock::now() + rel_time);
    }
    template <class Clock, class Duration>
    bool
    try_lock_until(
            const std::chrono::time_point<Clock, Duration>& abs_time);
    void unlock();

    // Shared ownership

    void lock_shared();
    bool try_lock_shared();

    template <class Rep, class Period>
    bool
    try_lock_shared_for(const std::chrono::duration<Rep, Period>& rel_time) {
        return try_lock_shared_until(std::chrono::steady_clock::now() +
                rel_time);
    }
    template <class Clock, class Duration>
    bool
    try_lock_shared_until(
            const std::chrono::time_point<Clock, Duration>& abs_time);
    void unlock_shared();
};

class scoped_shared_wlock {
public:
    explicit scoped_shared_wlock(shared_mutex & lock);
    ~scoped_shared_wlock();
private:
    shared_mutex & mLock;
    scoped_shared_wlock(const scoped_shared_wlock & other);
    scoped_shared_wlock & operator=(const scoped_shared_wlock & other);
};

class scoped_shared_rlock {
public:
    explicit scoped_shared_rlock(shared_mutex & lock);
    ~scoped_shared_rlock();
private:
    shared_mutex & mLock;
    scoped_shared_rlock(const scoped_shared_rlock & other);
    scoped_shared_rlock & operator=(const scoped_shared_rlock & other);
};

#endif /* SHARED_MUTEX_H */

