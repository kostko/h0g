/*
 * This file is part of h0g.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "context.h"

int main()
{
  IID::Context *context = new IID::Context();
  context->init();
  context->start();
  
  return 0;
}
