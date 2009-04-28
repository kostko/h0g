/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_IMPORTERS_3DS_H
#define IID_IMPORTERS_3DS_H

#include <string>

#include "storage/importers/mesh.h"

namespace IID {

class Item;
class Storage;

/**
 * This class implements 3DS mesh import.
 */
class ThreeDSMeshImporter : public MeshImporter {
public:
    /**
     * Class constructor.
     *
     * @param context A valid IID context
     */
    ThreeDSMeshImporter(Context *context);
    
    /**
     * Loads a 3DS mesh file into the respective Mesh/CompositeMesh item.
     *
     * @param storage Item's storage
     * @param item Item to load the data into
     * @param filename Already resolved filename
     */
    void load(Storage *storage, Item *item, const std::string &filename);
};

}

#endif
