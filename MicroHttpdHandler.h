/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   MicroHttpdhandler.h
 * Author: dront
 *
 * Created on 18 декабря 2015 г., 17:14
 */

#ifndef MICROHTTPDHANDLER_H
#define MICROHTTPDHANDLER_H

#include <microhttpd.h>
#include <stdint.h>
#include "SignalSlot.h"

class MicroHttpdHandler {
public:
    MicroHttpdHandler();
    virtual ~MicroHttpdHandler();
    bool Start(uint16_t port);
    void Stop();
    bool IsValid() const;
    Signal<const std::string & /* url */, const std::string & /* method */, const std::string & /* version */, std::string & /* result */> signalGet;
    Signal<const std::string & /* url */, const std::string & /* method */, const std::string & /* version */, std::string & /* result */> signalPut;
    Signal<const std::string & /* url */, const std::string & /* method */, const std::string & /* version */, std::string & /* result */> signalDelete;


private:
    MicroHttpdHandler(const MicroHttpdHandler & orig);
    MHD_Daemon * m_Daemon;
    static thread_local char * mUriBuffer;
    std::string TaskGet(const std::string url, const std::string method, const std::string version) const;
    std::string TaskPut(const std::string url, const std::string method, const std::string version) const;
    std::string TaskDelete(const std::string url, const std::string method, const std::string version) const;
    enum {
        URI_BUFFER_SIZE = 255,
        TASK_WAIT_GET = 100,
        TASK_WAIT_PUT = 1000,
        TASK_WAIT_DELETE = 1000,
        CONNECTION_LIMIT = 100,
        CONNECTION_TIMEOUT = 4000,
        THREAD_POOL_SIZE = 4
    };

    static int My_MHD_AccessHandlerCallback(void *cls,
            struct MHD_Connection *connection,
            const char *url,
            const char *method,
            const char *version,
            const char *upload_data,
            size_t *upload_data_size,
            void **con_cls);
    static void * My_URI_LoggerCallback(void *cls, const char *uri);
    std::string Handle(const char *url, const char *method, const char *version, int & ret);
    static const char * const METHOD_GET;
    static const size_t METHOD_GET_LEN;
    static const char * const METHOD_PUT;
    static const size_t METHOD_PUT_LEN;
    static const char * const METHOD_DELETE;
    static const size_t METHOD_DELETE_LEN;
    static const char * const RESPONSE_ERROR;
    static const size_t RESPONSE_ERROR_LEN;
};

#endif /* MICROHTTPDHANDLER_H */

