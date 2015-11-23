#include "src/storage/dd/sylvan/InternalSylvanDdManager.h"

#include "src/utility/macros.h"
#include "src/exceptions/NotImplementedException.h"
#include "src/exceptions/NotSupportedException.h"

namespace storm {
    namespace dd {
        uint_fast64_t InternalDdManager<DdType::Sylvan>::numberOfInstances = 0;
        uint_fast64_t InternalDdManager<DdType::Sylvan>::nextFreeVariableIndex = 0;
        
        InternalDdManager<DdType::Sylvan>::InternalDdManager() {
            if (numberOfInstances == 0) {
                // Initialize lace: auto-detecting number of workers.
                lace_init(0, 1000000);
                lace_startup(0, 0, 0);
                
                sylvan::Sylvan::initPackage(1ull << 16, 1ull << 32, 1ull << 16, 1ull << 32);
                sylvan::Sylvan::initBdd(1);
                sylvan::Sylvan::initMtbdd();
            }
            ++numberOfInstances;
        }
        
        InternalDdManager<DdType::Sylvan>::~InternalDdManager() {
            --numberOfInstances;
            if (numberOfInstances == 0) {
                sylvan::Sylvan::quitPackage();
            }
        }
        
        InternalBdd<DdType::Sylvan> InternalDdManager<DdType::Sylvan>::getBddOne() const {
            return InternalBdd<DdType::Sylvan>(this, sylvan::Bdd::bddOne());
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalDdManager<DdType::Sylvan>::getAddOne() const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        InternalBdd<DdType::Sylvan> InternalDdManager<DdType::Sylvan>::getBddZero() const {
            return InternalBdd<DdType::Sylvan>(this, sylvan::Bdd::bddZero());
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalDdManager<DdType::Sylvan>::getAddZero() const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        template<typename ValueType>
        InternalAdd<DdType::Sylvan, ValueType> InternalDdManager<DdType::Sylvan>::getConstant(ValueType const& value) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not yet implemented.");
        }
        
        std::pair<InternalBdd<DdType::Sylvan>, InternalBdd<DdType::Sylvan>> InternalDdManager<DdType::Sylvan>::createNewDdVariablePair() {
            InternalBdd<DdType::Sylvan> first = InternalBdd<DdType::Sylvan>(this, sylvan::Bdd::bddVar(nextFreeVariableIndex));
            InternalBdd<DdType::Sylvan> second = InternalBdd<DdType::Sylvan>(this, sylvan::Bdd::bddVar(nextFreeVariableIndex + 1));
            nextFreeVariableIndex += 2;
            return std::make_pair(first, second);
        }
        
        void InternalDdManager<DdType::Sylvan>::allowDynamicReordering(bool value) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Operation is not supported by sylvan.");
        }
        
        bool InternalDdManager<DdType::Sylvan>::isDynamicReorderingAllowed() const {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Operation is not supported by sylvan.");
        }
        
        void InternalDdManager<DdType::Sylvan>::triggerReordering() {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Operation is not supported by sylvan.");
        }
                
        template InternalAdd<DdType::Sylvan, double> InternalDdManager<DdType::Sylvan>::getAddOne() const;
        template InternalAdd<DdType::Sylvan, uint_fast64_t> InternalDdManager<DdType::Sylvan>::getAddOne() const;
        
        template InternalAdd<DdType::Sylvan, double> InternalDdManager<DdType::Sylvan>::getAddZero() const;
        template InternalAdd<DdType::Sylvan, uint_fast64_t> InternalDdManager<DdType::Sylvan>::getAddZero() const;
        
        template InternalAdd<DdType::Sylvan, double> InternalDdManager<DdType::Sylvan>::getConstant(double const& value) const;
        template InternalAdd<DdType::Sylvan, uint_fast64_t> InternalDdManager<DdType::Sylvan>::getConstant(uint_fast64_t const& value) const;
    }
}