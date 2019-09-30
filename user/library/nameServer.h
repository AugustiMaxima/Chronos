#ifndef NAMESERVER
#define NAMESERVER

void nameServer();

int RegisterAs(const char *name);

int WhoIs(const char *name);

int getNsTid();

#endif