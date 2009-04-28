/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_STORAGE_ARGUMENTS_H
#define IID_STORAGE_ARGUMENTS_H

#include "globals.h"

namespace IID {

class Storage;
class Item;

/**
 * An argument handler for loading data files into storage items. This
 * just calls a valid importer.
 */
void argumentLoadFile(Storage *storage, Item *item, StringMap arguments);

/**
 * An argument handler for setting storage item attributes.
 */
void argumentAttribute(Storage *storage, Item *item, StringMap arguments);

}

#endif
