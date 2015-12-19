/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   RestRequestHandler.h
 * Author: dront
 *
 * Created on 19 декабря 2015 г., 13:16
 */

#ifndef RESTREQUESTHANDLER_H
#define RESTREQUESTHANDLER_H

#include <string>
#include <map>
#include <atomic>
#include "UserDesc.h"
#include "shared_mutex.h"

class RestRequestHandler {
public:
    RestRequestHandler();
    virtual ~RestRequestHandler();
    UserDescriptionSet GetAllUserList();
    UserDescriptionSet GetFilterUserList(const UserDescription & Filter);
    bool AddUser(const UserDescription & User);
    bool DeleteUser(const UserDescription & User);
    uint64_t NextUserId();

private:
    RestRequestHandler(const RestRequestHandler& orig);

    /* search indexes */
    typedef std::multimap<std::string /* F/L Name */, uint64_t /* Id */ > NameMap;
    typedef std::multimap<UserDescription::GenderDescription /* Gender */, uint64_t /* Id */ > GenderMap;
    typedef std::multimap<char /* Age */, uint64_t /* Id */ > AgeMap;

    std::atomic<uint64_t> mUserId;
    shared_mutex mSharedMtx;
    UserDescriptionSet mUserDescriptionSet;
    NameMap mFirstNameMap;
    NameMap mLastNameMap;
    GenderMap mGenderMap;
    AgeMap mAgeMap;
};

#endif /* RESTREQUESTHANDLER_H */

