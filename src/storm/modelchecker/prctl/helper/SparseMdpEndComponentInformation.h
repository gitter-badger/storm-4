#pragma once

#include <vector>
#include <cstdint>

namespace storm {
    namespace storage {
        class BitVector;
        
        template <typename ValueType>
        class SparseMatrix;
        
        template <typename ValueType>
        class MaximalEndComponentDecomposition;
    }
    
    namespace modelchecker {
        namespace helper {
            
            template<typename ValueType>
            class SparseMdpEndComponentInformation {
            public:
                SparseMdpEndComponentInformation(storm::storage::MaximalEndComponentDecomposition<ValueType> const& endComponentDecomposition, storm::storage::BitVector const& maybeStates);
                
                bool isMaybeStateInEc(uint64_t maybeState) const;
                bool isStateInEc(uint64_t state) const;
                
                /*!
                 * Retrieves for each maybe state the number of maybe states not contained in ECs with an index smaller
                 * than the requested one.
                 */
                std::vector<uint64_t> const& getNumberOfMaybeStatesNotInEcBeforeIndices() const;
                
                /*!
                 * Retrieves the total number of maybe states on in ECs.
                 */
                uint64_t getNumberOfMaybeStatesNotInEc() const;
                
                /*!
                 * Retrieves the EC of the state (result may be NOT_IN_EC).
                 */
                uint64_t getEc(uint64_t state) const;
                
                /*!
                 * Retrieves the row group of the state after end component elimination.
                 */
                uint64_t getRowGroupAfterElimination(uint64_t state) const;

                bool getEliminatedEndComponents() const;

                uint64_t getNotInEcMarker() const;

                static SparseMdpEndComponentInformation<ValueType> eliminateEndComponents(storm::storage::MaximalEndComponentDecomposition<ValueType> const& endComponentDecomposition, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::BitVector const& maybeStates, storm::storage::BitVector const* sumColumns, storm::storage::BitVector const* selectedChoices, std::vector<ValueType> const* summand, storm::storage::SparseMatrix<ValueType>& submatrix, std::vector<ValueType>* columnSumVector, std::vector<ValueType>* summandResultVector);
                
                static SparseMdpEndComponentInformation<ValueType> eliminateEndComponents(storm::storage::MaximalEndComponentDecomposition<ValueType> const& endComponentDecomposition, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType>& rhsVector, storm::storage::BitVector const& maybeStates, storm::storage::SparseMatrix<ValueType>& submatrix, std::vector<ValueType>& subvector);
                
                void setValues(std::vector<ValueType>& result, storm::storage::BitVector const& maybeStates, std::vector<ValueType> const& fromResult);
                
            private:
                // A constant that marks that a state is not contained in any EC.
                uint64_t NOT_IN_EC;
                
                // A flag storing whether end components have been eliminated.
                bool eliminatedEndComponents;

                // Data about end components.
                std::vector<uint64_t> maybeStatesBefore;
                std::vector<uint64_t> maybeStatesNotInEcBefore;
                uint64_t numberOfMaybeStatesInEc;
                uint64_t numberOfMaybeStatesNotInEc;
                uint64_t numberOfEc;
                std::vector<uint64_t> maybeStateToEc;
            };
                        
        }
    }
}
