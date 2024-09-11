#pragma once
/*/////////////////////////////////////////////////////////////
// Author: Petras Swissler
// Description: Robot logic is often broken down into a set of
                higherarchical ACTION information. Typically,
                the way I handle it is to have "behaviors" 
                that consist of a variety of ACTIONs as well as
                "phases" within these ACTIONs.     
*//////////////////////////////////////////////////////////////
#define BEHAVIOR_IDLE       0x00
#define ACTION_IDLE          0x00
#define PHASE_IDLE          0x00

#define BEHAVIOR_HEARTBEAT  0x01

#define BEHAVIOR_EXAMPLE    0x02
#define ACTION_INIT          0x00
#define ACTION_FORWARD       0x01
#define ACTION_SPIN          0x03
