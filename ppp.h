#ifndef PPP_H
#define PPP_H

#include <QProcess>

class PPP : public QProcess
{
    public:
        PPP(QObject *parent = NULL);
    protected:
        void setupChildProcess();
};

#endif // PPP_H
