// This file is part of QPon, a simple interface Qt interface
// for the pon/poff ppp utilities.
//
// Copyright (C) 2007 Andy Teijelo <ateijelo@uh.cu>
// 
// QPon is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// version 2, as published by the Free Software Foundation;
// 
// QPon is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
// The file 'COPYING' contains the text of the GNU General Public License.

#include <QTime>
#include <QTimer>
// #include <Phonon>
#include <QDialog>
#include <QProcess>
#include <QTextStream>
#include <QSystemTrayIcon>

#include "ui_connectdialog.h"

#include "ppp.h"

class ConnectDialog : public QDialog
{
    Q_OBJECT
    Q_ENUMS(State)

    public:
        ConnectDialog(QWidget* parent=NULL);
        enum State { Connected,Connecting,Disconnected,Disconnecting,Retrying };

    private slots:
        void buttonClicked();
        void timeout();
        void pppFinished(int exitcode, QProcess::ExitStatus exitstatus);
        void populatePeers();
        void close();
        void closeEvent(QCloseEvent* event);
        void trayIconUpdate();

    private:
        void setState(State s);
        QString connectedTime();

        State state;
        QTime time;
        QTimer *timer;
        QTimer *trayTimer;
        PPP *ppp;
        Ui::ConnectDialog ui;

        QSystemTrayIcon trayIcon;
        QIcon trayIcons[2][2];
        QTextStream procNetDev;

        int traffic_in;
        int traffic_out;
   //     Phonon::MediaObject *connectedMusic;
   //     Phonon::MediaObject *disconnectedMusic;
};
