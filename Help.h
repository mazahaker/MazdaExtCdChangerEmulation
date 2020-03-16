inline int toInt(char f, char s) {
  char buf[3];
  buf[0] = f;
  buf[1] = s;
  buf[2] = '\0';
  return atoi(buf);
}

inline int toInt(char c) {
  return c - '0';
}
