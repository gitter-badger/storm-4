#include "src/storage/bisimulation/BisimulationDecomposition.h"

#include <chrono>

#include "src/models/sparse/Dtmc.h"
#include "src/models/sparse/Ctmc.h"
#include "src/models/sparse/StandardRewardModel.h"

#include "src/storage/bisimulation/DeterministicBlockData.h"

#include "src/modelchecker/propositional/SparsePropositionalModelChecker.h"
#include "src/modelchecker/results/ExplicitQualitativeCheckResult.h"

#include "src/settings/SettingsManager.h"
#include "src/settings/modules/GeneralSettings.h"

#include "src/utility/macros.h"
#include "src/exceptions/IllegalFunctionCallException.h"
#include "src/exceptions/InvalidOptionException.h"

namespace storm {
    namespace storage {
        
        using namespace bisimulation;
        
        template<typename ModelType, typename BlockDataType>
        BisimulationDecomposition<ModelType, BlockDataType>::Options::Options(ModelType const& model, storm::logic::Formula const& formula) : Options() {
            this->preserveSingleFormula(model, formula);
        }
        
        template<typename ModelType, typename BlockDataType>
        BisimulationDecomposition<ModelType, BlockDataType>::Options::Options(ModelType const& model, std::vector<std::shared_ptr<storm::logic::Formula>> const& formulas) : Options() {
            if (formulas.empty()) {
                this->respectedAtomicPropositions = model.getStateLabeling().getLabels();
                this->keepRewards = true;
            } if (formulas.size() == 1) {
                this->preserveSingleFormula(model, *formulas.front());
            } else {
                for (auto const& formula : formulas) {
                    preserveFormula(model, *formula);
                }
            }
        }
        
        template<typename ModelType, typename BlockDataType>
        BisimulationDecomposition<ModelType, BlockDataType>::Options::Options() : measureDrivenInitialPartition(false), phiStates(), psiStates(), respectedAtomicPropositions(), keepRewards(false), type(BisimulationType::Strong), bounded(false), buildQuotient(true) {
            // Intentionally left empty.
        }
        
        template<typename ModelType, typename BlockDataType>
        void BisimulationDecomposition<ModelType, BlockDataType>::Options::preserveFormula(ModelType const& model, storm::logic::Formula const& formula) {
            // Disable the measure driven initial partition.
            measureDrivenInitialPartition = false;
            phiStates = boost::none;
            psiStates = boost::none;
            
            // Preserve rewards if necessary.
            keepRewards = keepRewards || formula.containsRewardOperator();
            
            // Preserve bounded properties if necessary.
            bounded = bounded || (formula.containsBoundedUntilFormula() || formula.containsNextFormula());
            
            // Compute the relevant labels and expressions.
            this->addToRespectedAtomicPropositions(formula.getAtomicExpressionFormulas(), formula.getAtomicLabelFormulas());
        }
        
        template<typename ModelType, typename BlockDataType>
        void BisimulationDecomposition<ModelType, BlockDataType>::Options::preserveSingleFormula(ModelType const& model, storm::logic::Formula const& formula) {
            keepRewards = formula.containsRewardOperator();
            
            // We need to preserve bounded properties iff the formula contains a bounded until or a next subformula.
            bounded = formula.containsBoundedUntilFormula() || formula.containsNextFormula();
            
            // Compute the relevant labels and expressions.
            this->addToRespectedAtomicPropositions(formula.getAtomicExpressionFormulas(), formula.getAtomicLabelFormulas());

            // Check whether measure driven initial partition is possible and, if so, set it.
            this->checkAndSetMeasureDrivenInitialPartition(model, formula);
        }
        
        template<typename ModelType, typename BlockDataType>
        void BisimulationDecomposition<ModelType, BlockDataType>::Options::checkAndSetMeasureDrivenInitialPartition(ModelType const& model, storm::logic::Formula const& formula) {
            std::shared_ptr<storm::logic::Formula const> newFormula = formula.asSharedPointer();
            
            if (formula.isProbabilityOperatorFormula()) {
                newFormula = formula.asProbabilityOperatorFormula().getSubformula().asSharedPointer();
            } else if (formula.isRewardOperatorFormula()) {
                newFormula = formula.asRewardOperatorFormula().getSubformula().asSharedPointer();
            }
            
            std::shared_ptr<storm::logic::Formula const> leftSubformula = std::make_shared<storm::logic::BooleanLiteralFormula>(true);
            std::shared_ptr<storm::logic::Formula const> rightSubformula;
            if (newFormula->isUntilFormula()) {
                leftSubformula = newFormula->asUntilFormula().getLeftSubformula().asSharedPointer();
                rightSubformula = newFormula->asUntilFormula().getRightSubformula().asSharedPointer();
                if (leftSubformula->isPropositionalFormula() && rightSubformula->isPropositionalFormula()) {
                    measureDrivenInitialPartition = true;
                }
            } else if (newFormula->isEventuallyFormula()) {
                rightSubformula = newFormula->asEventuallyFormula().getSubformula().asSharedPointer();
                if (rightSubformula->isPropositionalFormula()) {
                    measureDrivenInitialPartition = true;
                }
            } else if (newFormula->isReachabilityRewardFormula()) {
                rightSubformula = newFormula->asReachabilityRewardFormula().getSubformula().asSharedPointer();
                if (rightSubformula->isPropositionalFormula()) {
                    measureDrivenInitialPartition = true;
                }
            }
            
            if (measureDrivenInitialPartition) {
                storm::modelchecker::SparsePropositionalModelChecker<ModelType> checker(model);
                std::unique_ptr<storm::modelchecker::CheckResult> phiStatesCheckResult = checker.check(*leftSubformula);
                std::unique_ptr<storm::modelchecker::CheckResult> psiStatesCheckResult = checker.check(*rightSubformula);
                phiStates = phiStatesCheckResult->asExplicitQualitativeCheckResult().getTruthValuesVector();
                psiStates = psiStatesCheckResult->asExplicitQualitativeCheckResult().getTruthValuesVector();
            }
        }
        
        template<typename ModelType, typename BlockDataType>
        void BisimulationDecomposition<ModelType, BlockDataType>::Options::addToRespectedAtomicPropositions(std::vector<std::shared_ptr<storm::logic::AtomicExpressionFormula const>> const& expressions, std::vector<std::shared_ptr<storm::logic::AtomicLabelFormula const>> const& labels) {
            std::set<std::string> labelsToRespect;
            for (auto const& labelFormula : labels) {
                labelsToRespect.insert(labelFormula->getLabel());
            }
            for (auto const& expressionFormula : expressions) {
                labelsToRespect.insert(expressionFormula->toString());
            }
            if (!respectedAtomicPropositions) {
                respectedAtomicPropositions = labelsToRespect;
            } else {
                respectedAtomicPropositions.get().insert(labelsToRespect.begin(), labelsToRespect.end());
            }
        }
        
        template<typename ModelType, typename BlockDataType>
        BisimulationDecomposition<ModelType, BlockDataType>::BisimulationDecomposition(ModelType const& model, Options const& options) : model(model), backwardTransitions(model.getBackwardTransitions()), options(options), partition(), comparator(), quotient(nullptr) {
            // Fix the respected atomic propositions if they were not explicitly given.
            if (!this->options.respectedAtomicPropositions) {
                this->options.respectedAtomicPropositions = model.getStateLabeling().getLabels();
            }
        }
        
        template<typename ModelType, typename BlockDataType>
        void BisimulationDecomposition<ModelType, BlockDataType>::computeBisimulationDecomposition() {
            std::chrono::high_resolution_clock::time_point totalStart = std::chrono::high_resolution_clock::now();

            std::chrono::high_resolution_clock::time_point initialPartitionStart = std::chrono::high_resolution_clock::now();
            // initialize the initial partition.
            if (options.measureDrivenInitialPartition) {
                STORM_LOG_THROW(options.phiStates, storm::exceptions::InvalidOptionException, "Unable to compute measure-driven initial partition without phi states.");
                STORM_LOG_THROW(options.psiStates, storm::exceptions::InvalidOptionException, "Unable to compute measure-driven initial partition without psi states.");
                this->initializeMeasureDrivenPartition();
            } else {
                this->initializeLabelBasedPartition();
            }
            std::chrono::high_resolution_clock::duration initialPartitionTime = std::chrono::high_resolution_clock::now() - initialPartitionStart;
            
            std::chrono::high_resolution_clock::time_point refinementStart = std::chrono::high_resolution_clock::now();
            this->performPartitionRefinement();
            std::chrono::high_resolution_clock::duration refinementTime = std::chrono::high_resolution_clock::now() - refinementStart;
            
            std::chrono::high_resolution_clock::time_point extractionStart = std::chrono::high_resolution_clock::now();
            this->extractDecompositionBlocks();
            std::chrono::high_resolution_clock::duration extractionTime = std::chrono::high_resolution_clock::now() - extractionStart;
            
            std::chrono::high_resolution_clock::time_point quotientBuildStart = std::chrono::high_resolution_clock::now();
            if (options.buildQuotient) {
                this->buildQuotient();
            }
            std::chrono::high_resolution_clock::duration quotientBuildTime = std::chrono::high_resolution_clock::now() - quotientBuildStart;
            
            std::chrono::high_resolution_clock::duration totalTime = std::chrono::high_resolution_clock::now() - totalStart;
            
            if (storm::settings::generalSettings().isShowStatisticsSet()) {
                std::chrono::milliseconds initialPartitionTimeInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(initialPartitionTime);
                std::chrono::milliseconds refinementTimeInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(refinementTime);
                std::chrono::milliseconds extractionTimeInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(extractionTime);
                std::chrono::milliseconds quotientBuildTimeInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(quotientBuildTime);
                std::chrono::milliseconds totalTimeInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(totalTime);
                std::cout << std::endl;
                std::cout << "Time breakdown:" << std::endl;
                std::cout << "    * time for initial partition: " << initialPartitionTimeInMilliseconds.count() << "ms" << std::endl;
                std::cout << "    * time for partitioning: " << refinementTimeInMilliseconds.count() << "ms" << std::endl;
                std::cout << "    * time for extraction: " << extractionTimeInMilliseconds.count() << "ms" << std::endl;
                std::cout << "    * time for building quotient: " << quotientBuildTimeInMilliseconds.count() << "ms" << std::endl;
                std::cout << "------------------------------------------" << std::endl;
                std::cout << "    * total time: " << totalTimeInMilliseconds.count() << "ms" << std::endl;
                std::cout << std::endl;
            }
        }
        
        template<typename ModelType, typename BlockDataType>
        void BisimulationDecomposition<ModelType, BlockDataType>::performPartitionRefinement() {
            // Insert all blocks into the splitter queue as a (potential) splitter.
            std::deque<Block<BlockDataType>*> splitterQueue;
            std::for_each(partition.getBlocks().begin(), partition.getBlocks().end(), [&] (std::unique_ptr<Block<BlockDataType>> const& block) { splitterQueue.push_back(block.get()); } );
            
            // Then perform the actual splitting until there are no more splitters.
            uint_fast64_t iterations = 0;
            while (!splitterQueue.empty()) {
                ++iterations;
                // Optionally: sort the splitter queue according to some criterion (here: prefer small splitters).
                std::sort(splitterQueue.begin(), splitterQueue.end(), [] (Block<BlockDataType> const* b1, Block<BlockDataType> const* b2) { return b1->getNumberOfStates() < b2->getNumberOfStates(); } );
                
                // Get and prepare the next splitter.
                Block<BlockDataType>* splitter = splitterQueue.front();
                splitterQueue.pop_front();
                splitter->data().setSplitter(false);
                
                // Now refine the partition using the current splitter.
                refinePartitionBasedOnSplitter(*splitter, splitterQueue);
            }
        }
        
        template<typename ModelType, typename BlockDataType>
        std::shared_ptr<ModelType> BisimulationDecomposition<ModelType, BlockDataType>::getQuotient() const {
            STORM_LOG_THROW(this->quotient != nullptr, storm::exceptions::IllegalFunctionCallException, "Unable to retrieve quotient model from bisimulation decomposition, because it was not built.");
            return this->quotient;
        }

        template<typename ModelType, typename BlockDataType>
        void BisimulationDecomposition<ModelType, BlockDataType>::splitInitialPartitionBasedOnStateRewards() {
            std::vector<ValueType> const& stateRewardVector = model.getUniqueRewardModel()->second.getStateRewardVector();
            partition.split([&stateRewardVector] (storm::storage::sparse::state_type const& a, storm::storage::sparse::state_type const& b) { return stateRewardVector[a] < stateRewardVector[b]; });
        }
        
        template<typename ModelType, typename BlockDataType>
        void BisimulationDecomposition<ModelType, BlockDataType>::initializeLabelBasedPartition() {
            partition = storm::storage::bisimulation::Partition<BlockDataType>(model.getNumberOfStates());

            for (auto const& label : options.respectedAtomicPropositions.get()) {
                if (label == "init") {
                    continue;
                }
                partition.splitStates(model.getStates(label));
            }
            
            // If the model has state rewards, we need to consider them, because otherwise reward properties are not
            // preserved.
            if (options.keepRewards && model.hasRewardModel()) {
                this->splitInitialPartitionBasedOnStateRewards();
            }
        }
        
        template<typename ModelType, typename BlockDataType>
        void BisimulationDecomposition<ModelType, BlockDataType>::initializeMeasureDrivenPartition() {
            std::pair<storm::storage::BitVector, storm::storage::BitVector> statesWithProbability01 = this->getStatesWithProbability01();
            
            boost::optional<storm::storage::sparse::state_type> representativePsiState;
            if (!options.psiStates.get().empty()) {
                representativePsiState = *options.psiStates.get().begin();
            }
            
            partition = storm::storage::bisimulation::Partition<BlockDataType>(model.getNumberOfStates(), statesWithProbability01.first, options.bounded || options.keepRewards ? options.psiStates.get() : statesWithProbability01.second, representativePsiState);
            
            // If the model has state rewards, we need to consider them, because otherwise reward properties are not
            // preserved.
            if (options.keepRewards && model.hasRewardModel()) {
                this->splitInitialPartitionBasedOnStateRewards();
            }
        }
        
        template<typename ModelType, typename BlockDataType>
        void BisimulationDecomposition<ModelType, BlockDataType>::extractDecompositionBlocks() {
            // Now move the states from the internal partition into their final place in the decomposition. We do so in
            // a way that maintains the block IDs as indices.
            this->blocks.resize(partition.size());
            for (auto const& blockPtr : partition.getBlocks()) {
                // We need to sort the states to allow for rapid construction of the blocks.
                partition.sortBlock(*blockPtr);
                
                // Convert the state-value-pairs to states only.
                this->blocks[blockPtr->getId()] = block_type(partition.begin(*blockPtr), partition.end(*blockPtr), true);
            }
        }
        
        template class BisimulationDecomposition<storm::models::sparse::Dtmc<double>, bisimulation::DeterministicBlockData>;
        template class BisimulationDecomposition<storm::models::sparse::Ctmc<double>, bisimulation::DeterministicBlockData>;
        
#ifdef STORM_HAVE_CARL
        template class BisimulationDecomposition<storm::models::sparse::Dtmc<storm::RationalFunction>, bisimulation::DeterministicBlockData>;
        template class BisimulationDecomposition<storm::models::sparse::Ctmc<storm::RationalFunction>, bisimulation::DeterministicBlockData>;
#endif
    }
}