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
#include "Common.h"

typedef struct {
    io_service_t service;
    io_object_t notification;
    
    
    
} MyDriverData;

IONotificationPortRef notificationPort = NULL;



void DeviceNotification (void *refCon , io_service_t service , natural_t messageType , void *messageArgument);
void DeviceAdded (void* refCon , io_iterator_t iter);
void findDriverWithMatchingDict (CFDictionaryRef matchingDict);
void setupAsyncOpNotif();
kern_return_t StartTimer (io_connect_t connection);
kern_return_t StopTimer (io_connect_t connection);
kern_return_t GetElapsedTimerTime (io_connect_t connection , uint32_t* timerTime);
kern_return_t GetElapsedTimerValue (io_connect_t connection , TimerValue* timerValue);
kern_return_t DelayForMS (io_connect_t connection , uint32_t miliseconds);
kern_return_t DelayForTime (io_connect_t connection , TimerValue* timerValue);
IONotificationPortRef MyDriverGetAsyncCompletionPort ();








int main(int argc, const char * argv[]) {
    CFDictionaryRef matchingDict = NULL;
    matchingDict = IOServiceMatching("IOUSBDevice");
    findDriverWithMatchingDict(matchingDict);
    
    
    return 0;
    
    
}

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
        
             if(CFEqual(className, CFSTR("IOUSBDevice"))) {
        IORegistryEntryGetName(service, name);
        std::cout<<"found device with name : "<<name<<std::endl;
                    CFTypeRef vendorName;
                    vendorName = IORegistryEntryCreateCFProperty(service, CFSTR("USB Vendor Name"), kCFAllocatorDefault, 0);
                    CFShow(vendorName);
        
               }
        
        
        
        
        
        CFRelease(className);
        IOObjectRelease(service);
        
        
        
    }
}

void findDriverWithMatchingDict (CFDictionaryRef matchingDict){
    io_iterator_t iter = 0;
    CFRunLoopSourceRef runLoopSource;
    // io_service_t service = 0;
    
    kern_return_t kr;
    
    
    notificationPort = IONotificationPortCreate(kIOMasterPortDefault);
    runLoopSource = IONotificationPortGetRunLoopSource(notificationPort);
    CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopDefaultMode);
    
    
    
    kr = IOServiceAddMatchingNotification(notificationPort, kIOFirstMatchNotification, matchingDict, DeviceAdded, NULL, &iter);
    
    DeviceAdded(NULL, iter);
    
    
    
    
    CFRunLoopRun();
    
    
    IONotificationPortDestroy(notificationPort);
    
    IOObjectRelease(iter);
    
    
}

kern_return_t StartTimer(io_connect_t connection) {
    return IOConnectCallMethod(connection, kTestUserClientStartTimer, NULL, 0, NULL, 0, NULL, 0, NULL, NULL);
}

kern_return_t GetElapsedTimerTime (io_connect_t connection , uint32_t* timerTime) {
    uint64_t scalarOut[1];
    uint32_t scalarOutCount;
    kern_return_t result;
    scalarOutCount = 1;
    result = IOConnectCallScalarMethod(connection, kTestUserClientGetElapsedTimerTime, NULL, 0, scalarOut, &scalarOutCount);
    return result;
    
}

kern_return_t DelayForTime (io_connect_t connection , TimerValue* timerValue) {
    return IOConnectCallStructMethod(connection, kTestUserClientDelayForTime, timerValue, sizeof(TimerValue), NULL, 0);
}


IONotificationPortRef gAsyncNotificationPort = NULL;
IONotificationPortRef MyDriverGetAsyncCompletionPort () {
    
    if(gAsyncNotificationPort != NULL)
        return gAsyncNotificationPort;
    
    gAsyncNotificationPort = IONotificationPortCreate(kIOMasterPortDefault);
    return gAsyncNotificationPort;
    
}

void setupOpNotif () {
    CFRunLoopSourceRef runLoopSource = IONotificationPortGetRunLoopSource(MyDriverGetAsyncCompletionPort());
    CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopDefaultMode);
    
}




