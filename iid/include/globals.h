/*
 * This file is part of the Infinite Improbability Drive.
 *
 * Copyright (C) 2009 by Jernej Kos <kostko@unimatrix-one.org>
 * Copyright (C) 2009 by Anze Vavpetic <anze.vavpetic@gmail.com>
 */
#ifndef IID_GLOBALS_H
#define IID_GLOBALS_H

#include <boost/unordered_map.hpp>

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/Array>

USING_PART_OF_NAMESPACE_EIGEN

using Eigen::Quaternionf;
using Eigen::Transform3f;
using Eigen::AlignedBox;
using Eigen::AngleAxisf;
using Eigen::Hyperplane;

namespace IID {

// Global definitions for ease of use
typedef boost::unordered_map<std::string, std::string> StringMap;

}

#endif
