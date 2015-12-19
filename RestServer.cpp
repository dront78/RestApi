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
    mHttpHandler->signalGet.connect_member(&RestServer::OnMethodGet, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    mHttpHandler->signalPut.connect_member(&RestServer::OnMethodPut, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    mHttpHandler->signalDelete.connect_member(&RestServer::OnMethodDelete, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    return mHttpHandler->Start(port);
}

void RestServer::Wait() {
    std::cin.get();
}

void RestServer::OnMethodGet(const std::string url, const std::string method, const std::string version) {
    std::cout << method << ' ' << url << ' ' << version << std::endl;
}

void RestServer::OnMethodPut(const std::string url, const std::string method, const std::string version) {
    std::cout << method << ' ' << url << ' ' << version << std::endl;
}

void RestServer::OnMethodDelete(const std::string url, const std::string method, const std::string version) {
    std::cout << method << ' ' << url << ' ' << version << std::endl;
}

