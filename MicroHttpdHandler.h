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
#include <atomic>
#include "SignalSlot.h"
#include "WasteHandler.h"

class MicroHttpdHandler : private WasteHandler {
public:
    MicroHttpdHandler();
    virtual ~MicroHttpdHandler();
    bool Start(uint16_t port);
    void Stop();
    bool IsValid() const;
    void OnTaskFinished();
    Signal<const std::string & /* url */, const std::string & /* method */, const std::string & /* version */, std::string & /* result */, WasteHandler & /* taskCallback */> signalGet;
    Signal<const std::string & /* url */, const std::string & /* method */, const std::string & /* version */, std::string & /* result */, WasteHandler & /* taskCallback */> signalPut;
    Signal<const std::string & /* url */, const std::string & /* method */, const std::string & /* version */, std::string & /* result */, WasteHandler & /* taskCallback */> signalDelete;


private:
    MicroHttpdHandler(const MicroHttpdHandler & orig);
    MHD_Daemon * m_Daemon;
    std::atomic<int> m_WasteLoad;
    static thread_local char * mUriBuffer;
    std::string TaskGet(const std::string url, const std::string method, const std::string version);
    std::string TaskPut(const std::string url, const std::string method, const std::string version);
    std::string TaskDelete(const std::string url, const std::string method, const std::string version);
    enum {
        URI_BUFFER_SIZE = 255,
        TASK_WAIT_GET = 100,
        TASK_WAIT_PUT = 1000,
        TASK_WAIT_DELETE = 1000,
        CONNECTION_LIMIT = 100,
        CONNECTION_TIMEOUT = 4000,
        THREAD_POOL_SIZE = 4,
        WASTELOAD = 1000
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

