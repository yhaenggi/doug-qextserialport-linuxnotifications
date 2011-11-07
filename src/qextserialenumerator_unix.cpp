#include "qextserialenumerator.h"
#include "qextserialenumerator_p.h"
#include <QtCore/QDebug>
#include <QtCore/QStringList>
#include <QtCore/QDir>

void QextSerialEnumeratorPrivate::platformSpecificInit()
{
}

void QextSerialEnumeratorPrivate::platformSpecificDestruct()
{
}

QList<QextPortInfo> QextSerialEnumeratorPrivate::getPorts_sys()
{
    QList<QextPortInfo> infoList;
#ifdef Q_OS_LINUX
    QStringList portNamePrefixes, portNameList;
    portNamePrefixes << "ttyS*"; // list normal serial ports first

    QDir dir("/dev");
    portNameList = dir.entryList(portNamePrefixes, (QDir::System | QDir::Files), QDir::Name);

    // remove the values which are not serial ports for e.g.  /dev/ttysa
    for (int i = 0; i < portNameList.size(); i++) {
        bool ok;
        QString current = portNameList.at(i);
        // remove the ttyS part, and check, if the other part is a number
        current.remove(0,4).toInt(&ok, 10);
        if (!ok) {
            portNameList.removeAt(i);
            i--;
        }
    }

    // get the non standard serial ports names
    // (USB-serial, bluetooth-serial, 18F PICs, and so on)
    // if you know an other name prefix for serial ports please let us know
    portNamePrefixes.clear();
    portNamePrefixes << "ttyACM*" << "ttyUSB*" << "rfcomm*";
    portNameList.append(dir.entryList(portNamePrefixes, (QDir::System | QDir::Files), QDir::Name));

    foreach (QString str , portNameList) {
        QextPortInfo inf;
        inf.physName = "/dev/"+str;
        inf.portName = str;

        if (str.contains("ttyS")) {
            inf.friendName = "Serial port "+str.remove(0, 4);
        }
        else if (str.contains("ttyUSB")) {
            inf.friendName = "USB-serial adapter "+str.remove(0, 6);
        }
        else if (str.contains("rfcomm")) {
            inf.friendName = "Bluetooth-serial adapter "+str.remove(0, 6);
        }
        inf.enumName = "/dev"; // is there a more helpful name for this?
        infoList.append(inf);
    }
#endif
    return infoList;
}

bool QextSerialEnumeratorPrivate::setUpNotifications_sys(bool setup)
{
    Q_UNUSED(setup)
    return false;
}
