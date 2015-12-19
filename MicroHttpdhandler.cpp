/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   MicroHttpdhandler.cpp
 * Author: dront
 *
 * Created on 18 декабря 2015 г., 17:14
 */
#include <cassert>
#include <string.h>
#include <ctype.h>
#include "MicroHttpdHandler.h"

const char * const MicroHttpdHandler::METHOD_GET("GET");
const size_t MicroHttpdHandler::METHOD_GET_LEN(3);
const char * const MicroHttpdHandler::METHOD_PUT("PUT");
const size_t MicroHttpdHandler::METHOD_PUT_LEN(3);
const char * const MicroHttpdHandler::METHOD_DELETE("DELETE");
const size_t MicroHttpdHandler::METHOD_DELETE_LEN(6);

MicroHttpdHandler::MicroHttpdHandler() : m_Daemon(0), mUriBuffer(new char[URI_BUFFER_SIZE + 1]) {
    mUriBuffer[URI_BUFFER_SIZE] = 0;
}

MicroHttpdHandler::MicroHttpdHandler(const MicroHttpdHandler& /* orig */) {
    assert(0);
}

MicroHttpdHandler::~MicroHttpdHandler() {
    Stop();
    delete [] mUriBuffer;
}

bool MicroHttpdHandler::Start(uint16_t port) {
    struct MHD_OptionItem ops[] = {
        { MHD_OPTION_CONNECTION_LIMIT, 100, NULL},
        { MHD_OPTION_CONNECTION_TIMEOUT, 10, NULL},
        { MHD_OPTION_THREAD_POOL_SIZE, 4, NULL},
        { MHD_OPTION_URI_LOG_CALLBACK, reinterpret_cast<intptr_t> (&MicroHttpdHandler::My_URI_LoggerCallback), this},
        { MHD_OPTION_END, 0, NULL}
    };
    m_Daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, port, 0, 0, &MicroHttpdHandler::My_MHD_AccessHandlerCallback, this, MHD_OPTION_ARRAY, ops, MHD_OPTION_END);
    return (0 != m_Daemon);
}

void MicroHttpdHandler::Stop() {
    if (m_Daemon) MHD_stop_daemon(m_Daemon);
    m_Daemon = 0;
}

bool MicroHttpdHandler::IsValid() const {
    return (0 != m_Daemon);
}

int MicroHttpdHandler::My_MHD_AccessHandlerCallback(void *cls, struct MHD_Connection *connection,
        const char *url, const char *method, const char *version, const char * upload_data,
        size_t * upload_data_size, void ** /* con_cls */) {
    MicroHttpdHandler * Handler(static_cast<MicroHttpdHandler *> (cls));
    Handler->Handle(url, method, version);
    const char *page = "<html><body>Hello, browser!</body></html>";
    MHD_Response * response = MHD_create_response_from_buffer(strlen(page), (void*) page, MHD_RESPMEM_PERSISTENT);
    int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);
    return ret;
}

void * MicroHttpdHandler::My_URI_LoggerCallback(void* cls, const char* uri) {
    MicroHttpdHandler * Handler(static_cast<MicroHttpdHandler *> (cls));
    if (uri) {
        strncpy(Handler->mUriBuffer, uri, URI_BUFFER_SIZE);
    } else {
        Handler->mUriBuffer[0] = 0;
    }
    return 0;
}

int MicroHttpdHandler::Handle(const char *url, const char *method, const char *version) {
    int ret(MHD_HTTP_OK);
    if (!method || !*method || !url || !*url) {
        ret = MHD_HTTP_BAD_REQUEST;
    } else if (0 == strncmp(METHOD_GET, method, METHOD_GET_LEN)) {
        signalGet.emit(mUriBuffer, method, version);
    } else if (0 == strncmp(METHOD_PUT, method, METHOD_PUT_LEN)) {
        signalPut.emit(mUriBuffer, method, version);
    } else if (0 == strncmp(METHOD_DELETE, method, METHOD_DELETE_LEN)) {
        signalDelete.emit(mUriBuffer, method, version);
    } else {
        ret = MHD_HTTP_METHOD_NOT_ALLOWED;
    }
    return ret;
}
