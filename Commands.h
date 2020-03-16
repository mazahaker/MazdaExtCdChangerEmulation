#define MIN_POS_F 7
#define MIN_POS_S 8
#define SEC_POS_F 9
#define SEC_POS_S 10
#define DISK_POS 6
#define TRACK_POS_F 3
#define TRACK_POS_S 4

char PING_ANSWER[] = "D8";                //58E
char HARDWARE_STATUS[] = "DB91000000C";   //59D
char DISC_COUNT[] = "DD123456000000FC0";  //59D
char DISC_INFO[] = "DC1000000000";        //59D - диск есть
char STATUS_STOP[] = "D9000010000000C";   //59D
char STATUS_SEEKING[] = "D9501010000000C";   //5110127 - поиск диска
char DISC_INFO_WITH_TRACK[] = "DC101999999F";        //5110127 - диск есть, первый трек №1, последний 99, длина диска 99м 99с
char STATUS_PLAY[] = "D94010100000001";   //5110127

char CHANGE_DISK1[] = "DB910010001";   //for change disk first cmd
char CHANGE_DISK2[] = "DB95000000C";   //for change disk second cmd

char SHUT_DOWN[] = "D9000010000000C";

void updateTrackTime() {
  STATUS_PLAY[MIN_POS_F] = tMin / 10 + '0';
  STATUS_PLAY[MIN_POS_S] = tMin % 10 + '0';
  STATUS_PLAY[SEC_POS_F] = tSec / 10 + '0';
  STATUS_PLAY[SEC_POS_S] = tSec % 10 + '0';
}

void updateShutDown() {
  SHUT_DOWN[DISK_POS] = disk + '0';
}

void updateDiskNumber() {
  CHANGE_DISK2[3] = disk + '0';
  DISC_INFO_WITH_TRACK[2] = disk + '0';
  STATUS_SEEKING[DISK_POS] = disk + '0';
}

void updateTrackNumber() {
  STATUS_PLAY[TRACK_POS_F] = track / 10 + '0';
  STATUS_PLAY[TRACK_POS_S] = track % 10 + '0';
  STATUS_PLAY[DISK_POS] = disk + '0';
}
