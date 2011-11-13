#ifndef CONFIG_H
#define CONFIG_H

#include <QtCore/QMutex>
#include <QtCore/QString>

// http://stackoverflow.com/questions/449436/singleton-instance-declared-as-static-variable-of-getinstance-method/449823#449823
// I'll need it on Win32. GCC will make it thread-safe.

class Config
{
public:
    static QString homeDir();
    static void checkConfig();
};

#endif // CONFIG_H
