/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_EXCEPTIONS_H
#define IID_EXCEPTIONS_H

#include <string>

namespace IID {

/**
 * Base class for IID exceptions.
 */
class Exception {
public:
    /**
     * Class constructor.
     *
     * @param message Exception message
     */
    Exception(const std::string &message)
      : m_message(message)
    {}
    
    /**
     * Returns the exception message.
     */
    std::string getMessage() const { return m_message; }
private:
    std::string m_message;
};

}

#endif
