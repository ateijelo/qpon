#include "ppp.h"

PPP::PPP(QObject *parent)
        : QProcess(parent)
{
}

void PPP::setupChildProcess()
{
#ifdef Q_OS_UNIX
    setsid();
#endif
}
