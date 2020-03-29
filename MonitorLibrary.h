void openMonitor() {
  if (monitorOpen) {
    return;
  }
  if (debug) {
    Serial.println("MONITOR OPEN");
  }
  digitalWrite(EXT2, HIGH);
  monitorOpen = true;
}

void closeMonitor() {
  if (!monitorOpen) {
    return;
  }
  if (debug) {
    Serial.println("MONITOR CLOSE");
  }
  digitalWrite(EXT2, LOW);
  monitorOpen = false;
}

manageMonitorPosition () {
  if (playing) {
    openMonitor();
  } else {
    closeMonitor();
  }
}
