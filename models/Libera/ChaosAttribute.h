/* 
 * File:   ChaosAttribute.h
 * Author: michelo
 *
 * Created on September 2, 2015, 5:29 PM
 */

#ifndef CHAOSATTRIBUTE_H
#define	CHAOSATTRIBUTE_H
#include <map>
#include <chaos/ui_toolkit/ChaosUIToolkit.h>

#include <chaos/ui_toolkit/HighLevelApi/DeviceController.h>

class ChaosAttribute {
    
chaos::ui::DeviceController* controller;
public:
    ChaosAttribute();
    
    ChaosAttribute(std::string path);

    ChaosAttribute(const ChaosAttribute& orig);
    virtual ~ChaosAttribute();
    template <typename T>
    T* get(std::string name);
    
    template <typename T>
    int set(std::string path,T& data);
    int setTimeout(int timeo_ms);
private:
    int timeo_ms;
   
};

#endif	/* CHAOSATTRIBUTE_H */

