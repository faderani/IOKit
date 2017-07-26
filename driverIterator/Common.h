//
//  Common.h
//  driverIterator
//
//  Created by Soroush Shahi on 7/26/17.
//  Copyright © 2017 soroush. All rights reserved.
//

#ifndef Common_h
#define Common_h

typedef struct TimerValue {
    uint64_t time;
}TimerValue;

enum TimerRequestCode {
    kTestUserClientStartTimer,
    kTestUserClientStopTimer,
    kTestUserClientGetElapsedTimerTime,
    kTestUserClientGetElapsedTimerValue,
    kTestUserClientDelayForMs,
    kTestUserClientDelayForTime,
    
    
    kTestUserClientMethodCount
    
};


#endif /* Common_h */
