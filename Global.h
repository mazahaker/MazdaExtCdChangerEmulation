#define M_IN 2
#define M_OUT 4
#define M_IN_INTERRUPT 0
#define EXT1 6 //black RELAY
#define EXT2 7 //white MONITOR
#define ACC_IN A0

boolean debug = true;

int track = 1; //1-99
int disk = 1; //1-9
int tSec;
int tMin;

volatile boolean needReceive;
boolean inited; //+
boolean playing; //+
boolean monitorOpen; //+

long timePlay = millis();//
