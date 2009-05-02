/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_IMPORTERS_COLLADA_H
#define IID_IMPORTERS_COLLADA_H

#include <string>

#include "storage/importers/mesh.h"

namespace IID {

class Item;
class Storage;

/**
 * This class implements COLLADA mesh import.
 */
class ColladaMeshImporter : public MeshImporter {
public:
    /**
     * Class constructor.
     *
     * @param context A valid IID context
     */
    ColladaMeshImporter(Context *context);
    
    /**
     * Loads a COLLADA mesh file into the respective Mesh/CompositeMesh item.
     *
     * @param storage Item's storage
     * @param item Item to load the data into
     * @param filename Already resolved filename
     */
    void load(Storage *storage, Item *item, const std::string &filename);
};

}

#endif
