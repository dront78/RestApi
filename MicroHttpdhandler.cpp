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
#include <future>
#include <iostream>
#include "MicroHttpdHandler.h"

const char * const MicroHttpdHandler::METHOD_GET("GET");
const size_t MicroHttpdHandler::METHOD_GET_LEN(3);
const char * const MicroHttpdHandler::METHOD_PUT("PUT");
const size_t MicroHttpdHandler::METHOD_PUT_LEN(3);
const char * const MicroHttpdHandler::METHOD_DELETE("DELETE");
const size_t MicroHttpdHandler::METHOD_DELETE_LEN(6);
const char * const MicroHttpdHandler::RESPONSE_ERROR("<html><body>TRANSACTION ERROR</body></html>");
const size_t MicroHttpdHandler::RESPONSE_ERROR_LEN(43);

thread_local char * MicroHttpdHandler::mUriBuffer;

MicroHttpdHandler::MicroHttpdHandler() : m_Daemon(0), m_WasteLoad(0) {
}

MicroHttpdHandler::MicroHttpdHandler(const MicroHttpdHandler& /* orig */) {
    assert(0);
}

MicroHttpdHandler::~MicroHttpdHandler() {
    Stop();
}

bool MicroHttpdHandler::Start(uint16_t port) {
    struct MHD_OptionItem ops[] = {
        { MHD_OPTION_CONNECTION_LIMIT, CONNECTION_LIMIT, NULL},
        { MHD_OPTION_CONNECTION_TIMEOUT, CONNECTION_TIMEOUT, NULL},
        { MHD_OPTION_THREAD_POOL_SIZE, THREAD_POOL_SIZE, NULL},
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

void MicroHttpdHandler::OnTaskFinished() {
    --m_WasteLoad;
}

std::string MicroHttpdHandler::TaskGet(const std::string url, const std::string method, const std::string version) {
    std::string _Result;
    signalGet.emit(url, method, version, _Result, *this);
    return _Result;
}

std::string MicroHttpdHandler::TaskPut(const std::string url, const std::string method, const std::string version) {
    std::string _Result;
    signalPut.emit(url, method, version, _Result, *this);
    return _Result;
}

std::string MicroHttpdHandler::TaskDelete(const std::string url, const std::string method, const std::string version) {
    std::string _Result;
    signalDelete.emit(url, method, version, _Result, *this);
    return _Result;
}

int MicroHttpdHandler::My_MHD_AccessHandlerCallback(void *cls, struct MHD_Connection *connection,
        const char *url, const char *method, const char *version, const char * /* upload_data */,
        size_t * /* upload_data_size */, void ** /* con_cls */) {
    // std::cout << "My_MHD_AccessHandlerCallback " << std::this_thread::get_id() << std::endl;
    MicroHttpdHandler * Handler(static_cast<MicroHttpdHandler *> (cls));
    int code(MHD_HTTP_OK);
    const std::string page = Handler->Handle(url, method, version, code);
    // MHD_RESPMEM_PERSISTENT means the buffer is constant and will not be touched
    MHD_Response * response = MHD_create_response_from_buffer(page.length(), strdup(page.c_str()), MHD_RESPMEM_MUST_FREE);
    int ret = MHD_queue_response(connection, code, response);
    MHD_destroy_response(response);
    return ret;
}

void * MicroHttpdHandler::My_URI_LoggerCallback(void* cls, const char* uri) {
    MicroHttpdHandler * Handler(static_cast<MicroHttpdHandler *> (cls));
    // std::cout << "My_URI_LoggerCallback " << std::this_thread::get_id() << std::endl;
    static thread_local char * myUri(0);
    if (0 == myUri) {
        myUri = mUriBuffer = new char [URI_BUFFER_SIZE + 1];
        mUriBuffer[URI_BUFFER_SIZE] = 0;
    }
    if (uri) {
        strncpy(Handler->mUriBuffer, uri, URI_BUFFER_SIZE);
    } else {
        Handler->mUriBuffer[0] = 0;
    }
    return 0;
}

std::string MicroHttpdHandler::Handle(const char *url, const char *method, const char *version, int & ret) {
    if (!method || !*method || !url || !*url) {
        ret = MHD_HTTP_BAD_REQUEST;
    } else if (m_WasteLoad > WASTELOAD) {
        ret = MHD_HTTP_SERVICE_UNAVAILABLE;
    } else if (0 == strncmp(METHOD_GET, method, METHOD_GET_LEN)) {
        std::string SaveUri(mUriBuffer);
        std::future<std::string> FutureTask = std::async(std::launch::async, &MicroHttpdHandler::TaskGet, this, SaveUri, std::string(method), std::string(version));
        ++m_WasteLoad;
        if (std::future_status::ready == FutureTask.wait_for(std::chrono::milliseconds(TASK_WAIT_GET))) {
            return FutureTask.get();
        } else {
            return RESPONSE_ERROR;
        }
    } else if (0 == strncmp(METHOD_PUT, method, METHOD_PUT_LEN)) {
        std::string SaveUri(mUriBuffer);
        std::future<std::string> FutureTask = std::async(std::launch::async, &MicroHttpdHandler::TaskPut, this, SaveUri, std::string(method), std::string(version));
        ++m_WasteLoad;
        if (std::future_status::ready == FutureTask.wait_for(std::chrono::milliseconds(TASK_WAIT_PUT))) {
            return FutureTask.get();
        } else {
            return RESPONSE_ERROR;
        }
    } else if (0 == strncmp(METHOD_DELETE, method, METHOD_DELETE_LEN)) {
        std::string SaveUri(mUriBuffer);
        std::future<std::string> FutureTask = std::async(std::launch::async, &MicroHttpdHandler::TaskDelete, this, SaveUri, std::string(method), std::string(version));
        ++m_WasteLoad;
        if (std::future_status::ready == FutureTask.wait_for(std::chrono::milliseconds(TASK_WAIT_DELETE))) {
            return FutureTask.get();
        } else {
            return RESPONSE_ERROR;
        }
    } else {
        ret = MHD_HTTP_METHOD_NOT_ALLOWED;
    }
    return RESPONSE_ERROR;
}
