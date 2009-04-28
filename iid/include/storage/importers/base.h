/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_IMPORTERS_BASE_H
#define IID_IMPORTERS_BASE_H

#include <string>

namespace IID {

class Storage;
class Item;
class Context;
class Logger;

/**
 * An abstract base class for importers.
 */
class Importer {
public:
    /**
     * Class constructor.
     *
     * @param context A valid IID context
     */
    Importer(Context *context);
    
    /**
     * Class destructor.
     */
    virtual ~Importer();
    
    /**
     * This method should read data from the specified filename and import
     * it into the specified storage item.
     *
     * @param storage Item's storage
     * @param item Item to load the data into
     * @param filename Already resolved filename
     */
    virtual void load(Storage *storage, Item *item, const std::string &filename) = 0;
protected:
    Context *m_context;
    Logger *m_logger;
};

}

#endif
