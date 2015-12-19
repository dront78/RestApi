/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   RestServer.h
 * Author: dront
 *
 * Created on 19 декабря 2015 г., 18:43
 */

#ifndef RESTSERVER_H
#define RESTSERVER_H

#include <stdint.h>
#include <iostream>
#include "MicroHttpdHandler.h"
#include "RestRequestHandler.h"

class RestServer {
public:
    RestServer();
    virtual ~RestServer();
    bool Start(uint16_t port);
    void Wait();
private:
    void OnMethodGet(const std::string url, const std::string method, const std::string version);
    void OnMethodPut(const std::string url, const std::string method, const std::string version);
    void OnMethodDelete(const std::string url, const std::string method, const std::string version);
    RestServer(const RestServer& orig);
    MicroHttpdHandler * mHttpHandler;
    RestRequestHandler * mRestRequestHandler;
};

#endif /* RESTSERVER_H */
