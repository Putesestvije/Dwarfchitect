#ifndef DESIGNATIONS_H
#define DESIGNATIONS_H

//typedef enum {DIG, CHANNEL, UPSTAIRS, DOWNSTAIRS, UPDOWNSTAIRS, RAMP, CLEAR} Designation;
enum Key {UP_LEFT = 0, UP = 1, UP_RIGHT = 2, RIGHT = 3, DOWN_RIGHT = 5,
           DOWN = 6, DOWN_LEFT = 7, LEFT = 8, D_DIG, D_CHANNEL,
           D_UP_STAIR, D_DOWN_STAIR, D_UPDOWN_STAIR,
           D_RAMP, UP_Z, DOWN_Z, SELECT, EOG, EOM, CLEAR};
#endif // DESIGNATIONS_H
