//
//  main.cpp
//  driverIterator
//
//  Created by Soroush Shahi on 7/24/17.
//  Copyright © 2017 soroush. All rights reserved.
//

#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/IOMessage.h>
#include <iostream>

typedef struct {
    io_service_t service;
    io_object_t notification;
    
    
    
} MyDriverData;

IONotificationPortRef notificationPort = NULL;






    




void DeviceNotification (void *refCon , io_service_t service , natural_t messageType , void *messageArgument) {
    MyDriverData *driverData = (MyDriverData*)refCon;
    kern_return_t kr;
    
    
    
    if (messageType == kIOMessageServiceIsTerminated) {
        io_name_t name;
        IORegistryEntryGetName(service, name);
        std::cout<<"device removed with name : " <<name<<std::endl;
        
        kr = IOObjectRelease(driverData->notification);
        IOObjectRelease(driverData->service);
        free(driverData);
    }
}



void DeviceAdded (void* refCon , io_iterator_t iter) {
    
    io_service_t service = 0;
    
    
    while ((service = IOIteratorNext(iter)) != 0 ) {
        MyDriverData *driverData;
        kern_return_t kr;
        
        driverData = (MyDriverData*)malloc(sizeof(MyDriverData));
        driverData->service = service;
        
        
        
        kr = IOServiceAddInterestNotification(notificationPort, service, kIOGeneralInterest, DeviceNotification, driverData, &driverData->notification);

        IORegistryEntrySetCFProperty(service, CFSTR("StopMessage"), CFSTR("driver has stopped"));
        
        CFStringRef className;
        io_name_t name;
        
        
        
        className = IOObjectCopyClass(service);
        
   //     if(CFEqual(className, CFSTR("IOUSBDevice"))) {
            IORegistryEntryGetName(service, name);
            std::cout<<"found device with name : "<<name<<std::endl;
//            CFTypeRef vendorName;
//            vendorName = IORegistryEntryCreateCFProperty(service, CFSTR("USB Vendor Name"), kCFAllocatorDefault, 0);
//            CFShow(vendorName);
            
 //       }
        
        
        
        
        
        CFRelease(className);
        IOObjectRelease(service);
        
        
        
    }
    
    
    
    
    
    
    
}

int main(int argc, const char * argv[]) {
    CFDictionaryRef matchingDict = NULL;
    io_iterator_t iter = 0;
    CFRunLoopSourceRef runLoopSource;
   // io_service_t service = 0;
    
    kern_return_t kr;

    
    matchingDict = IOServiceMatching("com_osxkernel_driver_IOKitTest");
    notificationPort = IONotificationPortCreate(kIOMasterPortDefault);
    runLoopSource = IONotificationPortGetRunLoopSource(notificationPort);
    CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopDefaultMode);
    
    
    
    kr = IOServiceAddMatchingNotification(notificationPort, kIOFirstMatchNotification, matchingDict, DeviceAdded, NULL, &iter);
    
    DeviceAdded(NULL, iter);
    
    
   
    
    CFRunLoopRun();
    
    
    IONotificationPortDestroy(notificationPort);
    
    IOObjectRelease(iter);
    
    
    return 0;
    
    
}



