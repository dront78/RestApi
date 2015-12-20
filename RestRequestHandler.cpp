/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   RestRequestHandler.cpp
 * Author: dront
 *
 * Created on 19 декабря 2015 г., 13:16
 */

#include <cassert>
#include <algorithm>
#include "RestRequestHandler.h"

RestRequestHandler::RestRequestHandler() : mUserId(0) {
}

RestRequestHandler::RestRequestHandler(const RestRequestHandler& /* orig */) {
    assert(0);
}

RestRequestHandler::~RestRequestHandler() {
}

UserDescriptionSet RestRequestHandler::GetAllUserList() {
    const scoped_shared_rlock _lock(mSharedMtx);
    return mUserDescriptionSet;
}

UserDescriptionSet RestRequestHandler::GetFilterUserList(const UserDescription & Filter) {
    const scoped_shared_rlock _lock(mSharedMtx);
    std::pair<NameMap::iterator, NameMap::iterator> FirstNameRange(!Filter.FirstName.empty()
            ? mFirstNameMap.equal_range(Filter.FirstName)
            : std::pair<NameMap::iterator, NameMap::iterator> (mFirstNameMap.begin(), mFirstNameMap.end())
            );
    UserDescriptionSet FirstNameSet;
    for (NameMap::iterator _it = FirstNameRange.first; _it != FirstNameRange.second; ++_it) {
        FirstNameSet.insert(UserDescription((*_it).second));
    }

    std::pair<NameMap::iterator, NameMap::iterator> LastNameRange(!Filter.LastName.empty()
            ? mLastNameMap.equal_range(Filter.LastName)
            : std::pair<NameMap::iterator, NameMap::iterator> (mLastNameMap.begin(), mLastNameMap.end())
            );
    UserDescriptionSet LastNameSet;
    for (NameMap::iterator _it = LastNameRange.first; _it != LastNameRange.second; ++_it) {
        LastNameSet.insert(UserDescription((*_it).second));
    }

    UserDescriptionSet SetIntersectPhase1;
    std::set_intersection(FirstNameSet.begin(), FirstNameSet.end(), LastNameSet.begin(), LastNameSet.end(), std::inserter(SetIntersectPhase1, SetIntersectPhase1.begin()));

    std::pair<GenderMap::iterator, GenderMap::iterator> GenderRange(Filter.Gender != UserDescription::U
            ? mGenderMap.equal_range(Filter.Gender)
            : std::pair<GenderMap::iterator, GenderMap::iterator> (mGenderMap.begin(), mGenderMap.end())
            );
    UserDescriptionSet GenderSet;
    for (GenderMap::iterator _it = GenderRange.first; _it != GenderRange.second; ++_it) {
        GenderSet.insert(UserDescription((*_it).second));
    }
    UserDescriptionSet SetIntersectPhase2;
    std::set_intersection(SetIntersectPhase1.begin(), SetIntersectPhase1.end(), GenderSet.begin(), GenderSet.end(), std::inserter(SetIntersectPhase2, SetIntersectPhase2.begin()));

    std::pair<AgeMap::iterator, AgeMap::iterator> AgeRange(Filter.Age != 0
            ? mAgeMap.equal_range(Filter.Age)
            : std::pair<AgeMap::iterator, AgeMap::iterator> (mAgeMap.begin(), mAgeMap.end())
            );
    UserDescriptionSet AgeSet;
    for (AgeMap::iterator _it = AgeRange.first; _it != AgeRange.second; ++_it) {
        AgeSet.insert(UserDescription((*_it).second));
    }
    UserDescriptionSet SetIntersectPhase3;
    std::set_intersection(SetIntersectPhase2.begin(), SetIntersectPhase2.end(), AgeSet.begin(), AgeSet.end(), std::inserter(SetIntersectPhase3, SetIntersectPhase3.begin()));

    UserDescriptionSet _Result;
    UserDescriptionSet::const_iterator _it(SetIntersectPhase3.begin());
    while (SetIntersectPhase3.end() != _it) {
        const UserDescription & Desc(*_it);
        _Result.insert(*mUserDescriptionSet.find(Desc));
        ++_it;
    }

    return _Result;
}

bool RestRequestHandler::AddUser(const UserDescription & User) {
    const scoped_shared_wlock _lock(mSharedMtx);
    std::pair < UserDescriptionSet::iterator, bool> _R(mUserDescriptionSet.insert(User));
    mFirstNameMap.insert(NameMap::value_type(User.FirstName, User.Id));
    mLastNameMap.insert(NameMap::value_type(User.LastName, User.Id));
    mGenderMap.insert(GenderMap::value_type(User.Gender, User.Id));
    mAgeMap.insert(AgeMap::value_type(User.Age, User.Id));
    return _R.second;
}

bool RestRequestHandler::DeleteUser(const UserDescription & Filter) {
    // code duplicated with GetFilterUserList to prevent equal_range recall fter Phase3
    const scoped_shared_wlock _lock(mSharedMtx);
    std::pair<NameMap::iterator, NameMap::iterator> FirstNameRange(!Filter.FirstName.empty()
            ? mFirstNameMap.equal_range(Filter.FirstName)
            : std::pair<NameMap::iterator, NameMap::iterator> (mFirstNameMap.begin(), mFirstNameMap.end())
            );
    UserDescriptionSet FirstNameSet;
    for (NameMap::iterator _it = FirstNameRange.first; _it != FirstNameRange.second; ++_it) {
        FirstNameSet.insert(UserDescription((*_it).second));
    }

    std::pair<NameMap::iterator, NameMap::iterator> LastNameRange(!Filter.LastName.empty()
            ? mLastNameMap.equal_range(Filter.LastName)
            : std::pair<NameMap::iterator, NameMap::iterator> (mLastNameMap.begin(), mLastNameMap.end())
            );
    UserDescriptionSet LastNameSet;
    for (NameMap::iterator _it = LastNameRange.first; _it != LastNameRange.second; ++_it) {
        LastNameSet.insert(UserDescription((*_it).second));
    }

    UserDescriptionSet SetIntersectPhase1;
    std::set_intersection(FirstNameSet.begin(), FirstNameSet.end(), LastNameSet.begin(), LastNameSet.end(), std::inserter(SetIntersectPhase1, SetIntersectPhase1.begin()));

    std::pair<GenderMap::iterator, GenderMap::iterator> GenderRange(Filter.Gender != UserDescription::U
            ? mGenderMap.equal_range(Filter.Gender)
            : std::pair<GenderMap::iterator, GenderMap::iterator> (mGenderMap.begin(), mGenderMap.end())
            );
    UserDescriptionSet GenderSet;
    for (GenderMap::iterator _it = GenderRange.first; _it != GenderRange.second; ++_it) {
        GenderSet.insert(UserDescription((*_it).second));
    }
    UserDescriptionSet SetIntersectPhase2;
    std::set_intersection(SetIntersectPhase1.begin(), SetIntersectPhase1.end(), GenderSet.begin(), GenderSet.end(), std::inserter(SetIntersectPhase2, SetIntersectPhase2.begin()));

    std::pair<AgeMap::iterator, AgeMap::iterator> AgeRange(Filter.Age != 0
            ? mAgeMap.equal_range(Filter.Age)
            : std::pair<AgeMap::iterator, AgeMap::iterator> (mAgeMap.begin(), mAgeMap.end())
            );
    UserDescriptionSet AgeSet;
    for (AgeMap::iterator _it = AgeRange.first; _it != AgeRange.second; ++_it) {
        AgeSet.insert(UserDescription((*_it).second));
    }
    UserDescriptionSet SetIntersectPhase3;
    std::set_intersection(SetIntersectPhase2.begin(), SetIntersectPhase2.end(), AgeSet.begin(), AgeSet.end(), std::inserter(SetIntersectPhase3, SetIntersectPhase3.begin()));

    UserDescriptionSet::iterator _it(SetIntersectPhase3.begin());
    while (SetIntersectPhase3.end() != _it) {
        const UserDescription & Desc(*_it);
        for (NameMap::iterator _it = FirstNameRange.first; _it != FirstNameRange.second;) {
            if ((*_it).second == Desc.Id) {
                _it = mFirstNameMap.erase(_it);
            } else {
                ++_it;
            }
        }

        for (NameMap::iterator _it = LastNameRange.first; _it != LastNameRange.second;) {
            if ((*_it).second == Desc.Id) {
                _it = mLastNameMap.erase(_it);
            } else {
                ++_it;
            }
        }

        for (GenderMap::iterator _it = GenderRange.first; _it != GenderRange.second;) {
            if ((*_it).second == Desc.Id) {
                _it = mGenderMap.erase(_it);
            } else {
                ++_it;
            }
        }

        for (AgeMap::iterator _it = AgeRange.first; _it != AgeRange.second;) {
            if ((*_it).second == Desc.Id) {
                _it = mAgeMap.erase(_it);
            } else {
                ++_it;
            }
        }

        mUserDescriptionSet.erase((*_it));
        ++_it;
    }
    return (!SetIntersectPhase3.empty());
}

uint64_t RestRequestHandler::NextUserId() {
    return ++mUserId;
}
