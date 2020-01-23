#ifndef CONSTANTS_H_
#define CONSTANTS_H_

// COSINE SIMILARITY
#define SENSOR_DATA_SIZE 48 // 3.2 seconds
#define SENSOR_SAMPLING_DELAY 50000 // 0.05 seconds
#define GESTURE_THRESHOLD 0.80 // cosinesimilarity threshold value

// BATTERY
#define AON_BATMON_BASE 0x40095000
#define AON_BATMON_O_BAT 0x28
#define MAX_BAT_VOLTAGE 3.3f


enum STATE {
    IDLE,
    READ_SENSOR,
    RECORD_SENSOR,
    DETECTED,
    TEMPERATURE,
    MSG_WAITING            
};
extern enum STATE mystate;



extern int sendMsg;



#endif //CONSTANTS_H_
