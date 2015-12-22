/*
 * Brief
 *
 * Rest server approach basis is a crossplatform libmictohttpd library
 * https://www.gnu.org/software/libmicrohttpd/
 * contains crossplatform http server with a small memory footprint
 * Wrapped in MicroHttpdHandler class, which is responsible for configure, run and handle http connections
 * uses a socket multiplexing scheme with a threadpool for a scalability
 *
 * Connection events run as async tasks with timeout for result to maka a connection pool free
 * as soon as possible. Expired events calculated as a waste load and leads to a redirect after constant amount
 *
 * RestRequestHandler class use a map-based approach to emulate the sql layer.
 * The requests are uri-based for testing. No json or xml library required.
 * No duplicates in a PUT requests are detected.
 * RW-lock idiom is used to access the data to provide a better scalability.
 *
 * RestServer class used to access the data storage and translate the results into http-based answers
 * Uses the Observer (signal/slot) pattern to glue with the httpd.
 *
 * no argv/argc parsing
 */

#include <cstdlib>
#include <stdio.h>
#include "RestServer.h"

/*
 *
 */
int main(int argc, char** argv) {
    RestServer * MyRestServer(new RestServer());
    if (!MyRestServer->Start(8080)) {
        std::cout << "Port 8080 busy, please select another." << std::endl;
    } else {
        std::cout << "Press Enter to stop the application..." << std::endl;
        MyRestServer->Wait();
    }
    delete MyRestServer;
    return 0;
}

