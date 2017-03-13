#include <algorithm>
#include "combiner.h"


namespace NGCForest {

    // TCombiner
    void TCombiner::Combine(const std::vector<TConstFeaturesPtr> &source, TFeatures &result) {
        result.resize(source.front()->size());
        DoCombine(source, result);
    }


    // TMajorityVote
    namespace {
        size_t ArgMax(const TFeatures &values) {
            size_t res = 0;
            for (size_t i = 1; i < values.size(); ++i) {
                if (values[i] > values[res])
                    res = i;
            }
            return res;
        }
    }

    void TMajorityVote::DoCombine(const std::vector<TConstFeaturesPtr> &source, TFeatures &result) {
        std::fill(result.begin(), result.end(), 0.0);
        for (size_t i = 0; i < source.size(); ++i) {
            size_t indexOfMax = ArgMax(*source[i]);
            result[indexOfMax] += 1.0;
        }
        for (double &val : result)
            val /= source.size();
    }

} // namespace NGCForest
