#ifndef BITCOIN_INIT_H
#define BITCOIN_INIT_H

void StartShutdown();
bool ShutdownRequested();
void Shutdown();
bool AppInit2(boost::thread_group& threadGroup);

#endif // BITCOIN_INIT_H

