/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "logger.h"

#include <iostream>
#include <time.h>
#include <stdlib.h>

using namespace std;

namespace IID {

Logger::Logger(const std::string &name)
  : m_module(name)
{
}

void Logger::error(const std::string &message)
{
  log("ERROR  ", message);
  abort();
}

void Logger::warning(const std::string &message)
{
  log("WARNING", message);
}

void Logger::info(const std::string &message)
{
  log("INFO   ", message);
}

void Logger::log(const std::string &type, const std::string &message)
{
  time_t rawtime;
  struct tm *timeinfo;
  
  // Grab formatted date and time
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  strftime(m_timeBuffer, 80, "%a, %d %b %Y %H:%M:%S", timeinfo);
  
  // Output string to stdout
  cout << m_timeBuffer << " " << type << " [" << m_module << "] " << message << endl;
}


}
