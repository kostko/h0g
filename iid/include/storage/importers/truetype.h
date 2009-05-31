/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_IMPORTERS_TRUETYPE_H
#define IID_IMPORTERS_TRUETYPE_H

#include <string>

#include "storage/importers/base.h"

namespace IID {

class Item;
class Storage;

/**
 * This class implements true type font import.
 */
class TrueTypeImporter : public Importer {
public:
    /**
     * Class constructor.
     *
     * @param context A valid IID context
     */
    TrueTypeImporter(Context *context);
    
    /**
     * Loads a TrueType font file into the respective Font item.
     *
     * @param storage Item's storage
     * @param item Item to load the data into
     * @param filename Already resolved filename
     */
    void load(Storage *storage, Item *item, const std::string &filename);
};

}

#endif
