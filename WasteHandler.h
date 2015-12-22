/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   WasteHandler.h
 * Author: dront
 *
 * Created on 22 декабря 2015 г., 11:49
 */

#ifndef WASTEHANDLER_H
#define WASTEHANDLER_H

class WasteHandler {
public:
    virtual ~WasteHandler() {

    }

    virtual void OnTaskFinished() = 0;
};


#endif /* WASTEHANDLER_H */

