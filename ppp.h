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
