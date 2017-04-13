#pragma once

#include "common.h"


namespace NGCForest {

    TCalculatorPtr TrainRandomForest(const TMiniBatch &x, const std::vector<size_t> &y, size_t classCount, size_t maxDepth, size_t treeCount);
    TCalculatorPtr TrainFullRandomForest(const TMiniBatch &x, const std::vector<size_t> &y, size_t classCount, size_t maxDepth, size_t treeCount);
    TCalculatorPtr TrainCascadeForest(TMiniBatch &x, const std::vector<size_t> &y, size_t classCount, size_t maxDepth, size_t treeCount, size_t levelCount);

} // namespace NGCForest

