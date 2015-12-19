/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   main.cpp
 * Author: dront
 *
 * Created on 18 декабря 2015 г., 17:13
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

