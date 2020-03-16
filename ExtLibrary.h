void sendWakeUp() {
  Serial.print("WAKEUP");
}

void sendPlay() {
  Serial.print("PLAY");
}

void sendStop() {
  Serial.print("STOP");
}

void sendChangeTrack(int oldDisk, int newDisk, int oldTrack, int newTrack) {
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
  Serial.print(next ? "NEXT" : "PREV");
}
