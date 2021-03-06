#include "storm/modelchecker/prctl/HybridDtmcPrctlModelChecker.h"

#include "storm/modelchecker/prctl/helper/HybridDtmcPrctlHelper.h"
#include "storm/modelchecker/prctl/helper/SparseDtmcPrctlHelper.h"

#include "storm/storage/dd/Odd.h"
#include "storm/storage/dd/DdManager.h"

#include "storm/utility/macros.h"
#include "storm/utility/graph.h"

#include "storm/models/symbolic/StandardRewardModel.h"

#include "storm/settings/modules/GeneralSettings.h"

#include "storm/modelchecker/results/SymbolicQualitativeCheckResult.h"
#include "storm/modelchecker/results/SymbolicQuantitativeCheckResult.h"
#include "storm/modelchecker/results/HybridQuantitativeCheckResult.h"

#include "storm/logic/FragmentSpecification.h"

#include "storm/exceptions/InvalidStateException.h"
#include "storm/exceptions/InvalidPropertyException.h"
#include "storm/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace modelchecker {
        template<typename ModelType>
        HybridDtmcPrctlModelChecker<ModelType>::HybridDtmcPrctlModelChecker(ModelType const& model, std::unique_ptr<storm::solver::LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory) : SymbolicPropositionalModelChecker<ModelType>(model), linearEquationSolverFactory(std::move(linearEquationSolverFactory)) {
            // Intentionally left empty.
        }
        
        template<typename ModelType>
        HybridDtmcPrctlModelChecker<ModelType>::HybridDtmcPrctlModelChecker(ModelType const& model) : SymbolicPropositionalModelChecker<ModelType>(model), linearEquationSolverFactory(std::make_unique<storm::solver::GeneralLinearEquationSolverFactory<ValueType>>()) {
            // Intentionally left empty.
        }
        
        template<typename ModelType>
        bool HybridDtmcPrctlModelChecker<ModelType>::canHandle(CheckTask<storm::logic::Formula, ValueType> const& checkTask) const {
            storm::logic::Formula const& formula = checkTask.getFormula();
            return formula.isInFragment(storm::logic::prctl().setLongRunAverageRewardFormulasAllowed(true).setLongRunAverageProbabilitiesAllowed(true));
        }
                
        template<typename ModelType>
        std::unique_ptr<CheckResult> HybridDtmcPrctlModelChecker<ModelType>::computeUntilProbabilities(CheckTask<storm::logic::UntilFormula, ValueType> const& checkTask) {
            storm::logic::UntilFormula const& pathFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> leftResultPointer = this->check(pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula());
            SymbolicQualitativeCheckResult<DdType> const& leftResult = leftResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            SymbolicQualitativeCheckResult<DdType> const& rightResult = rightResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            return storm::modelchecker::helper::HybridDtmcPrctlHelper<DdType, ValueType>::computeUntilProbabilities(this->getModel(), this->getModel().getTransitionMatrix(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), checkTask.isQualitativeSet(), *this->linearEquationSolverFactory);
        }
        
        template<typename ModelType>
        std::unique_ptr<CheckResult> HybridDtmcPrctlModelChecker<ModelType>::computeGloballyProbabilities(CheckTask<storm::logic::GloballyFormula, ValueType> const& checkTask) {
            storm::logic::GloballyFormula const& pathFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> subResultPointer = this->check(pathFormula.getSubformula());
            SymbolicQualitativeCheckResult<DdType> const& subResult = subResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            return storm::modelchecker::helper::HybridDtmcPrctlHelper<DdType, ValueType>::computeGloballyProbabilities(this->getModel(), this->getModel().getTransitionMatrix(), subResult.getTruthValuesVector(), checkTask.isQualitativeSet(), *this->linearEquationSolverFactory);
        }
        
        template<typename ModelType>
        std::unique_ptr<CheckResult> HybridDtmcPrctlModelChecker<ModelType>::computeNextProbabilities(CheckTask<storm::logic::NextFormula, ValueType> const& checkTask) {
            storm::logic::NextFormula const& pathFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> subResultPointer = this->check(pathFormula.getSubformula());
            SymbolicQualitativeCheckResult<DdType> const& subResult = subResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            return storm::modelchecker::helper::HybridDtmcPrctlHelper<DdType, ValueType>::computeNextProbabilities(this->getModel(), this->getModel().getTransitionMatrix(), subResult.getTruthValuesVector());
        }
                
        template<typename ModelType>
        std::unique_ptr<CheckResult> HybridDtmcPrctlModelChecker<ModelType>::computeBoundedUntilProbabilities(CheckTask<storm::logic::BoundedUntilFormula, ValueType> const& checkTask) {
            storm::logic::BoundedUntilFormula const& pathFormula = checkTask.getFormula();
            STORM_LOG_THROW(!pathFormula.hasLowerBound() && pathFormula.hasUpperBound(), storm::exceptions::InvalidPropertyException, "Formula needs to have single upper time bound.");
            STORM_LOG_THROW(pathFormula.hasIntegerUpperBound(), storm::exceptions::InvalidPropertyException, "Formula needs to have discrete upper time bound.");
            std::unique_ptr<CheckResult> leftResultPointer = this->check(pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula());
            SymbolicQualitativeCheckResult<DdType> const& leftResult = leftResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            SymbolicQualitativeCheckResult<DdType> const& rightResult = rightResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            return storm::modelchecker::helper::HybridDtmcPrctlHelper<DdType, ValueType>::computeBoundedUntilProbabilities(this->getModel(), this->getModel().getTransitionMatrix(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), pathFormula.getNonStrictUpperBound<uint64_t>(), *this->linearEquationSolverFactory);
        }
                
        template<typename ModelType>
        std::unique_ptr<CheckResult> HybridDtmcPrctlModelChecker<ModelType>::computeCumulativeRewards(storm::logic::RewardMeasureType, CheckTask<storm::logic::CumulativeRewardFormula, ValueType> const& checkTask) {
            storm::logic::CumulativeRewardFormula const& rewardPathFormula = checkTask.getFormula();
            STORM_LOG_THROW(rewardPathFormula.hasIntegerBound(), storm::exceptions::InvalidPropertyException, "Formula needs to have a discrete time bound.");
            return storm::modelchecker::helper::HybridDtmcPrctlHelper<DdType, ValueType>::computeCumulativeRewards(this->getModel(), this->getModel().getTransitionMatrix(), checkTask.isRewardModelSet() ? this->getModel().getRewardModel(checkTask.getRewardModel()) : this->getModel().getRewardModel(""), rewardPathFormula.getNonStrictBound<uint64_t>(), *this->linearEquationSolverFactory);
        }
                
        template<typename ModelType>
        std::unique_ptr<CheckResult> HybridDtmcPrctlModelChecker<ModelType>::computeInstantaneousRewards(storm::logic::RewardMeasureType, CheckTask<storm::logic::InstantaneousRewardFormula, ValueType> const& checkTask) {
            storm::logic::InstantaneousRewardFormula const& rewardPathFormula = checkTask.getFormula();
            STORM_LOG_THROW(rewardPathFormula.hasIntegerBound(), storm::exceptions::InvalidPropertyException, "Formula needs to have a discrete time bound.");
            return storm::modelchecker::helper::HybridDtmcPrctlHelper<DdType, ValueType>::computeInstantaneousRewards(this->getModel(), this->getModel().getTransitionMatrix(), checkTask.isRewardModelSet() ? this->getModel().getRewardModel(checkTask.getRewardModel()) : this->getModel().getRewardModel(""), rewardPathFormula.getBound<uint64_t>(), *this->linearEquationSolverFactory);
        }
        
        template<typename ModelType>
        std::unique_ptr<CheckResult> HybridDtmcPrctlModelChecker<ModelType>::computeReachabilityRewards(storm::logic::RewardMeasureType, CheckTask<storm::logic::EventuallyFormula, ValueType> const& checkTask) {
            storm::logic::EventuallyFormula const& eventuallyFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> subResultPointer = this->check(eventuallyFormula.getSubformula());
            SymbolicQualitativeCheckResult<DdType> const& subResult = subResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            return storm::modelchecker::helper::HybridDtmcPrctlHelper<DdType, ValueType>::computeReachabilityRewards(this->getModel(), this->getModel().getTransitionMatrix(), checkTask.isRewardModelSet() ? this->getModel().getRewardModel(checkTask.getRewardModel()) : this->getModel().getRewardModel(""), subResult.getTruthValuesVector(), checkTask.isQualitativeSet(), *this->linearEquationSolverFactory);
        }


        template<typename ModelType>
        std::unique_ptr<CheckResult> HybridDtmcPrctlModelChecker<ModelType>::computeLongRunAverageProbabilities(CheckTask<storm::logic::StateFormula, ValueType> const& checkTask) {
            storm::logic::StateFormula const& stateFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> subResultPointer = this->check(stateFormula);
            SymbolicQualitativeCheckResult<DdType> const& subResult = subResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            
            return storm::modelchecker::helper::HybridDtmcPrctlHelper<DdType, ValueType>::computeLongRunAverageProbabilities(this->getModel(), this->getModel().getTransitionMatrix(), subResult.getTruthValuesVector(), *this->linearEquationSolverFactory);
        }
        
        template<typename ModelType>
        std::unique_ptr<CheckResult> HybridDtmcPrctlModelChecker<ModelType>::computeLongRunAverageRewards(storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::LongRunAverageRewardFormula, ValueType> const& checkTask) {
            return storm::modelchecker::helper::HybridDtmcPrctlHelper<DdType, ValueType>::computeLongRunAverageRewards(this->getModel(), this->getModel().getTransitionMatrix(), checkTask.isRewardModelSet() ? this->getModel().getRewardModel(checkTask.getRewardModel()) : this->getModel().getRewardModel(""), *this->linearEquationSolverFactory);
        }
        
        template class HybridDtmcPrctlModelChecker<storm::models::symbolic::Dtmc<storm::dd::DdType::CUDD, double>>;
        template class HybridDtmcPrctlModelChecker<storm::models::symbolic::Dtmc<storm::dd::DdType::Sylvan, double>>;

        template class HybridDtmcPrctlModelChecker<storm::models::symbolic::Dtmc<storm::dd::DdType::Sylvan, storm::RationalNumber>>;
        template class HybridDtmcPrctlModelChecker<storm::models::symbolic::Dtmc<storm::dd::DdType::Sylvan, storm::RationalFunction>>;
    }
}
