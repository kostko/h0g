/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#include "storage/importers/base.h"
#include "context.h"
#include "logger.h"

namespace IID {

Importer::Importer(Context *context)
  : m_context(context),
    m_logger(context->logger("iid.importer"))
{
}

Importer::~Importer()
{
  delete m_logger;
}

}
