/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_LOGGER_H
#define IID_LOGGER_H

#include <string>

namespace IID {

class Logger {
public:
    /**
     * Class constructor.
     */
    Logger(const std::string &name);
    
    /**
     * Output an error message.
     *
     * @param message Message string
     */
    void error(const std::string &message);
    
    /**
     * Output a warning message.
     *
     * @param message Message string
     */
    void warning(const std::string &message);
    
    /**
     * Output an info message.
     *
     * @param message Message string
     */
    void info(const std::string &message);
    
    void log(const std::string &type, const std::string &message);
private:
    // Currently active module identifier
    std::string m_module;
    
    // Time info buffer
    char m_timeBuffer[80];
};

}

#endif
