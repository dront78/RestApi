/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   UserDesc.h
 * Author: dront
 *
 * Created on 19 декабря 2015 г., 14:18
 */

#ifndef USERDESC_H
#define USERDESC_H

#include <set>

struct UserDescription {

    enum GenderDescription {
        U, // undefined
        M, // male
        F // female
    };

    const uint64_t Id;
    std::string FirstName;
    std::string LastName;
    GenderDescription Gender;
    char Age;

    explicit UserDescription(const uint64_t & id) : Id(id), Gender(U), Age(0) {

    }

    ~UserDescription() {

    }

    UserDescription(const uint64_t & id, const std::string & firstName, const std::string & lastName = "",
            const GenderDescription & gender = U, const char & age = 0)
    : Id(id), FirstName(firstName), LastName(lastName), Gender(gender), Age(age) {

    }

    bool operator<(const UserDescription & other) const {
        return (Id < other.Id);
    }

    bool operator==(const UserDescription & other) const {
        return (Id == other.Id);
    }
};

typedef std::set<UserDescription> UserDescriptionSet;

#endif /* USERDESC_H */

