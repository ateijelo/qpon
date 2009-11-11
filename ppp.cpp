#include "ppp.h"

PPP::PPP(QObject *parent)
        : QProcess(parent)
{
}

void PPP::setupChildProcess()
{
    setsid();
}
