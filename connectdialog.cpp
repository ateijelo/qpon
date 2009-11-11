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

#include <QTextStream>
#include <QSettings>
#include <QDebug>
#include <QFile>
#include <QDir>

#include "connectdialog.h"

ConnectDialog::ConnectDialog(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    populatePeers();
    state = Disconnected;
    setState(Disconnected);

    QSettings settings("QPon","qpon");
    settings.beginGroup("Main");
    int i = ui.peersComboBox->findText(settings.value("SelectedPeer").toString());
    i=qMax(i,0);
    ui.peersComboBox->setCurrentIndex(i);
    restoreGeometry(settings.value("Geometry").toByteArray());
    settings.endGroup();

    ppp = new PPP(this);
    //ppp->closeReadChannel(QProcess::StandardOutput);
    //ppp->closeReadChannel(QProcess::StandardError);

    for (int i=0; i<2; i++)
        for (int o=0; o<2; o++)
            trayIcons[i][o].addFile(QString("traffic%1%2.png").arg(i).arg(o));

    trayIcon.setIcon(trayIcons[0][0]);
    //trayIcon.show();

    timer = new QTimer();
    timer->setSingleShot(true);

    trayTimer = new QTimer();
    connect(trayTimer,SIGNAL(timeout()),this,SLOT(trayIconUpdate()));
    trayTimer->start(500);

    QFile *f = new QFile("/proc/net/dev"); f->open(QIODevice::ReadOnly);
    procNetDev.setDevice(f);

//    connectedMusic = Phonon::createPlayer(Phonon::MusicCategory,
//            Phonon::MediaSource("connected.wav"));
//    disconnectedMusic = Phonon::createPlayer(Phonon::MusicCategory,
//            Phonon::MediaSource("disconnected.wav"));

    connect(ui.button,SIGNAL(clicked()),this,SLOT(buttonClicked()));
    connect(ppp,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(pppFinished(int,QProcess::ExitStatus)));
    connect(this,SIGNAL(finished(int)),this,SLOT(close()));
    connect(timer,SIGNAL(timeout()),this,SLOT(timeout()));
}

void ConnectDialog::trayIconUpdate()
{
    if (!trayIcon.isVisible())
        return;
    procNetDev.seek(0);
    while (true)
    {
        QString s = procNetDev.readLine();
        if (s.isNull())
            break;
        QStringList l = s.split(QRegExp("[\\s:]+"));
        if (l.at(1).startsWith("ppp"))
        {
            int in = l.at(2).toInt();
            int out = l.at(10).toInt();
            
            int din = (in != traffic_in)? 1 : 0;
            int dout = (out != traffic_out)? 1 : 0;
            
            trayIcon.setIcon(trayIcons[din][dout]);
            trayIcon.setToolTip(tr("Connected time: %1<br>Received: %2 bytes<br>Sent: %3 bytes")
                    .arg(connectedTime())
                    .arg(in)
                    .arg(out));

            traffic_in = in;
            traffic_out = out;
        }
    }
}

QString ConnectDialog::connectedTime()
{
    int elapsed;
    QString msg;
    elapsed = time.elapsed() / 1000;
    if (elapsed>=3600)
    {
        msg=QString("%1:%2:%3").arg(elapsed/3600)
                                    .arg((elapsed%3600)/60,2,10,QLatin1Char('0'))
                                    .arg(elapsed%60,2,10,QLatin1Char('0'));
    }
    else
    {
        msg=QString("%2:%3").arg(elapsed/60,2,10,QLatin1Char('0'))
                                 .arg(elapsed%60,2,10,QLatin1Char('0'));
    }
    return msg;
}

void ConnectDialog::populatePeers()
{
    QDir peers("/etc/ppp/peers");
    ui.peersComboBox->addItems(peers.entryList(QDir::Files));
}

void ConnectDialog::close()
{
    switch (state)
    {
        case Retrying:
        case Connected:
        case Connecting:
            system("poff");
            break;
        case Disconnected:
        case Disconnecting:
            break;
    }

    QSettings settings("QPon","qpon");
    settings.beginGroup("Main");
    settings.setValue("SelectedPeer",ui.peersComboBox->currentText());
    settings.endGroup();
}

void ConnectDialog::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    QSettings settings("QPon","qpon");
    settings.beginGroup("Main");
    settings.setValue("Geometry",saveGeometry());
    settings.setValue("SelectedPeer",ui.peersComboBox->currentText());
    settings.endGroup();
}

void ConnectDialog::pppFinished(int exitcode, QProcess::ExitStatus exitstatus)
{
    qDebug() << "exit code:" << exitcode;
    qDebug() << "exit status:" << exitstatus;
    switch (state)
    {
        case Connected:
        case Disconnecting:
            QProcess::startDetached("aplay",QStringList() << "-q" << "disconnect.wav");
            trayIcon.hide();
        default:
            break;
    }
    if (exitstatus==QProcess::CrashExit)
    {
        if (state == Disconnecting)
        {
            setState(Disconnected);
            return;
        }
        setState(Retrying);
        return;
    }
    switch (exitcode)
    {
        case 5:   // pppd terminated because it was sent a SIGINT, SIGTERM or SIGHUP signal
        case 12:  // The link was established successfully and terminated because it was idle
            setState(Disconnected);
            return;
    }
    switch (state)
    {
        case Retrying:
        case Connected:
        case Connecting:
            setState(Retrying);
            break;
        case Disconnected:
        case Disconnecting:
            setState(Disconnected);
            break;
    }
}

void ConnectDialog::timeout()
{
    switch (state)
    {
        case Connecting:
            {
                QFile f("/proc/net/route");
                f.open(QIODevice::ReadOnly | QIODevice::Text);
                QTextStream s(&f);
                QString d=s.readAll();
                if (d.contains("ppp"))
                {
                    QProcess::startDetached("aplay",QStringList() << "-q" << "connect.wav");
                    trayIcon.show();
                    //connectedMusic->play();
                    time.start();
                    setState(Connected);
                }
                timer->start(1000);
            }
            break;
        case Connected:
            setState(Connected);
            break;
        case Retrying:
            state=Disconnected;
            buttonClicked();
            break;
        case Disconnected:
        case Disconnecting:
            break;
    }
}

void ConnectDialog::setState(State s)
{
    switch (s)
    {
        case Disconnected:
            ui.statusLabel->setText(tr("<font color=\"#808080\"><b>Disconnected</b></font>"));
            ui.button->setText(tr(" &Connect "));
            ui.button->setEnabled(true);
            ui.peersComboBox->setEnabled(true);
            state=s;
            break;
        case Connecting:
            ui.statusLabel->setText(tr("<font color=\"#dd4400\"><b>Connecting...</b></font>"));
            ui.button->setText(tr(" &Disconnect "));
            ui.peersComboBox->setEnabled(false);
            state=s;
            timer->start(1000);
            break;
        case Connected:
            ui.statusLabel->setText(tr("<font color=\"#00bb00\"><b>Connected %1</b></font>").arg(connectedTime()));
            state=s;
            timer->start(200);
            break;
        case Retrying:
            ui.statusLabel->setText(tr("<font color=\"#dd0000\"><b>Error, retrying...</b></font>"));
            state=s;
            timer->start(2000);
            break;
        case Disconnecting:
            ui.button->setText(tr(" Disconnecting... "));
            ui.button->setEnabled(false);
            state=s;
            break;
    }
}

void ConnectDialog::buttonClicked()
{
    QString prog;
    QStringList args;
    switch (state)
    {
        case Disconnected:
            setState(Connecting);
            prog = "/usr/sbin/pppd";
            args << "call" << ui.peersComboBox->currentText() << "nodetach";
            ppp->start(prog,args);
            break;
        case Connected:
        case Connecting:
            setState(Disconnecting);
            QProcess::startDetached("poff");
            break;
        case Retrying:
            setState(Disconnected);
            break;
        case Disconnecting:
            break;
    }
}
