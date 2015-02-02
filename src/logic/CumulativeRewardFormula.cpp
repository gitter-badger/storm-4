#include "src/logic/CumulativeRewardFormula.h"

namespace storm {
    namespace logic {
        CumulativeRewardFormula::CumulativeRewardFormula(uint_fast64_t stepBound) : stepBound(stepBound) {
            // Intentionally left empty.
        }
        
        bool CumulativeRewardFormula::isCumulativeRewardFormula() const {
            return true;
        }
        
        uint_fast64_t CumulativeRewardFormula::getStepBound() const {
            return stepBound;
        }
        
        std::ostream& CumulativeRewardFormula::writeToStream(std::ostream& out) const {
            out << "C<=" << stepBound;
            return out;
        }
    }
}