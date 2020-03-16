#include "Global.h"
#include "Commands.h"
#include "Help.h"
#include "MLibrary.h"
#include "ExtLibrary.h"

void setup() {
  Serial.begin(9600);
  pinMode(M_IN, INPUT);
  pinMode(M_OUT, OUTPUT);
  attachInterrupt(M_IN_INTERRUPT, enableNeedReceive, LOW);
}

void enableNeedReceive() {
  needReceive = true;
}

void sendTime() {
  timePlay = millis();
  while (!needReceive) {
    if (millis() - timePlay > 1000) {
      detachInterrupt(M_IN_INTERRUPT);
      timePlay = millis();
      tSec++;
      if (tSec >= 60) {
        tMin = tMin < 98 ? tMin + 1 : 0;
        tSec = 0;
      }
      updateTrackTime();
      send(STATUS_PLAY, sizeof(STATUS_PLAY) - 1);
      attachInterrupt(M_IN_INTERRUPT, enableNeedReceive, LOW);
    }
  }
}

void loop() {
  if (needReceive) {
    detachInterrupt(M_IN_INTERRUPT);
    uint32_t message = receive();

    //    if (getAddress(message) == 5) {
    switch (message) {
      case 0x58E : { //пинг
          send(PING_ANSWER, sizeof(PING_ANSWER) - 1);
          playing = false;
          break;
        }
      case 0x59D : { //проснись
          send(HARDWARE_STATUS, sizeof(HARDWARE_STATUS) - 1);
          send(DISC_COUNT, sizeof(DISC_COUNT) - 1);
          send(DISC_INFO, sizeof(DISC_INFO) - 1);
          send(STATUS_STOP, sizeof(STATUS_STOP) - 1);
          sendWakeUp();
          playing = false;
          break;
        }
      default:
        commandProcessing(message);
    }
    //    }
    attachInterrupt(M_IN_INTERRUPT, enableNeedReceive, LOW);
  }
  if (playing) {
    sendTime();
  }
}

//разбор комад переключения по трекам
void commandProcessing(uint32_t command) {
  int bufSize = 10;
  char commandBuf[bufSize];
  for (int i = 0; i < bufSize; i++) {
    commandBuf[i] = 'X';
  }
  String(command, HEX).toCharArray(commandBuf, bufSize);

  //если мы пришли не по адресу 5 или не по команде Control
  if (commandBuf[0] != '5' || commandBuf[1] != '1') { //0x5130021(5)
    return;
  }
  //анализ подкоманды
  switch (commandBuf[2]) {
    case '1' : { // управление воспроизведением
        if (command == 0x5110127) { //команда играй
          send(STATUS_SEEKING, sizeof(STATUS_SEEKING) - 1);
          send(DISC_INFO_WITH_TRACK, sizeof(DISC_INFO_WITH_TRACK) - 1);
          sendPlay();
          playing = true;
        } else if (command == 0x5114024) { // команда стоп (возможно переход на другое устройство)
          updateShutDown();
          send(SHUT_DOWN, sizeof(SHUT_DOWN) - 1);
          sendStop();
          playing = false;
        }
        break;
      }
    case '3' : { // переключение треков
        playing = true;
        tSec = 0;
        tMin = 0;
        updateTrackTime();

        int oldDisk = disk;
        int oldTrack = track;
        int newDisk = toInt(commandBuf[3]);
        int newTrack = toInt(commandBuf[4], commandBuf[5]);

        if (newDisk != 0) {
          track = 1;
          disk = newDisk;
          updateDiskNumber();
          updateTrackNumber();
          send(CHANGE_DISK1, sizeof(CHANGE_DISK1) - 1);
          send(CHANGE_DISK2, sizeof(CHANGE_DISK2) - 1);
          send(STATUS_SEEKING, sizeof(STATUS_SEEKING) - 1);
          send(DISC_INFO_WITH_TRACK, sizeof(DISC_INFO_WITH_TRACK) - 1);
        } else if (newTrack != 0) {
          track = newTrack;
          updateTrackNumber();
        }
        send(STATUS_PLAY, sizeof(STATUS_PLAY) - 1);
        sendChangeTrack(oldDisk, disk, oldTrack, track);
        break;
      }
    default:;
  }
}
