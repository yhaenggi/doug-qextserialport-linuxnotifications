

#include <stdio.h>
#include "qextserialport.h"

#ifdef _TTY_WIN_
#include <QRegExp>
#endif

/*!
Default constructor.  Note that the name of the device used by a QextSerialPort constructed with
this constructor will be determined by #defined constants, or lack thereof - the default behavior
is the same as _TTY_LINUX_.  Possible naming conventions and their associated constants are:

\verbatim

Constant         Used By         Naming Convention
----------       -------------   ------------------------
_TTY_WIN_        Windows         COM1, COM2
_TTY_IRIX_       SGI/IRIX        /dev/ttyf1, /dev/ttyf2
_TTY_HPUX_       HP-UX           /dev/tty1p0, /dev/tty2p0
_TTY_SUN_        SunOS/Solaris   /dev/ttya, /dev/ttyb
_TTY_DIGITAL_    Digital UNIX    /dev/tty01, /dev/tty02
_TTY_FREEBSD_    FreeBSD         /dev/ttyd0, /dev/ttyd1
_TTY_OPENBSD_    OpenBSD         /dev/tty00, /dev/tty01
_TTY_LINUX_      Linux           /dev/ttyS0, /dev/ttyS1
<none>           Linux           /dev/ttyS0, /dev/ttyS1
\endverbatim

This constructor assigns the device name to the name of the first port on the specified system.
See the other constructors if you need to open a different port.
*/
QextSerialPort::QextSerialPort(QextSerialPort::QueryMode mode)
    : QIODevice()
{
    construct();
    setQueryMode(mode);
    platformSpecificInit();
}

QextSerialPort::QextSerialPort()
 : QIODevice()
{

#ifdef _TTY_WIN_
    setPortName("COM1");

#elif defined(_TTY_IRIX_)
    setPortName("/dev/ttyf1");

#elif defined(_TTY_HPUX_)
    setPortName("/dev/tty1p0");

#elif defined(_TTY_SUN_)
    setPortName("/dev/ttya");

#elif defined(_TTY_DIGITAL_)
    setPortName("/dev/tty01");

#elif defined(_TTY_FREEBSD_)
    setPortName("/dev/ttyd1");

#elif defined(_TTY_OPENBSD_)
    setPortName("/dev/tty00");

#else
    setPortName("/dev/ttyS0");
#endif

    construct();
    platformSpecificInit();
}

/*!
Construct a port and assign it to the device specified by the name parameter.
*/
QextSerialPort::QextSerialPort(const QString & name)
 : QIODevice()
{
    setPortName(name);
    construct();
    platformSpecificInit();
}

/*!
Constructs a serial port attached to the port specified by name.
name is the name of the device, which is windowsystem-specific,
e.g."COM1" or "/dev/ttyS0".
*/
QextSerialPort::QextSerialPort(const QString & name, QextSerialPort::QueryMode mode)
    : QIODevice()
{
    construct();
    setQueryMode(mode);
    setPortName(name);
    platformSpecificInit();
}

/*!
Constructs a port with default name and specified settings.
*/
QextSerialPort::QextSerialPort(const PortSettings& settings, QextSerialPort::QueryMode mode)
    : QIODevice()
{
    construct();
    setBaudRate(settings.BaudRate);
    setDataBits(settings.DataBits);
    setParity(settings.Parity);
    setStopBits(settings.StopBits);
    setFlowControl(settings.FlowControl);
    setTimeout(settings.Timeout_Millisec);
    setQueryMode(mode);
    platformSpecificInit();
}

/*!
Constructs a port with specified name and settings.
*/
QextSerialPort::QextSerialPort(const QString & name, const PortSettings& settings, QextSerialPort::QueryMode mode)
    : QIODevice()
{
    construct();
    setPortName(name);
    setBaudRate(settings.BaudRate);
    setDataBits(settings.DataBits);
    setParity(settings.Parity);
    setStopBits(settings.StopBits);
    setFlowControl(settings.FlowControl);
    setTimeout(settings.Timeout_Millisec);
    setQueryMode(mode);
    platformSpecificInit();
}

/*!
Common constructor function for setting up default port settings.
(115200 Baud, 8N1, Hardware flow control where supported, otherwise no flow control, and 0 ms timeout).
*/
void QextSerialPort::construct()
{
    lastErr = E_NO_ERROR;
    Settings.BaudRate=BAUD115200;
    Settings.DataBits=DATA_8;
    Settings.Parity=PAR_NONE;
    Settings.StopBits=STOP_1;
    Settings.FlowControl=FLOW_HARDWARE;
    Settings.Timeout_Millisec=500;
    mutex = new QMutex( QMutex::Recursive );
    setOpenMode(QIODevice::NotOpen);
}

void QextSerialPort::setQueryMode(QueryMode mechanism)
{
    _queryMode = mechanism;
}

/*!
Sets the name of the device associated with the object, e.g. "COM1", or "/dev/ttyS0".
*/
void QextSerialPort::setPortName(const QString & name)
{
    port = name;
    #ifdef Q_OS_WIN
    QRegExp rx("COM(\\d+)");
    if(port.contains(rx)) {
      int portnum = rx.cap(1).toInt();
      if(portnum > 9)
        port.prepend("\\\\.\\"); // COM ports greater than 9 need \\.\ prepended
    }
    #endif
}

/*!
Returns the name set by setPortName().
*/
QString QextSerialPort::portName() const
{
    return port;
}

/*!
Returns the baud rate of the serial port.  For a list of possible return values see
the definition of the enum BaudRateType.
*/
BaudRateType QextSerialPort::baudRate(void) const
{
    return Settings.BaudRate;
}

/*!
Returns the number of data bits used by the port.  For a list of possible values returned by
this function, see the definition of the enum DataBitsType.
*/
DataBitsType QextSerialPort::dataBits() const
{
    return Settings.DataBits;
}

/*!
Returns the type of parity used by the port.  For a list of possible values returned by
this function, see the definition of the enum ParityType.
*/
ParityType QextSerialPort::parity() const
{
    return Settings.Parity;
}

/*!
Returns the number of stop bits used by the port.  For a list of possible return values, see
the definition of the enum StopBitsType.
*/
StopBitsType QextSerialPort::stopBits() const
{
    return Settings.StopBits;
}

/*!
Returns the type of flow control used by the port.  For a list of possible values returned
by this function, see the definition of the enum FlowType.
*/
FlowType QextSerialPort::flowControl() const
{
    return Settings.FlowControl;
}

/*!
Returns true if device is sequential, otherwise returns false. Serial port is sequential device
so this function always returns true. Check QIODevice::isSequential() documentation for more
information.
*/
bool QextSerialPort::isSequential() const
{
    return true;
}

/*!
This function will return true if the input buffer is empty (or on error), and false otherwise.
Call QextSerialPort::lastError() for error information.
*/
bool QextSerialPort::atEnd() const
{
    return size() != 0;
}

/*!
This function will read a line of buffered input from the port, stopping when either maxSize bytes
have been read, the port has no more data available, or a newline is encountered.
The value returned is the length of the string that was read.
*/
qint64 QextSerialPort::readLine(char * data, qint64 maxSize)
{
    qint64 numBytes = bytesAvailable();
    char* pData = data;

    if (maxSize < 2) //maxSize must be larger than 1
        return -1;

    /*read a byte at a time for MIN(bytesAvail, maxSize - 1) iterations, or until a newline*/
    while (pData<(data+numBytes) && --maxSize) {
        readData(pData, 1);
        if (*pData++ == '\n') {
            break;
        }
    }
    *pData='\0';

    /*return size of data read*/
    return (pData-data);
}

///*!
//Copy constructor.
//
//\deprecated
//*/
//QextSerialPort::QextSerialPort(const QextSerialPort& s)
//{}
//
///*!
//\fn QextSerialPort& QextSerialPort::operator=(const QextSerialPort& s)
//Overrides the = operator.
//
//\deprecated
//*/
//QextSerialPort& QextSerialPort::operator=(const QextSerialPort& s)
//{
//    return (QextSerialPort&)QextBaseType::operator=(s);
//}

/*!
Standard destructor.
*/
QextSerialPort::~QextSerialPort()
{
    if (isOpen()) {
        close();
    }
    platformSpecificDestruct();
    delete mutex;
}
