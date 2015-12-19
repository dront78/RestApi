/*
 * File:   shared_mutex.cpp
 * reference based http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2406.html#shared_mutex
 */

#include <cassert>
#include "shared_mutex.h"

shared_mutex::shared_mutex()
: state_(0) {
}

shared_mutex::~shared_mutex() {
    std::lock_guard<mutex_t> _(mut_);
}

// Exclusive ownership

void shared_mutex::lock() {
    std::unique_lock<mutex_t> lk(mut_);
    while (state_ & write_entered_)
        gate1_.wait(lk);
    state_ |= write_entered_;
    while (state_ & n_readers_)
        gate2_.wait(lk);
}

bool shared_mutex::try_lock() {
    std::unique_lock<mutex_t> lk(mut_);
    if (state_ == 0) {
        state_ = write_entered_;
        return true;
    }
    return false;
}

void shared_mutex::unlock() {
    std::lock_guard<mutex_t> _(mut_);
    state_ = 0;
    gate1_.notify_all();
}

// Shared ownership

void shared_mutex::lock_shared() {
    std::unique_lock<mutex_t> lk(mut_);
    while ((state_ & write_entered_) || (state_ & n_readers_) == n_readers_)
        gate1_.wait(lk);
    count_t num_readers = (state_ & n_readers_) + 1;
    state_ &= ~n_readers_;
    state_ |= num_readers;
}

bool shared_mutex::try_lock_shared() {
    std::unique_lock<mutex_t> lk(mut_);
    count_t num_readers = state_ & n_readers_;
    if (!(state_ & write_entered_) && num_readers != n_readers_) {
        ++num_readers;
        state_ &= ~n_readers_;
        state_ |= num_readers;
        return true;
    }
    return false;
}

void shared_mutex::unlock_shared() {
    std::lock_guard<mutex_t> _(mut_);
    count_t num_readers = (state_ & n_readers_) - 1;
    state_ &= ~n_readers_;
    state_ |= num_readers;
    if (state_ & write_entered_) {
        if (num_readers == 0)
            gate2_.notify_one();
    } else {
        if (num_readers == n_readers_ - 1)
            gate1_.notify_one();
    }
}

// scoped_shared_wlock

scoped_shared_wlock::scoped_shared_wlock(shared_mutex & lock)
: mLock(lock) {
    mLock.lock();
}

scoped_shared_wlock::~scoped_shared_wlock() {
    mLock.unlock();
}

scoped_shared_wlock::scoped_shared_wlock(const scoped_shared_wlock & other)
: mLock(other.mLock) {
    assert(0);
}

scoped_shared_wlock & scoped_shared_wlock::operator=(const scoped_shared_wlock & /* other */) {
    return *this;
}

// scoped_shared_rlock

scoped_shared_rlock::scoped_shared_rlock(shared_mutex & lock)
: mLock(lock) {
    mLock.lock_shared();
}

scoped_shared_rlock::~scoped_shared_rlock() {
    mLock.unlock_shared();
}

scoped_shared_rlock::scoped_shared_rlock(const scoped_shared_rlock & other)
: mLock(other.mLock) {
    assert(0);
}

scoped_shared_rlock & scoped_shared_rlock::operator=(const scoped_shared_rlock & /* other */) {
    return *this;
}
