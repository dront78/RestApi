/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   RestServer.cpp
 * Author: dront
 *
 * Created on 19 декабря 2015 г., 18:43
 */

#include "RestServer.h"

RestServer::RestServer()
: mHttpHandler(new MicroHttpdHandler()), mRestRequestHandler(new RestRequestHandler) {
}

RestServer::RestServer(const RestServer& /* orig */) {
}

RestServer::~RestServer() {
    mHttpHandler->Stop();
    delete mRestRequestHandler;
    delete mHttpHandler;
}

bool RestServer::Start(uint16_t port) {
    mHttpHandler->signalGet.connect_member(&RestServer::OnMethodGet, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
    mHttpHandler->signalPut.connect_member(&RestServer::OnMethodPut, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
    mHttpHandler->signalDelete.connect_member(&RestServer::OnMethodDelete, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
    return mHttpHandler->Start(port);
}

void RestServer::Wait() {
    std::cin.get();
}

void RestServer::OnMethodGet(const std::string & url, const std::string & method, const std::string & version, std::string & result, WasteHandler & taskCallback) {
    std::cout << method << ' ' << url << ' ' << version << std::endl;
    const UserDescriptionSet & DescSet(mRestRequestHandler->GetFilterUserList(Parse(url)));
    UserDescriptionSet::const_iterator _it(DescSet.begin());
    std::stringstream mGetResponse;
    mGetResponse << "<!DOCTYPE html><html><head></head><body>";
    while (DescSet.end() != _it) {
        const UserDescription Desc(*_it);
        mGetResponse << Desc.FirstName << "&#44;" << Desc.LastName << "&#44;" << static_cast<char> (Desc.Gender) << "&#44;" << static_cast<int> (Desc.Age) << "</br>";
        ++_it;
    }
    mGetResponse << "</body></html>" << '\n';
    result = mGetResponse.str();
    taskCallback.OnTaskFinished();
}

void RestServer::OnMethodPut(const std::string & url, const std::string & method, const std::string & version, std::string & result, WasteHandler & taskCallback) {
    std::cout << method << ' ' << url << ' ' << version << std::endl;
    mRestRequestHandler->AddUser(Parse(url, mRestRequestHandler->NextUserId()));
    taskCallback.OnTaskFinished();
}

void RestServer::OnMethodDelete(const std::string & url, const std::string & method, const std::string & version, std::string & result, WasteHandler & taskCallback) {
    std::cout << method << ' ' << url << ' ' << version << std::endl;
    mRestRequestHandler->DeleteUser(Parse(url));
    taskCallback.OnTaskFinished();
}

UserDescription RestServer::Parse(const std::string & url, uint64_t id) {
    UserDescription D(id);
    if (ValidateUrl(url)) {
        D.FirstName = GetFirstName(url);
        D.LastName = GetLastName(url);
        D.Gender = GetGender(url);
        D.Age = GetAge(url);
    }
    return D;
}

bool RestServer::ValidateUrl(const std::string & url) const {
    // simplest uri parser. TODO: to use rapidjson library here
    std::string::size_type _h1(url.find("/user?"));
    std::string::size_type _h2(url.find('='));
    return ((0 == _h1) && (std::string::npos != _h2));
}

std::string RestServer::GetFirstName(const std::string & url) const {
    // simplest uri parser. TODO: to use rapidjson library here
    std::string::size_type _h1(url.find("firstname="));
    std::string::size_type _h2(url.find('&', _h1));
    const std::string _R((std::string::npos != _h1) ? std::string(url.begin() + _h1 + 10, (std::string::npos != _h2) ? url.begin() + _h2 : url.end()) : "");
    return _R;
}

std::string RestServer::GetLastName(const std::string & url) const {
    // simplest uri parser. TODO: to use rapidjson library here
    std::string::size_type _h1(url.find("lastname="));
    std::string::size_type _h2(url.find('&', _h1));
    const std::string _R((std::string::npos != _h1) ? std::string(url.begin() + _h1 + 9, (std::string::npos != _h2) ? url.begin() + _h2 : url.end()) : "");
    return _R;
}

UserDescription::GenderDescription RestServer::GetGender(const std::string & url) const {
    // simplest uri parser. TODO: to use rapidjson library here
    std::string::size_type _h1(url.find("gender="));
    std::string::size_type _h2(url.find('&', _h1));
    const std::string _R((std::string::npos != _h1) ? std::string(url.begin() + _h1 + 7, (std::string::npos != _h2) ? url.begin() + _h2 : url.end()) : "");
    return !_R.empty() ? static_cast<UserDescription::GenderDescription> (_R.at(0)) : static_cast<UserDescription::GenderDescription> (UserDescription::U);
}

char RestServer::GetAge(const std::string & url) const {
    // simplest uri parser. TODO: to use rapidjson library here
    std::string::size_type _h1(url.find("age="));
    std::string::size_type _h2(url.find('&', _h1));
    const std::string _R((std::string::npos != _h1) ? std::string(url.begin() + _h1 + 4, (std::string::npos != _h2) ? url.begin() + _h2 : url.end()) : "");
    char Value(0);
    try {
        if (!_R.empty()) Value = std::stoi(_R);
    } catch (...) {
        // TODO: error handling
    }
    return Value;
}


