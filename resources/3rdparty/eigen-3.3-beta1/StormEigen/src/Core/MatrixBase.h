// This file is part of Eigen, a lightweight C++ template library
// for linear algebra.
//
// Copyright (C) 2006-2009 Benoit Jacob <jacob.benoit.1@gmail.com>
// Copyright (C) 2008 Gael Guennebaud <gael.guennebaud@inria.fr>
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef STORMEIGEN_MATRIXBASE_H
#define STORMEIGEN_MATRIXBASE_H

namespace StormEigen {

/** \class MatrixBase
  * \ingroup Core_Module
  *
  * \brief Base class for all dense matrices, vectors, and expressions
  *
  * This class is the base that is inherited by all matrix, vector, and related expression
  * types. Most of the Eigen API is contained in this class, and its base classes. Other important
  * classes for the Eigen API are Matrix, and VectorwiseOp.
  *
  * Note that some methods are defined in other modules such as the \ref LU_Module LU module
  * for all functions related to matrix inversions.
  *
  * \tparam Derived is the derived type, e.g. a matrix type, or an expression, etc.
  *
  * When writing a function taking Eigen objects as argument, if you want your function
  * to take as argument any matrix, vector, or expression, just let it take a
  * MatrixBase argument. As an example, here is a function printFirstRow which, given
  * a matrix, vector, or expression \a x, prints the first row of \a x.
  *
  * \code
    template<typename Derived>
    void printFirstRow(const StormEigen::MatrixBase<Derived>& x)
    {
      cout << x.row(0) << endl;
    }
  * \endcode
  *
  * This class can be extended with the help of the plugin mechanism described on the page
  * \ref TopicCustomizingEigen by defining the preprocessor symbol \c STORMEIGEN_MATRIXBASE_PLUGIN.
  *
  * \sa \ref TopicClassHierarchy
  */
template<typename Derived> class MatrixBase
  : public DenseBase<Derived>
{
  public:
#ifndef STORMEIGEN_PARSED_BY_DOXYGEN
    typedef MatrixBase StorageBaseType;
    typedef typename internal::traits<Derived>::StorageKind StorageKind;
    typedef typename internal::traits<Derived>::StorageIndex StorageIndex;
    typedef typename internal::traits<Derived>::Scalar Scalar;
    typedef typename internal::packet_traits<Scalar>::type PacketScalar;
    typedef typename NumTraits<Scalar>::Real RealScalar;

    typedef DenseBase<Derived> Base;
    using Base::RowsAtCompileTime;
    using Base::ColsAtCompileTime;
    using Base::SizeAtCompileTime;
    using Base::MaxRowsAtCompileTime;
    using Base::MaxColsAtCompileTime;
    using Base::MaxSizeAtCompileTime;
    using Base::IsVectorAtCompileTime;
    using Base::Flags;
    
    using Base::derived;
    using Base::const_cast_derived;
    using Base::rows;
    using Base::cols;
    using Base::size;
    using Base::coeff;
    using Base::coeffRef;
    using Base::lazyAssign;
    using Base::eval;
    using Base::operator+=;
    using Base::operator-=;
    using Base::operator*=;
    using Base::operator/=;
    using Base::operator*;
    using Base::operator/;

    typedef typename Base::CoeffReturnType CoeffReturnType;
    typedef typename Base::ConstTransposeReturnType ConstTransposeReturnType;
    typedef typename Base::RowXpr RowXpr;
    typedef typename Base::ColXpr ColXpr;
#endif // not STORMEIGEN_PARSED_BY_DOXYGEN



#ifndef STORMEIGEN_PARSED_BY_DOXYGEN
    /** type of the equivalent square matrix */
    typedef Matrix<Scalar,STORMEIGEN_SIZE_MAX(RowsAtCompileTime,ColsAtCompileTime),
                          STORMEIGEN_SIZE_MAX(RowsAtCompileTime,ColsAtCompileTime)> SquareMatrixType;
#endif // not STORMEIGEN_PARSED_BY_DOXYGEN

    /** \returns the size of the main diagonal, which is min(rows(),cols()).
      * \sa rows(), cols(), SizeAtCompileTime. */
    STORMEIGEN_DEVICE_FUNC
    inline Index diagonalSize() const { return (std::min)(rows(),cols()); }

    typedef typename Base::PlainObject PlainObject;

#ifndef STORMEIGEN_PARSED_BY_DOXYGEN
    /** \internal Represents a matrix with all coefficients equal to one another*/
    typedef CwiseNullaryOp<internal::scalar_constant_op<Scalar>,PlainObject> ConstantReturnType;
    /** \internal the return type of MatrixBase::adjoint() */
    typedef typename internal::conditional<NumTraits<Scalar>::IsComplex,
                        CwiseUnaryOp<internal::scalar_conjugate_op<Scalar>, ConstTransposeReturnType>,
                        ConstTransposeReturnType
                     >::type AdjointReturnType;
    /** \internal Return type of eigenvalues() */
    typedef Matrix<std::complex<RealScalar>, internal::traits<Derived>::ColsAtCompileTime, 1, ColMajor> EigenvaluesReturnType;
    /** \internal the return type of identity */
    typedef CwiseNullaryOp<internal::scalar_identity_op<Scalar>,PlainObject> IdentityReturnType;
    /** \internal the return type of unit vectors */
    typedef Block<const CwiseNullaryOp<internal::scalar_identity_op<Scalar>, SquareMatrixType>,
                  internal::traits<Derived>::RowsAtCompileTime,
                  internal::traits<Derived>::ColsAtCompileTime> BasisReturnType;
#endif // not STORMEIGEN_PARSED_BY_DOXYGEN

#define STORMEIGEN_CURRENT_STORAGE_BASE_CLASS StormEigen::MatrixBase
#   include "../plugins/CommonCwiseUnaryOps.h"
#   include "../plugins/CommonCwiseBinaryOps.h"
#   include "../plugins/MatrixCwiseUnaryOps.h"
#   include "../plugins/MatrixCwiseBinaryOps.h"
#   ifdef STORMEIGEN_MATRIXBASE_PLUGIN
#     include STORMEIGEN_MATRIXBASE_PLUGIN
#   endif
#undef STORMEIGEN_CURRENT_STORAGE_BASE_CLASS

    /** Special case of the template operator=, in order to prevent the compiler
      * from generating a default operator= (issue hit with g++ 4.1)
      */
    STORMEIGEN_DEVICE_FUNC
    Derived& operator=(const MatrixBase& other);

    // We cannot inherit here via Base::operator= since it is causing
    // trouble with MSVC.

    template <typename OtherDerived>
    STORMEIGEN_DEVICE_FUNC
    Derived& operator=(const DenseBase<OtherDerived>& other);

    template <typename OtherDerived>
    STORMEIGEN_DEVICE_FUNC
    Derived& operator=(const EigenBase<OtherDerived>& other);

    template<typename OtherDerived>
    STORMEIGEN_DEVICE_FUNC
    Derived& operator=(const ReturnByValue<OtherDerived>& other);

    template<typename OtherDerived>
    STORMEIGEN_DEVICE_FUNC
    Derived& operator+=(const MatrixBase<OtherDerived>& other);
    template<typename OtherDerived>
    STORMEIGEN_DEVICE_FUNC
    Derived& operator-=(const MatrixBase<OtherDerived>& other);

#ifdef __CUDACC__
    template<typename OtherDerived>
    STORMEIGEN_DEVICE_FUNC
    const Product<Derived,OtherDerived,LazyProduct>
    operator*(const MatrixBase<OtherDerived> &other) const
    { return this->lazyProduct(other); }
#else

    template<typename OtherDerived>
    const Product<Derived,OtherDerived>
    operator*(const MatrixBase<OtherDerived> &other) const;

#endif

    template<typename OtherDerived>
    STORMEIGEN_DEVICE_FUNC 
    const Product<Derived,OtherDerived,LazyProduct>
    lazyProduct(const MatrixBase<OtherDerived> &other) const;

    template<typename OtherDerived>
    Derived& operator*=(const EigenBase<OtherDerived>& other);

    template<typename OtherDerived>
    void applyOnTheLeft(const EigenBase<OtherDerived>& other);

    template<typename OtherDerived>
    void applyOnTheRight(const EigenBase<OtherDerived>& other);

    template<typename DiagonalDerived>
    STORMEIGEN_DEVICE_FUNC
    const Product<Derived, DiagonalDerived, LazyProduct>
    operator*(const DiagonalBase<DiagonalDerived> &diagonal) const;

    template<typename OtherDerived>
    STORMEIGEN_DEVICE_FUNC
    typename internal::scalar_product_traits<typename internal::traits<Derived>::Scalar,typename internal::traits<OtherDerived>::Scalar>::ReturnType
    dot(const MatrixBase<OtherDerived>& other) const;

    STORMEIGEN_DEVICE_FUNC RealScalar squaredNorm() const;
    STORMEIGEN_DEVICE_FUNC RealScalar norm() const;
    RealScalar stableNorm() const;
    RealScalar blueNorm() const;
    RealScalar hypotNorm() const;
    STORMEIGEN_DEVICE_FUNC const PlainObject normalized() const;
    STORMEIGEN_DEVICE_FUNC void normalize();

    STORMEIGEN_DEVICE_FUNC const AdjointReturnType adjoint() const;
    STORMEIGEN_DEVICE_FUNC void adjointInPlace();

    typedef Diagonal<Derived> DiagonalReturnType;
    STORMEIGEN_DEVICE_FUNC
    DiagonalReturnType diagonal();
    
    typedef typename internal::add_const<Diagonal<const Derived> >::type ConstDiagonalReturnType;
    STORMEIGEN_DEVICE_FUNC
    ConstDiagonalReturnType diagonal() const;

    template<int Index> struct DiagonalIndexReturnType { typedef Diagonal<Derived,Index> Type; };
    template<int Index> struct ConstDiagonalIndexReturnType { typedef const Diagonal<const Derived,Index> Type; };

    template<int Index> 
    STORMEIGEN_DEVICE_FUNC
    typename DiagonalIndexReturnType<Index>::Type diagonal();

    template<int Index>
    STORMEIGEN_DEVICE_FUNC
    typename ConstDiagonalIndexReturnType<Index>::Type diagonal() const;
    
    typedef Diagonal<Derived,DynamicIndex> DiagonalDynamicIndexReturnType;
    typedef typename internal::add_const<Diagonal<const Derived,DynamicIndex> >::type ConstDiagonalDynamicIndexReturnType;

    STORMEIGEN_DEVICE_FUNC
    DiagonalDynamicIndexReturnType diagonal(Index index);
    STORMEIGEN_DEVICE_FUNC
    ConstDiagonalDynamicIndexReturnType diagonal(Index index) const;

    template<unsigned int Mode> struct TriangularViewReturnType { typedef TriangularView<Derived, Mode> Type; };
    template<unsigned int Mode> struct ConstTriangularViewReturnType { typedef const TriangularView<const Derived, Mode> Type; };

    template<unsigned int Mode>
    STORMEIGEN_DEVICE_FUNC
    typename TriangularViewReturnType<Mode>::Type triangularView();
    template<unsigned int Mode>
    STORMEIGEN_DEVICE_FUNC
    typename ConstTriangularViewReturnType<Mode>::Type triangularView() const;

    template<unsigned int UpLo> struct SelfAdjointViewReturnType { typedef SelfAdjointView<Derived, UpLo> Type; };
    template<unsigned int UpLo> struct ConstSelfAdjointViewReturnType { typedef const SelfAdjointView<const Derived, UpLo> Type; };

    template<unsigned int UpLo> 
    STORMEIGEN_DEVICE_FUNC
    typename SelfAdjointViewReturnType<UpLo>::Type selfadjointView();
    template<unsigned int UpLo>
    STORMEIGEN_DEVICE_FUNC
    typename ConstSelfAdjointViewReturnType<UpLo>::Type selfadjointView() const;

    const SparseView<Derived> sparseView(const Scalar& m_reference = Scalar(0),
                                         const typename NumTraits<Scalar>::Real& m_epsilon = NumTraits<Scalar>::dummy_precision()) const;
    STORMEIGEN_DEVICE_FUNC static const IdentityReturnType Identity();
    STORMEIGEN_DEVICE_FUNC static const IdentityReturnType Identity(Index rows, Index cols);
    STORMEIGEN_DEVICE_FUNC static const BasisReturnType Unit(Index size, Index i);
    STORMEIGEN_DEVICE_FUNC static const BasisReturnType Unit(Index i);
    STORMEIGEN_DEVICE_FUNC static const BasisReturnType UnitX();
    STORMEIGEN_DEVICE_FUNC static const BasisReturnType UnitY();
    STORMEIGEN_DEVICE_FUNC static const BasisReturnType UnitZ();
    STORMEIGEN_DEVICE_FUNC static const BasisReturnType UnitW();

    STORMEIGEN_DEVICE_FUNC
    const DiagonalWrapper<const Derived> asDiagonal() const;
    const PermutationWrapper<const Derived> asPermutation() const;

    STORMEIGEN_DEVICE_FUNC
    Derived& setIdentity();
    STORMEIGEN_DEVICE_FUNC
    Derived& setIdentity(Index rows, Index cols);

    bool isIdentity(const RealScalar& prec = NumTraits<Scalar>::dummy_precision()) const;
    bool isDiagonal(const RealScalar& prec = NumTraits<Scalar>::dummy_precision()) const;

    bool isUpperTriangular(const RealScalar& prec = NumTraits<Scalar>::dummy_precision()) const;
    bool isLowerTriangular(const RealScalar& prec = NumTraits<Scalar>::dummy_precision()) const;

    template<typename OtherDerived>
    bool isOrthogonal(const MatrixBase<OtherDerived>& other,
                      const RealScalar& prec = NumTraits<Scalar>::dummy_precision()) const;
    bool isUnitary(const RealScalar& prec = NumTraits<Scalar>::dummy_precision()) const;

    /** \returns true if each coefficients of \c *this and \a other are all exactly equal.
      * \warning When using floating point scalar values you probably should rather use a
      *          fuzzy comparison such as isApprox()
      * \sa isApprox(), operator!= */
    template<typename OtherDerived>
    inline bool operator==(const MatrixBase<OtherDerived>& other) const
    { return cwiseEqual(other).all(); }

    /** \returns true if at least one pair of coefficients of \c *this and \a other are not exactly equal to each other.
      * \warning When using floating point scalar values you probably should rather use a
      *          fuzzy comparison such as isApprox()
      * \sa isApprox(), operator== */
    template<typename OtherDerived>
    inline bool operator!=(const MatrixBase<OtherDerived>& other) const
    { return cwiseNotEqual(other).any(); }

    NoAlias<Derived,StormEigen::MatrixBase > noalias();

    // TODO forceAlignedAccess is temporarily disabled
    // Need to find a nicer workaround.
    inline const Derived& forceAlignedAccess() const { return derived(); }
    inline Derived& forceAlignedAccess() { return derived(); }
    template<bool Enable> inline const Derived& forceAlignedAccessIf() const { return derived(); }
    template<bool Enable> inline Derived& forceAlignedAccessIf() { return derived(); }

    STORMEIGEN_DEVICE_FUNC Scalar trace() const;

    template<int p> STORMEIGEN_DEVICE_FUNC RealScalar lpNorm() const;

    STORMEIGEN_DEVICE_FUNC MatrixBase<Derived>& matrix() { return *this; }
    STORMEIGEN_DEVICE_FUNC const MatrixBase<Derived>& matrix() const { return *this; }

    /** \returns an \link StormEigen::ArrayBase Array \endlink expression of this matrix
      * \sa ArrayBase::matrix() */
    STORMEIGEN_DEVICE_FUNC STORMEIGEN_STRONG_INLINE ArrayWrapper<Derived> array() { return ArrayWrapper<Derived>(derived()); }
    /** \returns a const \link StormEigen::ArrayBase Array \endlink expression of this matrix
      * \sa ArrayBase::matrix() */
    STORMEIGEN_DEVICE_FUNC STORMEIGEN_STRONG_INLINE const ArrayWrapper<const Derived> array() const { return ArrayWrapper<const Derived>(derived()); }

/////////// LU module ///////////

    STORMEIGEN_DEVICE_FUNC
    inline const FullPivLU<PlainObject> fullPivLu() const;
    STORMEIGEN_DEVICE_FUNC
    inline const PartialPivLU<PlainObject> partialPivLu() const;

    STORMEIGEN_DEVICE_FUNC
    inline const PartialPivLU<PlainObject> lu() const;

    STORMEIGEN_DEVICE_FUNC
    inline const Inverse<Derived> inverse() const;
    
    template<typename ResultType>
    inline void computeInverseAndDetWithCheck(
      ResultType& inverse,
      typename ResultType::Scalar& determinant,
      bool& invertible,
      const RealScalar& absDeterminantThreshold = NumTraits<Scalar>::dummy_precision()
    ) const;
    template<typename ResultType>
    inline void computeInverseWithCheck(
      ResultType& inverse,
      bool& invertible,
      const RealScalar& absDeterminantThreshold = NumTraits<Scalar>::dummy_precision()
    ) const;
    Scalar determinant() const;

/////////// Cholesky module ///////////

    inline const LLT<PlainObject>  llt() const;
    inline const LDLT<PlainObject> ldlt() const;

/////////// QR module ///////////

    inline const HouseholderQR<PlainObject> householderQr() const;
    inline const ColPivHouseholderQR<PlainObject> colPivHouseholderQr() const;
    inline const FullPivHouseholderQR<PlainObject> fullPivHouseholderQr() const;

/////////// Eigenvalues module ///////////

    inline EigenvaluesReturnType eigenvalues() const;
    inline RealScalar operatorNorm() const;

/////////// SVD module ///////////

    inline JacobiSVD<PlainObject> jacobiSvd(unsigned int computationOptions = 0) const;
    inline BDCSVD<PlainObject>    bdcSvd(unsigned int computationOptions = 0) const;

/////////// Geometry module ///////////

    #ifndef STORMEIGEN_PARSED_BY_DOXYGEN
    /// \internal helper struct to form the return type of the cross product
    template<typename OtherDerived> struct cross_product_return_type {
      typedef typename internal::scalar_product_traits<typename internal::traits<Derived>::Scalar,typename internal::traits<OtherDerived>::Scalar>::ReturnType Scalar;
      typedef Matrix<Scalar,MatrixBase::RowsAtCompileTime,MatrixBase::ColsAtCompileTime> type;
    };
    #endif // STORMEIGEN_PARSED_BY_DOXYGEN
    template<typename OtherDerived>
    STORMEIGEN_DEVICE_FUNC
    inline typename cross_product_return_type<OtherDerived>::type
    cross(const MatrixBase<OtherDerived>& other) const;
    
    template<typename OtherDerived>
    STORMEIGEN_DEVICE_FUNC
    inline PlainObject cross3(const MatrixBase<OtherDerived>& other) const;
    
    STORMEIGEN_DEVICE_FUNC
    inline PlainObject unitOrthogonal(void) const;
    
    inline Matrix<Scalar,3,1> eulerAngles(Index a0, Index a1, Index a2) const;
    
    inline ScalarMultipleReturnType operator*(const UniformScaling<Scalar>& s) const;
    // put this as separate enum value to work around possible GCC 4.3 bug (?)
    enum { HomogeneousReturnTypeDirection = ColsAtCompileTime==1&&RowsAtCompileTime==1 ? ((internal::traits<Derived>::Flags&RowMajorBit)==RowMajorBit ? Horizontal : Vertical)
                                          : ColsAtCompileTime==1 ? Vertical : Horizontal };
    typedef Homogeneous<Derived, HomogeneousReturnTypeDirection> HomogeneousReturnType;
    inline HomogeneousReturnType homogeneous() const;
    
    enum {
      SizeMinusOne = SizeAtCompileTime==Dynamic ? Dynamic : SizeAtCompileTime-1
    };
    typedef Block<const Derived,
                  internal::traits<Derived>::ColsAtCompileTime==1 ? SizeMinusOne : 1,
                  internal::traits<Derived>::ColsAtCompileTime==1 ? 1 : SizeMinusOne> ConstStartMinusOne;
    typedef CwiseUnaryOp<internal::scalar_quotient1_op<typename internal::traits<Derived>::Scalar>,
                const ConstStartMinusOne > HNormalizedReturnType;

    inline const HNormalizedReturnType hnormalized() const;

////////// Householder module ///////////

    void makeHouseholderInPlace(Scalar& tau, RealScalar& beta);
    template<typename EssentialPart>
    void makeHouseholder(EssentialPart& essential,
                         Scalar& tau, RealScalar& beta) const;
    template<typename EssentialPart>
    void applyHouseholderOnTheLeft(const EssentialPart& essential,
                                   const Scalar& tau,
                                   Scalar* workspace);
    template<typename EssentialPart>
    void applyHouseholderOnTheRight(const EssentialPart& essential,
                                    const Scalar& tau,
                                    Scalar* workspace);

///////// Jacobi module /////////

    template<typename OtherScalar>
    void applyOnTheLeft(Index p, Index q, const JacobiRotation<OtherScalar>& j);
    template<typename OtherScalar>
    void applyOnTheRight(Index p, Index q, const JacobiRotation<OtherScalar>& j);

///////// SparseCore module /////////

    template<typename OtherDerived>
    STORMEIGEN_STRONG_INLINE const typename SparseMatrixBase<OtherDerived>::template CwiseProductDenseReturnType<Derived>::Type
    cwiseProduct(const SparseMatrixBase<OtherDerived> &other) const
    {
      return other.cwiseProduct(derived());
    }

///////// MatrixFunctions module /////////

    typedef typename internal::stem_function<Scalar>::type StemFunction;
    const MatrixExponentialReturnValue<Derived> exp() const;
    const MatrixFunctionReturnValue<Derived> matrixFunction(StemFunction f) const;
    const MatrixFunctionReturnValue<Derived> cosh() const;
    const MatrixFunctionReturnValue<Derived> sinh() const;
    const MatrixFunctionReturnValue<Derived> cos() const;
    const MatrixFunctionReturnValue<Derived> sin() const;
    const MatrixSquareRootReturnValue<Derived> sqrt() const;
    const MatrixLogarithmReturnValue<Derived> log() const;
    const MatrixPowerReturnValue<Derived> pow(const RealScalar& p) const;
    const MatrixComplexPowerReturnValue<Derived> pow(const std::complex<RealScalar>& p) const;

  protected:
    STORMEIGEN_DEVICE_FUNC MatrixBase() : Base() {}

  private:
    STORMEIGEN_DEVICE_FUNC explicit MatrixBase(int);
    STORMEIGEN_DEVICE_FUNC MatrixBase(int,int);
    template<typename OtherDerived> STORMEIGEN_DEVICE_FUNC explicit MatrixBase(const MatrixBase<OtherDerived>&);
  protected:
    // mixing arrays and matrices is not legal
    template<typename OtherDerived> Derived& operator+=(const ArrayBase<OtherDerived>& )
    {STORMEIGEN_STATIC_ASSERT(std::ptrdiff_t(sizeof(typename OtherDerived::Scalar))==-1,YOU_CANNOT_MIX_ARRAYS_AND_MATRICES); return *this;}
    // mixing arrays and matrices is not legal
    template<typename OtherDerived> Derived& operator-=(const ArrayBase<OtherDerived>& )
    {STORMEIGEN_STATIC_ASSERT(std::ptrdiff_t(sizeof(typename OtherDerived::Scalar))==-1,YOU_CANNOT_MIX_ARRAYS_AND_MATRICES); return *this;}
};


/***************************************************************************
* Implementation of matrix base methods
***************************************************************************/

/** replaces \c *this by \c *this * \a other.
  *
  * \returns a reference to \c *this
  *
  * Example: \include MatrixBase_applyOnTheRight.cpp
  * Output: \verbinclude MatrixBase_applyOnTheRight.out
  */
template<typename Derived>
template<typename OtherDerived>
inline Derived&
MatrixBase<Derived>::operator*=(const EigenBase<OtherDerived> &other)
{
  other.derived().applyThisOnTheRight(derived());
  return derived();
}

/** replaces \c *this by \c *this * \a other. It is equivalent to MatrixBase::operator*=().
  *
  * Example: \include MatrixBase_applyOnTheRight.cpp
  * Output: \verbinclude MatrixBase_applyOnTheRight.out
  */
template<typename Derived>
template<typename OtherDerived>
inline void MatrixBase<Derived>::applyOnTheRight(const EigenBase<OtherDerived> &other)
{
  other.derived().applyThisOnTheRight(derived());
}

/** replaces \c *this by \a other * \c *this.
  *
  * Example: \include MatrixBase_applyOnTheLeft.cpp
  * Output: \verbinclude MatrixBase_applyOnTheLeft.out
  */
template<typename Derived>
template<typename OtherDerived>
inline void MatrixBase<Derived>::applyOnTheLeft(const EigenBase<OtherDerived> &other)
{
  other.derived().applyThisOnTheLeft(derived());
}

} // end namespace StormEigen

#endif // STORMEIGEN_MATRIXBASE_H
