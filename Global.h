#define M_IN 2
#define M_OUT 5
#define M_IN_INTERRUPT 0

volatile boolean needReceive;
int track = 1; //1-99
int disk = 1; //1-9
int tSec;
int tMin;

boolean playing;
long timePlay = millis();//
