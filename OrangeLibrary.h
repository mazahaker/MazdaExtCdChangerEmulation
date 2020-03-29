int powerWaitTimeConst = 10000; //Время ожидания повторного включения зажигания и отключения апельсинки (1000 = 1с)
int powerLimit = 200; //лимит ацп наличия питания
boolean wait; //Флаг ожидания события в функции усправления питанием
long powerWaitTime; //Время ожидания в функции усправления питанием

long pingPongWaitTime; //Время ожидания в функции соединения с оранж

enum OrangePowerState {
  ON,
  OFF,
  WAIT_ACC_AGAIN,
  WAIT_OFF,
  WAIT_ACC
} powerState = WAIT_ACC;

enum OrangeConnectionState {
  PING_SEND,
  PONG_WAIT,
  CONNECTED
} connectionState = PING_SEND;

void sendPing() {
  Serial.println("<ORANGE:PING>");
}

//void sendOpenPlayer() {
//  if (!playerOpened) {
//    playerOpened = true;
//    Serial.println("<ORANGE:OPENPLAYER>");
//  }
//}

void sendShutDown() {
  if (connectionState != CONNECTED) {
    return;
  }
  Serial.println("<ORANGE:SHUTDOWN>");
}

void sendPlay() {
  if (connectionState != CONNECTED) {
    return;
  }
  Serial.println("<ORANGE:PLAY>");
}

void sendStop() {
  if (connectionState != CONNECTED) {
    return;
  }
  Serial.println("<ORANGE:STOP>");
}

void sendChangeTrack(int oldDisk, int newDisk, int oldTrack, int newTrack) {
  if (connectionState != CONNECTED) {
    return;
  }
  if (oldDisk == newDisk && oldTrack == newTrack) {
    return;
  }

  boolean next;
  if (oldDisk != newDisk) {

    if (oldDisk == newDisk && oldTrack == newTrack) {
      return;
    }

    if (oldDisk == 6 && newDisk == 1) {
      next = true;
    } else {
      if (oldDisk == 1 && newDisk == 6) {
        next = false;
      } else {
        next = newDisk > oldDisk;
      }
    }
  } else {
    if (oldTrack == 99 && newTrack == 1) {
      next = true;
    } else {
      if (oldTrack == 1 && newTrack == 99) {
        next = false;
      } else {
        next = newTrack > oldTrack;
      }
    }
  }
  Serial.println(next ? "<ORANGE:NEXT>" : "<ORANGE:PREV>");
}

boolean manageRelay() {
  boolean stateOn = false;                                             //Признак стабильного питания апельсинки
  int acc = analogRead(ACC_IN);
  switch (powerState) {
    case ON : {                                                       //Предыдущее состояние вкл
        wait = false;
        if (acc > powerLimit) {                                       //Если асс выключено, то переходим в режим ожидания (время powerWaitTime) влючения и не выключаем реле
          stateOn = true;
        } else {
          powerState = WAIT_ACC_AGAIN;
          powerWaitTime = millis();
        }
        break;
      }
    case WAIT_ACC_AGAIN : {
        if (acc > powerLimit) {                                       //Если мы были в состоянии ожидает питания  и оно появилось, то перехдим в режим "питание есть"
          powerState = ON;
        } else if (millis() - powerWaitTime > powerWaitTimeConst) {             //Иначе смотрим сколько прошло времени с момента ожидания, и если больше powepowerWaitTimetConst, то переходим в режим отключения
          powerState = WAIT_OFF;
          powerWaitTime = millis();                                        //При переходе в режим отключения засекаем время на это отключение
          sendShutDown();                                             //И посылаем команду - выключись
        }
        break;
      }
    case WAIT_OFF : {
        if (millis() - powerWaitTime > powerWaitTimeConst) {                    //Ждем когда пройдет время ожидания выключения
          powerState = OFF;
        }
        break;
      }
    case OFF : {                                                      //Отключаем реле питания и переходим в режим ожидания включения
        digitalWrite(EXT1, LOW);
        powerState = WAIT_ACC;
        break;
      }
    case WAIT_ACC : {
        //        if (acc > powerLimit) {                                       //В режиме ожидания включения если появилось питание, то ждем 3сек, не пропадет ли оно и включаем реле
        //          if (!wait) {
        //            powerWaitTime = millis();
        //            wait = true;
        //          }
        //          if (millis() - powerWaitTime > 3000 && wait) {
        //            digitalWrite(EXT1, HIGH);
        //            powerState = ON;
        //          }
        //        }
        if (inited && acc > powerLimit) { //включаем после успешной инициализации устройства
          digitalWrite(EXT1, HIGH);
          powerState = ON;
        }
        break;
      }
    default : ;
  }
  return stateOn;
}

//Функция сканирования наличия ответа на пинг
boolean tryPong() {
  String serialData = "";
  if (!Serial.available()) {
    return false;
  }

  do {
    char chr = Serial.read();
    serialData.concat(chr);
  } while (Serial.available());

  return serialData == "pong";
}

//функция запроса доступности апельсинки
void pingOrange() {
  if (connectionState != CONNECTED && powerState == ON && millis() - pingPongWaitTime > 1500) { // если уже подключениы или есть питание или таймаут 1.5 сек не прошел, то уходим
    switch (connectionState) {
      case PING_SEND : {                                          //Посылаем пинг и уходим
          sendPing();
          connectionState = PONG_WAIT;
          pingPongWaitTime = millis();
          break;
        }
      case PONG_WAIT : {                                          //Проверяем понг. при успехе и флаге воспр. посылаем play
          connectionState = tryPong() ? CONNECTED : PING_SEND;
          if (connectionState == CONNECTED) {
            if (playing) {
              sendPlay();
            }
          } else {
            pingPongWaitTime = millis();
          }
          break;
        }
      default : ;
    }
  }
}
