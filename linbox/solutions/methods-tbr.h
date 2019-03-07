/* linbox/solutions/methods.h
 * Copyright (C) 1999, 2001 Jean-Guillaume Dumas, Bradford Hovinen
 *
 * Written by Jean-Guillaume Dumas <Jean-Guillaume.Dumas@imag.fr>,
 *            Bradford Hovinen <hovinen@cis.udel.edu>
 *
 * ------------------------------------
 * 2003-02-03  Bradford Hovinen  <bghovinen@math.uwaterloo.ca>
 *
 * Reorganization: moved all the method-specific traits to the
 * corresponding structures, out of SolverTraits. Added a class
 * BlockLanczosTraits.
 * ------------------------------------
 * 2002-07-08  Bradford Hovinen  <hovinen@cis.udel.edu>
 *
 * Changed the name _DEFAULT_EarlyTerm_THRESHOLD_ to the more
 * standard-consistent DEFAULT_EARLY_TERM_THRESHOLD; changed the name
 * Early_Term_Threshold to earlyTermThreshold, also in keeping with the
 * standard.
 *
 * Added method traits for elimination and lanczos
 * ------------------------------------
 *
 * ========LICENCE========
 * This file is part of the library LinBox.
 *
 * LinBox is free software: you can redistribute it and/or modify
 * it under the terms of the  GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * ========LICENCE========
 *.
 */

/*! @file solutions/methods.h
 * @ingroup solutions
 * @brief NO DOC
 */

#ifndef __LINBOX_method_H
#define __LINBOX_method_H

#include <string> // size_t

#ifndef DEFAULT_EARLY_TERM_THRESHOLD
#  define DEFAULT_EARLY_TERM_THRESHOLD 20
#endif

#include "linbox/util/mpicpp.h"

#ifndef LINBOX_USE_BLACKBOX_THRESHOLD
#define LINBOX_USE_BLACKBOX_THRESHOLD 1000
#endif

namespace LinBox
{
	///
	// JGD 22.01.2007 : adapted from Lidzhade Fhulu's
	template <typename EnumT, typename BaseEnumT>
	struct InheritEnum {
		InheritEnum() {}
		InheritEnum(EnumT e) :
			enum_(e)
		{}
		InheritEnum(BaseEnumT e) :
		       	baseEnum_(e)
	       	{}
		explicit InheritEnum( int val ) :
		       	enum_(static_cast<EnumT>(val))
	       	{}
		operator EnumT() const { return enum_; }
	private:
		union {
			EnumT enum_;
			BaseEnumT baseEnum_;
		};
	};

	///
	struct Specifier {
		/** Whether the system is known to be singular or nonsingular */
		enum SingularState {
			SINGULARITY_UNKNOWN, SINGULAR, NONSINGULAR
		};

		/** @brief Which preconditioner to use to ensure generic rank profile
		 *
		 * NO_PRECONDITIONER - Do not use any preconditioner
		 * BUTTERFLY - Use a butterfly network, see @ref Butterfly
		 * SPARSE - Use a sparse preconditioner, c.f. (Mulders 2000)
		 * TOEPLITZ - Use a Toeplitz preconditioner, c.f. (Kaltofen and Saunders
		 * 1991)
		 * SYMMETRIZE - Use A^T A (Lanczos only)
		 * PARTIAL_DIAGONAL - Use AD, where D is a random nonsingular diagonal
		 * matrix (Lanczos only)
		 * PARTIAL_DIAGONAL_SYMMETRIZE - Use A^T D A, where D is a random
		 * nonsingular diagonal matrix (Lanczos only)
		 * FULL_DIAGONAL - Use D_1 A^T D_2 A D_1, where D_1 and D_2 are random
		 * nonsingular diagonal matrices (Lanczos only)
		 * DENSE (Dixon use)
		 */

		enum Preconditioner {
			NO_PRECONDITIONER, BUTTERFLY, SPARSE, TOEPLITZ, SYMMETRIZE, PARTIAL_DIAGONAL, PARTIAL_DIAGONAL_SYMMETRIZE, FULL_DIAGONAL, DENSE
		};

		/** Other shapes :
		 *  UNIMOD_UT -- unimodular upper triang. Toeplitz
		 *  UNIMOD_LT -- unimodular lower triang. Toeplitz
		 *  UNIMOD_UH -- unimodular upper triang. Hankel
		 *  UNIMOD_LH -- unimodular lower triang. Hankel
		 **/
		enum BlackboxShape {
			DIAGONAL = 15, HANKEL, UNIMOD_UT, UNIMOD_LT,  UNIMOD_UH,  UNIMOD_LH,  BLKVECTOR, TRI_SUP, TRI_INF
		};

		/** Shape of a Blackbox
		 *  Precontioner shapes and
		 *  other blackbox shape are
		 *  combined
		 **/
		typedef InheritEnum<BlackboxShape, Preconditioner> Shape;

		/** Whether the rank of the system is known (otherwise its value) */
		enum {
			RANK_UNKNOWN = 0
		};

		/** Whether the system is known to be symmetric */
		enum {
			SYMMETRIC = true, NON_SYMMETRIC = false
		};

		/** Whether the probabilistic computation has to be certified Las-Vegas */
		enum {
			CERTIFY = true, DONT_CERTIFY = false
		};

		/** Linear-time pivoting or not for eliminations */
		enum PivotStrategy {
			PIVOT_LINEAR, PIVOT_NONE
		};

		Specifier ( ) :
			_preconditioner(NO_PRECONDITIONER),
			_rank(RANK_UNKNOWN),
			_singular(SINGULARITY_UNKNOWN),
			_symmetric(NON_SYMMETRIC),
			_certificate(CERTIFY),
			_maxTries(1),
			_ett(DEFAULT_EARLY_TERM_THRESHOLD),
			.blockingFactor = 16,
			_strategy(PIVOT_LINEAR),
			_shape(SPARSE),
			_provensuccessprobability( 0.0 )
			, _checkResult( true )
#ifdef __LINBOX_HAVE_MPI
			, _communicatorp( 0 )
#endif
			{}

		Specifier (const Specifier& s):
			_preconditioner( s._preconditioner),
			_rank( s._rank),
			_singular( s._singular),
			_symmetric( s._symmetric),
			_certificate( s._certificate),
			_maxTries( s._maxTries),
			_ett( s._ett),
			_blockingFactor( s._blockingFactor),
			_strategy( s._strategy),
			_shape( s._shape),
			_provensuccessprobability( s._provensuccessprobability)
			, _checkResult( s._checkResult )
#ifdef __LINBOX_HAVE_MPI
			, _communicatorp(s._communicatorp)
#endif
			{}

		/** Accessors
		 *
		 * These functions just return the corresponding parameters from the
		 * structure
		 */

		Preconditioner	preconditioner ()	const { return _preconditioner; }
		size_t		rank ()			const { return _rank; }
		SingularState	singular ()		const { return _singular; }
		bool		symmetric ()		const { return _symmetric; }
		bool		certificate ()		const { return _certificate; }
		size_t	maxTries ()		const { return _maxTries; }
		size_t.earlyTerminationThreshold	const { return _ett; }
		size_t	blockingFactor ()	const { return _blockingFactor; }
		PivotStrategy	strategy ()		const { return _strategy; }
		Shape		shape ()		const { return _shape; }
		double		trustability ()		const { return _provensuccessprobability; }
		bool		checkResult ()		const { return _checkResult; }
#ifdef __LINBOX_HAVE_MPI
		Communicator* communicatorp ()		const { return _communicatorp; }
#endif


		/** @brief Manipulators
		 *
		 * These functions allow on-the-fly modification of a SolverTraits
		 * structure. Note that it is guaranteed that your SolverTraits
		 * structure will not be modified during @ref solve.
		 */

		void preconditioner (Preconditioner p) { _preconditioner = p; }
		void rank           (size_t r)         { _rank = r; }
		void singular       (SingularState s)  { _singular = s; }
		void symmetric      (bool s)           { _symmetric = s; }
		void certificate    (bool s)           { _certificate = s; }
		void maxTries       (size_t n)  { _maxTries = n; }
		void earlyTermThreshold (size_t e) { _ett = e; }
		void blockingFactor (size_t b)  { _blockingFactor = b; }
		void strategy (PivotStrategy Strategy) { _strategy = Strategy; }
		void shape          (Shape s)          { _shape = s; }
		void trustability   (double p)         { _provensuccessprobability = p; }
		void checkResult    (bool s)           { _checkResult = s; }
#ifdef __LINBOX_HAVE_MPI
		void communicatorp  (Communicator* cp) { _communicatorp = cp; }
#endif


	protected:
		Preconditioner _preconditioner;
		size_t         _rank;
		SingularState  _singular;
		bool           _symmetric;
		bool           _certificate;
		size_t  _maxTries;
		size_t  _ett;
		size_t  _blockingFactor;
		PivotStrategy  _strategy;
		Shape          _shape;
		double         _provensuccessprobability;
		bool           _checkResult;
#ifdef __LINBOX_HAVE_MPI
		Communicator*   _communicatorp;
#endif
	};

	/// AutoSpecifier
	struct AutoSpecifier : public Specifier {
		AutoSpecifier(){};
		AutoSpecifier (const Specifier& m) :
		       	Specifier(m)
		{};
#ifdef __LINBOX_HAVE_MPI
		AutoSpecifier (Communicator& C) :
		       	Specifier()
		{ _communicatorp = &C; };
#endif
	};

	/// Method::Blackbox
	struct Method::Blackbox : public Specifier {
		Method::Blackbox(){};
		Method::Blackbox (const Specifier& m) :
		       	Specifier(m)
		{};
	};

	/// EliminationSpecifier
	struct EliminationSpecifier : public Specifier {
		EliminationSpecifier(){};
		EliminationSpecifier (const Specifier& m) :
		       	Specifier(m)
		{};
	};

	/// CRASpecifier
	struct CRASpecifier : public Specifier {
		CRASpecifier () {};
		CRASpecifier (const Specifier &m) :
			Specifier(m)
		{};
	};

	///
	struct Method::Wiedemann : public Specifier {
		/** Constructora.
		 *
		 *  @param symmetric True only if the system is symmetric. This improves
		 * performance somewhat, but will yield incorrect results if the system
		 * is not actually symmetric. Default is false.
		 *
		 * @param thres
		 *
		 * @param rank Rank, if known; otherwise use RANK_UNKNOWN
		 *
		 * @param preconditioner Preconditioner to use, default is sparse
		 *
		 * @param singular Whether the system is known to be singular or
		 * nonsingular; default is UNKNOWN
		 *
		 * @param certificate True if the solver should attempt to find a
		 * certificate of inconsistency if it suspects the system to be
		 * inconsistent; default is true
		 *
		 * @param maxTries Maximum number of trials before giving up and
		 * returning a failure; default is 100
		 *
		 * @param checkResult
		 */
		Method::Wiedemann (
				 bool           Symmetric      = NON_SYMMETRIC,
				 size_t  Thres          = DEFAULT_EARLY_TERM_THRESHOLD,
				 size_t         Rank           = RANK_UNKNOWN,
				 Preconditioner Precond        = SPARSE,
				 SingularState  Singular       = SINGULARITY_UNKNOWN,
				 bool           Certificate    = CERTIFY,
				 size_t  MaxTries       = 100,
				 bool           CheckResult    = true
				)

		{
			Specifier::_preconditioner = Precond;
			Specifier::_rank           = (Rank);
			Specifier::_singular       = (Singular);
			Specifier::_symmetric      = (Symmetric);
			Specifier::_certificate    = (Certificate);
			Specifier::_maxTries       = (MaxTries);
			Specifier::_ett            = (Thres);
			Specifier::_checkResult    = (CheckResult);
		}

		Method::Wiedemann( const Specifier& S) :
		      	Specifier(S)
	       	{}
	};

	///
	struct WiedemannExtensionTraits : public Method::Wiedemann {
		WiedemannExtensionTraits (
					  bool           Symmetric      = NON_SYMMETRIC,
					  size_t  Thres          = DEFAULT_EARLY_TERM_THRESHOLD,
					  size_t         Rank           = RANK_UNKNOWN,
					  Preconditioner Precond        = SPARSE,
					  SingularState  Singular       = SINGULARITY_UNKNOWN,
					  bool           Certificate    = CERTIFY,
					  size_t  MaxTries       = 100,
					  bool           CheckResult    = true
					 ) :
			Method::Wiedemann(Symmetric,Thres,Rank,Precond,Singular,
					Certificate,MaxTries,CheckResult)
		{}
		WiedemannExtensionTraits( const Specifier& S) :
			Method::Wiedemann(S)
		{}
	};

	///
	struct LanczosTraits : public Specifier {
		/** Constructor.
		 *
		 * @param preconditioner Preconditioner to use, default is sparse
		 * @param maxTries Maximum number of trials before giving up and
		 * returning a failure; default is 100
		 */
		LanczosTraits (Preconditioner Precond  = FULL_DIAGONAL,
			       size_t  MaxTries = 100)
		{
			Specifier::_preconditioner =(Precond);
			Specifier::_maxTries =(MaxTries);
		}
		LanczosTraits( const Specifier& S) :
		      	Specifier(S)
	       	{}
	};

	///
	struct BlockLanczosTraits : public Specifier {
		/** Constructor.
		 *
		 * @param preconditioner Preconditioner to use, default is sparse
		 * @param maxTries Maximum number of trials before giving up and
		 * returning a failure; default is 100
		 * @param blockingFactor Blocking factor to use
		 */
		BlockLanczosTraits (Preconditioner Precond        = FULL_DIAGONAL,
				    size_t  MaxTries       = 100,
				    int            BlockingFactor = 16)
		{
			Specifier::_preconditioner = (Precond);
			Specifier::_maxTries       = (MaxTries);
			Specifier::_blockingFactor = (size_t) (BlockingFactor);
		}

		BlockLanczosTraits( const Specifier& S) :
		      	Specifier(S)
	       	{}
	};

	///
	struct Method::SparseElimination  : public Specifier {
		/** Constructor.
		 *
		 * @param strategy Pivoting strategy to use
		 */
		Method::SparseElimination (PivotStrategy Strategy = PIVOT_LINEAR)
		{
			Specifier::_strategy = (Strategy) ;
		}
		Method::SparseElimination( const EliminationSpecifier& S) :
		      	Specifier(S)
	       	{}
	};

	///
	struct Method::Dixon : public Specifier {

		enum SolutionType {
			DETERMINIST, RANDOM, DIOPHANTINE
		};

		Method::Dixon ( SolutionType   Solution       = DETERMINIST,
			      SingularState  Singular       = SINGULARITY_UNKNOWN,
			      bool           Certificate    = DONT_CERTIFY,
			      int            MaxTries       = 10,
			      Preconditioner Precond        = DENSE,
			      size_t         Rank           = RANK_UNKNOWN)
		{
			_solution                  = (Solution);
			Specifier::_singular       = (Singular);
			Specifier::_certificate    = (Certificate);
			Specifier::_maxTries       = (size_t) (MaxTries);
			Specifier::_preconditioner = (Precond);
			Specifier::_rank           = (Rank);
		}

		Method::Dixon( const Specifier& S) :
		       	Specifier(S)
		{
			_solution= RANDOM;
		}

		SolutionType solution () const { return _solution;}

		void solution (SolutionType s) { _solution= (s);}

	protected:
		SolutionType _solution;
	};


	/// To select algorithms that use Giorgi' algorithms/block-massey-domain.h
	struct Method::BlockWiedemann : public Specifier {
		Method::BlockWiedemann ( Preconditioner Precond= NO_PRECONDITIONER,
				       size_t         Rank   = RANK_UNKNOWN)
		{
			Specifier::_preconditioner = Precond;
			Specifier::_rank           = Rank;
		}
		Method::BlockWiedemann( const Specifier& S) :
		       	Specifier(S)
		{}
	};

	/// To select algorithms that use Yuhasz' algorithms/coppersmith.h
	struct CoppersmithTraits : public Specifier {
		CoppersmithTraits ( Preconditioner Precond= NO_PRECONDITIONER,
				       size_t         Rank   = RANK_UNKNOWN)
		{
			Specifier::_preconditioner = Precond;
			Specifier::_rank           = Rank;
		}
		CoppersmithTraits( const Specifier& S) :
		       	Specifier(S)
		{}
	};


	//Using Wan's numeric/symbolic method to solve linear systems.
	//based on a preprinted article, submitted to JSC 2004
	struct Method::NumericSymbolicNorm : public Specifier
	{
		Method::NumericSymbolicNorm ( Preconditioner Precond= NO_PRECONDITIONER,
			    size_t         Rank   = RANK_UNKNOWN)
		{
			Specifier::_preconditioner = (Precond);
			Specifier::_rank           = (Rank) ;
		}
		Method::NumericSymbolicNorm( const Specifier& S) :
		       	Specifier(S)
	       	{}
	};

	//Using Youse et al numerical/symbolic method to symbolically solve linear systems.
	// This is like Wan's but replaces a norm condition with an overlap test
	// to confirm valid numeric iteration steps.
	struct Method::NumericSymbolicOverlap : public Specifier
	{
		Method::NumericSymbolicOverlap ( Preconditioner Precond= NO_PRECONDITIONER,
				  size_t         Rank   = RANK_UNKNOWN)
		{
			Specifier::_preconditioner = (Precond);
			Specifier::_rank           = (Rank) ;
		}
		Method::NumericSymbolicOverlap( const Specifier& S) :
		       	Specifier(S)
	       	{}
	};

	//Use a numerical/symbolic method if it works.  If it fails, go to a dixon style method.
	struct AdaptiveSolverTraits : public Specifier
	{
		AdaptiveSolverTraits ( Preconditioner Precond= NO_PRECONDITIONER,
				  size_t         Rank   = RANK_UNKNOWN)
		{
			Specifier::_preconditioner = (Precond);
			Specifier::_rank           = (Rank) ;
		}
		AdaptiveSolverTraits ( const Specifier& S) :
		       	Specifier(S)
	       	{}
	};

	///
	struct BlockHankelTraits : public Specifier {
		BlockHankelTraits ( Preconditioner Precond= NO_PRECONDITIONER,
				    size_t         Rank   = RANK_UNKNOWN)
		{
			Specifier::_preconditioner = Precond;
			Specifier::_rank           = Rank;
		}
		BlockHankelTraits( const Specifier& S) :
		       	Specifier(S)
	       	{}
	};

	///
	struct DenseEliminationTraits : public Specifier {
		DenseEliminationTraits() {}
		DenseEliminationTraits( const Specifier& S) :
		       	Specifier(S)
	       	{}

	};

	struct IMLNonSing {} ;
	struct IMLCertSolv {} ;
	/*! IML wrapper.
	 * IML proposes 2 system solving kinds:
	 *  - (1) non singular where one can solve AX=B or XA=B for B a set of
	 *  vectors and A non singular.
	 *  - (2) certified system solving where we get a certificate for the
	 *  rectangular cases : no solution, the smallest one
	 *  .
	 *
	 * @todo enable multi-vectors.
	 * @todo enable right/left solving.
	 * @todo be input aware (long/Integer)
	 */
	struct IMLTraits : public Specifier {
		bool _computeRNS ;
		bool _reduce ;
		unsigned int _nullcol ;
		int _imlroutine;
		/*! Constructor.
		 *
		 * @param imlroutine \c 1 -> non singular ; \c 2 -> certified
		 * @param withRNS  computre RNS
		 * @todo make the special flags available in \c Specifier.
		 */
		IMLTraits ( const IMLNonSing & imlroutine, // routine 1
			    bool withRNS = false) :
			_computeRNS(withRNS),
			_reduce(false),_nullcol(10),
			_imlroutine(1)
		{
			singular(NONSINGULAR);
		}

		/*! Constructor.
		 *
		 * @param imlroutine2  \c 2 -> certified
		 * @param certify
		 * @param reduce reduce the result ?
		 * @param nullcolred look at IML doc.
		 */

		IMLTraits ( const IMLCertSolv & imlroutine2, // routine 2
			    bool certify            = DONT_CERTIFY,
			    bool reduce             = false ,
			    unsigned int nullcolred = 10  /* IML default */) :
			_computeRNS(false),
			_reduce(reduce),
			_nullcol(nullcolred),
			_imlroutine(2)
		{
			certificate(certify);
		}


		//!@bug not complete
		IMLTraits ( const Specifier & S) :
			Specifier(S)
		{}

		int routine() const { return _imlroutine;  }
		bool reduced () const { return _reduce ; }
		bool computeRNS() const { return _computeRNS ;  }
		unsigned int nullcol() const { return _nullcol ; }
		} ;

	struct CRATraits ;

    template <class IterationMethod, class Dispatch>
    struct CRATraitsWIP ;

    /// Method specifiers for controlling algorithm choice
    struct Method {
        typedef AutoSpecifier               Auto;                       //!< Method::Auto : no doc
        typedef Method::Blackbox           Blackbox;                   //!< Method::Blackbox : no doc
        typedef EliminationSpecifier        Elimination;                //!< Method::Elimination : no doc
        typedef CRATraits                   CRA;                        //!< Use CRA for solving Integer systems.
        typedef Method::Wiedemann             Wiedemann;                  //!< Method::Wiedemann : no doc
        typedef WiedemannExtensionTraits    WiedemannExtension;         //!< Method::WiedemannExtension :  no doc
        typedef Method::BlockWiedemann        BlockWiedemann;             //!< Method::BlockWiedemann : no doc
        typedef CoppersmithTraits           Coppersmith;                //!< Method::Coppersmith : no doc
        typedef LanczosTraits               Lanczos;                    //!< Method::Lanczos : no doc.
        typedef BlockLanczosTraits          BlockLanczos;               //!< Method::BlockLanczos : no doc.
        typedef Method::SparseElimination     SparseElimination;          //!< Method::SparseElimination : no doc
        typedef Method::NumericSymbolicOverlap         NumSymOverlap;              //!< Method::NumericSymbolicOverlap : Use Youse's overlap-based numeric/symbolic iteration for Rational solving of dense integer systems
        typedef Method::NumericSymbolicNorm            NumSymNorm;                 //!< Method::NumericSymbolicNorm : Use Wan's (older) norm-based numeric/symbolic iteration for Rational solving of dense integer systems
        typedef AdaptiveSolverTraits        Adaptive;                   //!< Method::Adaptive: Use NumSymOverlap if it works.  If it fails, switch to IML probably.
        typedef DenseEliminationTraits      DenseElimination;           //!< Method::DenseElimination : no doc
        typedef Method::Dixon                 Dixon;                      //!< Method::Dixon : no doc
        typedef BlockHankelTraits           BlockHankel;                //!< Method::BlockHankel : no doc
        typedef IMLTraits                   IML;                        //!< Use IML for solving Dense Integer systems.
        Method() {}
    };

	/**
     * Used for solve using CRA.
     *
     * Iterations for solve uses IterationMethod.
     * The dispatch decides how the computation will be shared
     * between nodes or such.
     */
	template <class IterationMethod, class DispatchType>
    struct CRATraitsWIP {
        IterationMethod iterationMethod;
        DispatchType dispatch;
	};

	/// Solve using CRA (iterations uses SolveMethod)
	struct CRATraits {
	protected:
		Specifier& _solveMethod; //!< Method used to solve sub-computations.

	public:
		CRATraits( Specifier & m) :
			_solveMethod(m) {}

		Specifier & iterationMethod() const {
			return _solveMethod;
		}
	};


	template<class BB>
	bool useBB(const BB& A)
	{
		return (A.coldim() > LINBOX_USE_BLACKBOX_THRESHOLD) && (A.rowdim() > LINBOX_USE_BLACKBOX_THRESHOLD);
	}


	template<class _Field, class _Rep>
	class BlasMatrix ; // forward declaration...

	template<class Field, class _Rep>
	bool useBB(const BlasMatrix<Field,_Rep>& A) { return false; }


	/** Solver traits.
	 *
	 * User-specified parameters for solving a linear system.
	 */
	struct SolverTraits : public Specifier {
		/** Constructor.
		 *
		 * @param CheckResult True if and only if the solution should be checked
		 * for correctness after it is computed (very much recommended for the
		 * randomized algorithms Wiedemann and Lanczos); default is true
		 */
		SolverTraits (bool CheckResult = true)
		{
			Specifier::_checkResult = CheckResult;
		}

		/** Constructor from a MethodTraits structure
		 *
		 * @param S MethodTraits structure from which to get defaults
		 */
		SolverTraits( const Specifier& S) :
		       	Specifier(S)
	       	{}
	};

	/** Exception thrown when the computed solution vector is not a true
	 * solution to the system, but none of the problems cited below exist
	 */
	class LinboxError {};

	/** Exception thrown when the system to be solved is
	 * inconsistent. Contains a certificate of inconsistency.
	 */
	template <class Vector>
	class InconsistentSystem {
	public:
		InconsistentSystem (Vector &u) :
		       	_u (u)
		{}

		const Vector &certificate () const { return _u; }

	private:

		Vector _u;
	};

}

#endif // __LINBOX_method_H

// Local Variables:
// mode: C++
// tab-width: 4
// indent-tabs-mode: nil
// c-basic-offset: 4
// End:
// vim:sts=4:sw=4:ts=4:et:sr:cino=>s,f0,{0,g0,(0,\:0,t0,+0,=s