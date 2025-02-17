/**
 * Besm ALLAH
 * use << operator for matrix move or = for matrix copy
 */

#ifndef __NSRMATHLIB_H__
#define __NSRMATHLIB_H__

#include <math.h>
#include <stdarg.h>     /* va_list, va_start, va_arg, va_end */
#include <stddef.h> //NULL
#include <iostream>
#include <stdio.h>
#include "nsrCore.h" //for LOG

#ifdef __cplusplus
extern "C" {
#endif

extern int allocated_size; //check for memory leak

//for speed, comment MATLAB_ARRAYS_ENABLED
//this enables the use of matlab syntax (row, col start from 1) when using overloaded A(), A(,)
#define MATLAB_ARRAYS_ENABLED

#define STORE_COLWISE /* Needed for matlab-fortran interaction */
//#define STORE_ROWWISE

//#define Tfloat double
typedef double Tfloat;
#define SWAP(a,b) {d=a;a=b;b=d;}
/** @brief    structure produce some of useful variables. We must set up these variables for each matrix.
 */
struct Matheader {
	/** The pointer "raw_pointer" contains value of main matrix elements */
	Tfloat *raw_pointer;
	/** Shows if raw_pointer is made with malloc */
	char raw_pointer_made_with_malloc;
	/** The pointer "start_pointers" contains value of matrix first rows */
	Tfloat **start_pointers;
	/** Shows if start_pointers is made with malloc */
	char start_pointers_made_with_malloc;

	/** The variable "memory_size" contains number of elements of a matrix */
	int memory_size;
	/** The variable "max_starters" contains max number of rows/col starts
	  * of a matrix(depending on rowwise or colwise storage structure ) */
	int max_starters;

	/** The variable "r" contains number of rows of a matrix */
	int r;
	/** The variable "c" contains number of columns of a matrix */
	int c;

#ifdef MATLAB_ARRAYS_ENABLED
	/** If enabled, overloaded A(), A(,) are modified to comply with matlab syntax(rows, cols start from 1) */
	bool matrix_indices_start_by_one;
#endif

	/** We define the boolean variable "autogenerated" . When we want choose between copy or move */
	bool autoGenerated;

	/** The  variable "error" is set up by "-1" in function "inverse" when matrix don't have inverse*/
	int error;

	/** Disable disinit once so that a local matrix created and returned from a function survives */
	int disable_disinit_once;
};

class Matrice
{
public:

	//encapsulating all data in this pointer makes (shallow)object copy with minimum overhead
	Matheader * p;

	////////////////////////////
	//Allocations and disallocations
protected: //accessible by subclasses but not user as every call to initialize() is redundant and causes memory leak
	/** @brief
	* Should be called just once in the lifetime of matrix(in constructors)
	* WARNING: USING THIS FUNCTION TWICE WITHOUT USING disinit() IN BETWEEN WILL CAUSE MEMORY LEAK
	* @param raw_pointer: pointer to matrix start
	* @param start_pointers: array of matrix rows start*/
	void initialize(int init_rows, int init_cols, Tfloat* raw_pointer = NULL, Tfloat** start_pointers = NULL, bool matrix_indices_start_by_one = false);
public:
	/** @brief
	* Function "disinit" delete a matrix and all variables which are defined by structure "Matheader" completely.
	*/
	void disinit();

	/** @brief
	* Can be called after initialize to resize matrix, can be called multiple times
	* @param rows_: new row size
	* @param cols_: new column size */
	void reinit(int rows_, int cols_);

	////////////////////////////
	//Constructors and destructors

	//globally defined classes' constructor are not called in micro, call separately
	Matrice(bool matrix_indices_start_by_one = false) { initialize(0, 0, NULL, NULL, matrix_indices_start_by_one); }

	//raw_pointer: pointer to matrix start
	//start_pointers: array of matrix rows start
	Matrice(int init_rows, int init_cols, Tfloat* raw_pointer = NULL, Tfloat** start_pointers = NULL, bool matrix_indices_start_by_one = false)
	{
		initialize(init_rows, init_cols, raw_pointer, start_pointers, matrix_indices_start_by_one);
	}

	Matrice(const char* flag, int dimension);

	~Matrice();

	////////////////////////////
	//Copies and moves//////////

	//copy constructor
	//Is called when:
	//	1) Matrice a = b; //equal(=) operator is used at the time of Matrice declaration
	//	2) Matrice a(b);
	//MOVE CONSTRUCTOR NOT USED, USE (Matrice &a = b) FOR DECLERATIVE COPY BY REFERENCE
	Matrice(const Matrice &M);

	/** @brief
	*  Matrix equalization
	* Overloading the operator "=" so that matrix copy takes effect
	* Note: Copy-Move constructor is called instead when a Matrice is being defined(like: Matrice a = b;)
	* @note Pay attention to difference between copy matrix or move matrix.
	* @param M: Matrix which is moved or copied.
	* @return returns a matrix just for a=b=c syntax */
	Matrice &operator = (const Matrice &M);

	//shallow copy, use for reference assignments: a << b + c, a << b.diag(), ...
	friend void operator << (Matrice &output, const Matrice &M);

	////////////////////////////
	//Element actions///////////

	/** @brief
	*  A function that gives us number of rows of matrix.
	* @param without any variable as input.
	* @return Number of rows of matrix.*/
	int rows() const;

	/** @brief
	*  A function that gives us number of columns of matrix.
	* @param without any variable as input.
	* @return Number of columns of matrix.*/
	int cols() const;

	/** @brief
	 *  It is an inline function which gives us the value of a Specified element of a matrix.
	 * @param row: Number of rows of matrix
	 * @param col: Number of columns of matrix
	 * @return The value of a Specified element of a matrix.*/
	///TODO: replace with macro for 2x speed
	/*	inline Tfloat &elem(int row, int col)
		{
	#ifdef STORE_ROWWISE
			return p->start_pointers[row][col];
	#else //STORE_COLWISE
			return p->start_pointers[col][row];
	#endif
		}*/

#ifdef STORE_ROWWISE
#define elem(row, col) p->start_pointers[row][col]
#else //STORE_COLWISE
#define elem(row, col) p->start_pointers[col][row]
#endif

	/** @brief
	*  It is an inline function which gives us the value of a Specified element of a matrix.
	* @param row: Number of rows of matrix
	* @param col: Number of columns of matrix
	* @return The value of a Specified element of a matrix.*/
	/*inline Tfloat elemc(int row, int col) const
	{
	#ifdef STORE_ROWWISE
		return p->start_pointers[row][col];
	#else //STORE_COLWISE
		return p->start_pointers[col][row];
	#endif
	}*/

#ifdef STORE_ROWWISE
#define elemc(row, col) p->start_pointers[row][col]
#else //STORE_COLWISE
#define elemc(row, col) p->start_pointers[col][row]
#endif

	/** @brief
	*  Overloading operator "()" so that gives us the value of a Specified element of a matrix .
	* @param m: Number of row of matrix
	* @param n: Number of column of matrix
	* @return The value of a Specified element of a matrix which is located in a Specified row and column.*/
	inline Tfloat &operator()(int m, int n)
	{
#ifdef MATLAB_ARRAYS_ENABLED
		if(p->matrix_indices_start_by_one) {
			m--; n--;
		}
#endif

		return elem(m, n);
	}

	/** @brief
	*  If the matrix has one row or one column the operator "()"  gives us the value of a Specified element of a matrix .
	* @param m: Number of element of a matrix
	* @return The value of a Specified element of a matrix*/
	inline Tfloat &operator()(int m)
	{
#ifdef MATLAB_ARRAYS_ENABLED
		if(p->matrix_indices_start_by_one)
			m--;
#endif

		if(p->c == 1)
			return elem(m, 0);
		else if(p->r == 1)
			return elem(0, m);
		
        LOGE("matlib:", "not one dimentional!\n");
        return elem(m, 0);
	}

	/** @brief
	*  If the matrix has one row or one column the operator "[]"  gives us the value of a Specified element of a matrix .
	* @param m: Number of element of a matrix
	* @return The value of a Specified element of a matrix*/
	inline Tfloat &operator [](int m)
	{
		if(p->c == 1)
			return elem(m, 0);
		else if(p->r == 1)
			return elem(0, m);
		
        LOGE("matlib:", "not one dimentional!\n");
        return elem(m, 0);
	}

	//Not works in row matrices for better berformance
	inline Tfloat &x() {return elem(0, 0);}
	inline Tfloat &y() {return elem(1, 0);}
	inline Tfloat &z() {return elem(2, 0);}
	inline Tfloat &w() {return elem(3, 0);}

	inline Tfloat &r() {return elem(0, 0);}
	inline Tfloat &g() {return elem(1, 0);}
	inline Tfloat &b() {return elem(2, 0);}
	inline Tfloat &a() {return elem(3, 0);}
	inline Tfloat &d() {return elem(3, 0);}

	inline Tfloat &e1() {return elem(0, 0);}
	inline Tfloat &e2() {return elem(1, 0);}
	inline Tfloat &e3() {return elem(2, 0);}
	inline Tfloat &e4() {return elem(3, 0);}
	inline Tfloat &et() {return elem(3, 0);}
	inline Tfloat &e0() {return elem(3, 0);}

	inline Tfloat x() const {return elemc(0, 0);}
	inline Tfloat y() const {return elemc(1, 0);}
	inline Tfloat z() const {return elemc(2, 0);}
	inline Tfloat w() const {return elemc(3, 0);}

	inline Tfloat r() const {return elemc(0, 0);}
	inline Tfloat g() const {return elemc(1, 0);}
	inline Tfloat b() const {return elemc(2, 0);}
	inline Tfloat a() const {return elemc(3, 0);}
	inline Tfloat d() const {return elemc(3, 0);}

	inline Tfloat e1() const {return elemc(0, 0);}
	inline Tfloat e2() const {return elemc(1, 0);}
	inline Tfloat e3() const {return elemc(2, 0);}
	inline Tfloat e4() const {return elemc(3, 0);}
	inline Tfloat et() const {return elemc(3, 0);}
	inline Tfloat e0() const {return elemc(3, 0);}

	////////////////////////////
	//Visualization/////////////

	/** @brief
	* (~armadillo syntax)
	* This function prints the matrix.
	* @param name : prints name before print */
	void print(const char* name = NULL, int group_size = 0);

#if defined(_WIN32) || defined(__linux__) || defined(__ANDROID__)
	/** @brief
	* (~armadillo syntax)
	* This function saves the matrix to a file
	* @param addr : address of the file */
	void save(const char* filepath);

	/** @brief
	* (~armadillo syntax)
	* This function loads the matrix from a file
	* @param addr : address of the file */
	void load(const char* filepath);
#endif
	////////////////////////////
	//Transposing///////////////

	/** @brief
	* (armadillo syntax)
	*  Gives transpose of a matrix
	* @return matrix transpose */
	Matrice t();

	//(eigen syntax)
	Matrice transpose();

	////////////////////////////
	//Get subsets of matrix/////

	/** @brief
	*  A function that gives a copy of a column of the matrix.
	* @param c: matrix column, starting by one if p->matrix_indices_start_by_one is set
	* @return output copy matrix */
	Matrice col(int c);

	/** @brief
	*  A function that gives an unsafe reference of a column of the matrix.
	* @param c: matrix column, starting by one if p->matrix_indices_start_by_one is set
	*  To work with the reference, store it by a shallow copy (e.g. "a << b.colr(1);")
	*  WARNING: USING THIS FUNCTION WITHOUT STORING RETURN MAT BY "<<" OPERATOR WILL CAUSE MEMORY LEAK
	* @return output ref matrix */
	Matrice colr(int c);

	/** @brief
	*  A function that gives a copy of a row of the matrix.
	* @param c: matrix row, starting by one if p->matrix_indices_start_by_one is set
	* @return output copy matrix */
	Matrice row(int c);

	/** @brief
	*  A function that gives an unsafe reference of a row of the matrix.
	* @param c: matrix row, starting by one if p->matrix_indices_start_by_one is set
	*  To work with the reference, store it by a shallow copy (e.g. "a << b.colr(1);")
	*  WARNING: USING THIS FUNCTION WITHOUT STORING RETURN MAT BY "<<" OPERATOR WILL CAUSE MEMORY LEAK
	* @return output ref matrix */
	Matrice rowr(int c);

	/** @brief
	*  A function that gives a copy of a subset of the matrix.
	* @param min_row: starting by one if p->matrix_indices_start_by_one is set
	* @param max_row: starting by one if p->matrix_indices_start_by_one is set
	* @param min_col: starting by one if p->matrix_indices_start_by_one is set
	* @param max_col: starting by one if p->matrix_indices_start_by_one is set
	* @return output copy matrix */
	Matrice subs(int min_row, int max_row, int min_col, int max_col);

	/** @brief
	*  A function that gives an unsafe reference of a subset of the matrix.
	* @param min_row: starting by one if p->matrix_indices_start_by_one is set
	* @param max_row: starting by one if p->matrix_indices_start_by_one is set
	* @param min_col: starting by one if p->matrix_indices_start_by_one is set
	* @param max_col: starting by one if p->matrix_indices_start_by_one is set
	*  To work with the reference, store it by a shallow copy (e.g. "a << b.subsr(1,2,3,4);")
	*  WARNING: USING THIS FUNCTION WITHOUT STORING RETURN MAT BY "<<" OPERATOR WILL CAUSE MEMORY LEAK
	* @return output ref matrix */
	Matrice subsr(int min_row, int max_row, int min_col, int max_col);

	/** @brief
	*  A function that gives a copy of the diagonal of the matrix.
	* @return output copy matrix */
	Matrice diag();

	/** @brief
	*  A function that gives an unsafe reference of the diagonal of the matrix.
	*  To work with the reference, store it by a shallow copy (e.g. "a << b.diagr();")
	*  WARNING: USING THIS FUNCTION WITHOUT STORING RETURN MAT BY "<<" OPERATOR WILL CAUSE MEMORY LEAK
	* @return output ref matrix */
	Matrice diagr();

	///////////////////////////
	//Various fillings of matrix

	/** @brief
	* (armadillo syntax)
	* Fills all of elements of a matrix by a constant value.
	* @param val: The constant value.
	* @return It is void so does not return anything */
	void fill(Tfloat val);

	/** @brief
	*  Fills all of elements of a matrix by a desired value.
	* WARNING: DON'T FORGET TO PUT FLOATING DOT AFTER INPUT NUMBERS
	* @param first,...: values which matrix is filled by them.
	* @return It is void so does not return anything */
	void fill2(Tfloat first, ...);

	/** @brief
	*  This function fills all of elements of a matrix by another matrix.
	* this(dst_min_row:dst_max_row, dst_min_col:dst_max_col) =
	*			A(src_min_row:src_max_row, src_min_col:src_max_col)
	* @param first,...: values which matrix is filled by them.
	* @return It is void so does not return anything */
	void fill3(int dst_min_row, int dst_max_row, int dst_min_col, int dst_max_col,
			   Matrice &A, int src_min_row, int src_max_row, int src_min_col, int src_max_col);

	/** @brief
	*  This function fills all of elements of a matrix by a random value.
	* @param min: The minimum of random values.
	* @param max: The maximum of random values.
	* @return It is void so does not return anything */
	void fillRand(Tfloat min, Tfloat max);

	/** @brief
	*  This function fills matrix with eye mat
	*/
	void fillEye();

	////////////////////////////
	//Old API///////////////////

	/** @brief
	*  deprecated, B.equal(A) equals B = A
	*  This function equalize two matrix.
	* @param A: matrix with we want to equal it to another
	*/
	void equal(const Matrice &A);

	/** @brief
	* This function equalize two matrices with modifications
	*  not deprecated as B.equal3(A, '-') is still faster than B = -A(eliminating a matrix copy)
	* @param A: Matrix with we want to copy its values into another matrix
	* @param oper: '\'' for transpose, '-' for minus
	* @return this function return "0" to show us it is work correctly or not */
	int equal2(const Matrice &A, char oper); //

	/** @brief
	*  This function do an algebraic calculations of matrix .
	*  not deprecated as C.equal3(A, '+', B) is still faster than C = A + B (eliminating a matrix copy)
	* @param A: A is a matrix type input.
	* @param B: B is a matrix type input.
	* @param oper: '+' and '-' for matrix addition and substraction, '*' for matrix multiplication and dot product, 'x' for cross product
	*/
	void equal3(Matrice &A, char oper, Matrice &B); //

	/** @brief
	*  A function that gives us value of diagonal elements of matrix A.
	* @param A: A is a matrix which we want to get its diagonal element's values.
	* @return It is void so does not return anything  .*/
	//output is a column vector
	void equaldiag(const Matrice &A);

	/** @brief
	*  A function that multiples each value of diagonal elements to a specified number.
	* @param scale: The number which we want to multiply it to each value of diagonal elements.
	* @return It is void so does not return anything  .*/
	void scaleDiag(Tfloat scale);

	/** @brief
	*  A function that gives us one corresponding row of matrix A.
	* @param A: A is a matrix which we want to get one of its rows.
	* @param row: The number of the row which we want to get it.
	* @return It is void so does not return anything*/
	//output is a column vector
	void equalrow(const Matrice &A, int row);

	/** @brief
	*  A function that gives us one corresponding column of matrix A.
	* @param A: A is a matrix which we want to get one of its columns.
	* @param col: The number of the column which we want to get it.
	* @return It is void so does not return anything.*/
	//output is a column vector
	void equalcol(const Matrice &A, int col);

	/** @brief
	*  A function that equals a corresponding row of a matrix to the first row of matrix A.
	* @param A: A is a matrix which we want to equal corresponding row of a matrix to its first row .
	* @param row: The number of the row which we want to equal it to the first row of matrix A.
	* @return It is void so did not return anything.*/
	//input is a column vector
	void rowequal(int row, const Matrice &A);

	/** @brief
	*  A function that equals a corresponding column of a matrix to the first column of matrix A.
	* @param A: A is a matrix which we want to equal corresponding column of a matrix to its first column .
	* @param col: The number of the column which we want to equal it to the first column of matrix A.
	* @return It is void so did not return anything.*/
	//input is a column vector
	void colequal(int col, const Matrice &A);

	/** @brief
	*  A function that calculate mean of a specified row of a matrix.
	* @param row: The number of the row which we want to calculate mean of its values.
	* @return It returns mean of a specified row as a number of type Tfloat.*/
	Tfloat rowMean(int row);

	/** @brief
	*  A function that calculate mean of a specified column of a matrix.
	* @param clo: The number of the column which we want to calculate mean of its values.
	* @return It returns mean of a specified column as a number of type Tfloat.*/
	Tfloat colMean(int col);

	////////////////////////////
	//New API///////////////////

	/** @brief
	*  Scalar equalization
	* Overload the operator "=" so that equals each element of a matrix to a constant value.
	* @param d: Each element of the matrix back of sign "=" equals to the constant value "d"  .
	* @return  returns a matrix just for a=b=2 syntax */
	Matrice &operator = (const Tfloat d);

	/** @brief
	* (common syntax)
	* Matrix addition(e.g.: A+B)
	*  Overload the operator "+" so that adds each element of a matrix with the corresponding element of another matrix.
	* @param M: Matrix which we sum it with the matrix back of sign "+".
	* @return  returns a matrix that is sum of tow matrix */
	Matrice operator + (const Matrice &M);

	/** @brief
	* (common syntax)
	* Scalar addition(e.g.: A+1.)
	* Overload the operator "+" so that sums each element of a matrix with a constant value.
	* @param rval: The constant value which sums with each element of the matrix back of sign "+"  .
	* @return  returns a matrix that is sum of a matrix and a constant value */
	Matrice operator + (const Tfloat rval);

	Matrice &operator+=(Matrice &rhs);

	Matrice &operator += (const Tfloat rhs);

	/** @brief
	* (common syntax)
	* Matrix subtraction(e.g.: A-B)
	* Overload the operator "-" so that subtracts each element of a matrix from the corresponding element of another matrix.
	* @param M: Matrix which we Subtract it from the matrix back of sign "-".
	* @return  returns a matrix that is subtracted of tow matrix */
	Matrice operator - (const Matrice &M);

	/** @brief
	* (common syntax)
	* Scalar subtraction(e.g.: A-1.)
	* Overload the operator "-" so that subtracts each element of a matrix from a constant value.
	* @param rval: The constant value which Subtract from each element of the matrix back of sign "-"  .
	* @return  returns a matrix that is subtracted of a matrix and a constant value */
	Matrice operator - (const Tfloat rval);

	Matrice &operator-=(Matrice &rhs);

	Matrice &operator -= (const Tfloat rhs);

	/** @brief
	* (common syntax)
	* Matrix multiplication(e.g.: A*B)
	* Overload the operator "*" so that multiples two matrices.
	* @param M: Matrix which we multiples it by the matrix back of sign "*".
	* @return  returns a matrix that is equal to two matrices multiplication */
	Matrice operator * (const Matrice &M);

	/** @brief
	* (common syntax)
	* Scalar multiplication(e.g.: A*2.)
	* Overload the operator "*" so that multiples each element of a matrix by a constant value.
	* @param d: The constant value which multiples by each element of the matrix back of sign "*"  .
	* @return  returns a matrix that is equal to multiplication of a matrix and a constant value */
	Matrice operator * (Tfloat d);

	/** @brief
	* (common syntax)
	* Unary multiply by scalar(e.g.: A*=2.)
	* Overload the operator "*=" so that multiples each element of a matrix by a constant value.
	* @param rhs: The constant value which multiples by each element of the matrix.
	* @return result */
	Matrice &operator *= (Tfloat rhs);

	/** @brief
	*  Elementwise Matrix2Matrix Scalar Division(e.g.: A/B)
	* Overload the operator "/" so that divides each element of a matrix by the corresponding element of another matrix.
	* @param M: Matrix which we divided it by the matrix back of sign "/".
	* @return returns a matrix that is equal to two Matrices Division  */
	Matrice operator / (Matrice &M);

	/** @brief
	*  Matrix Scalar Division(e.g.: A/2.)
	* Overload the operator "/" so that divides each element of a matrix by a constant value.
	* @param d: Each element of the matrix back of sign "/" divides by the constant value "d"    .
	* @return  returns a matrix that is equal to Division of a matrix and a constant value */
	Matrice operator / (Tfloat d);

	/** @brief
	* (common syntax)
	* Unary divide by scalar(e.g.: A/=2.)
	* Overload the operator "/=" so that divides each element of a matrix by a constant value.
	* @param rhs: The constant value which is divided by each element of the matrix.
	* @return result */
	Matrice &operator /= (Tfloat rhs);

	/** @brief
	*  Element-wise matrix power(e.g.;A^2., A^.5 or A^x)
	* Overload the operator "^" so that divides each element of a matrix by a constant value.
	* @param d: Each element of the matrix back of sign "/" divides by the constant value "d"    .
	* @return  returns a matrix that is equal to Division of a matrix and a constant value */
	Matrice operator^ (const Tfloat rhs);

	////////////////////////////
	//Other/////////////////////

	/** @brief
	*  A function that symmetrize a matrix. A matrix is symmetrized ,if its non-diagonal elements are equal.
	* @return It is void so does not return anything.*/
	void symetrize();

	/** @brief
	*  This function dot products two vectors.
	* @param A: A is a matrix type input which has just one row or one column.
	* @return result */
	Tfloat dot(Matrice &A);

	/** @brief
	*  This function cross products two vectors.
	* @param A: A is a matrix type input which has just one row or one column.
	* @return result */
	Matrice cross(Matrice &A);

	/** @brief
	*  A function that normalize a matrix. A matrix is normalized, if the sum of its all squared values equals to 1.
	* @return 1 if success, 0 if matrix is zero */
	int normalize();

	/** @brief
	*  A function that calculates norm(length) of a matrix as a concatenated vector.
	* @return norm */
	Tfloat norm();

	//osg vec3 syntax
	Tfloat length();

	/** @brief
	*  A function that calculates squared sum of all elements of a matrix.
	* @return norm2 */
	Tfloat norm2();

	//osg vec3 syntax
	Tfloat length2();

	/** @brief
	*  This function calculates root mean square of matrix elements.
	* @return rms*/
	Tfloat rms();

	/** @brief
	*  Calculates trace of matrix.
	* @return trace */
	Tfloat trace();

	Tfloat minim();

	Tfloat maxim();

	Tfloat median();

	Matrice abs();

	//Inverses//////////////////

	/** @brief
	* (eigen syntax)
	* This function Computes inverse of a matrix
	* @return It is void so does not return anything */
	Matrice inverse();

#if 0
#if defined(__linux__) || defined(__WIN32__)
	//recursive algorithm not appropriate for microcontrollers
	/** @brief
	* (eigen syntax)
	* This function Computes inverse of a matrix
	* @return It is void so does not return anything */
	double determinant();
#endif
#endif

	//nsrMathLibInv.cpp************************************

	Matrice operator / (Matrice M);

	int equalInv(const Matrice &A, Matrice &Tmp);//deprecated

	Tfloat determinant();        // Determinant
	bool isPositiveDefinite();
	Matrice inverseKristianLUGeneral();           // Inverse
	Matrice inverseGaussJordanGeneral();           // Inverse
	Matrice inverseLapackLUGeneral();           // Inverse
	Matrice inverseLapackPositive();           // Inverse
	Matrice inverseQuicPositive(float lambda = 0.5); // Inverse, lambda is regularization sparsification param, entering systematic error by about lambda val

private:

	Matrice scale(void);
	void copy_column(Matrice &mm, int from_col, int to_col);
	//void deepcopy (Matrice & from, Matrice & to);
	void lu_decompose(Matrice &indx, int &d, Matrice &lu_decomp);
	void switch_columns(int col1, int col2);
	void lu_back_subst(const Matrice &indx, Matrice &b);
};

//matlab syntax
Matrice zeros(int rows, int cols = -1);

//matlab syntax
Matrice ones(int rows, int cols = -1);

//matlab syntax
Matrice eye(int rows, int cols = -1);

//autofill & create row-col mat///////////////////
//count macro input args
#define COUNT_NARG(...) \
	PP_NARG_(__VA_ARGS__,PP_RSEQ_N())
#define PP_NARG_(...) \
	PP_ARG_N(__VA_ARGS__)
#define PP_ARG_N( \
				  _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
				  _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
				  _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
				  _61,_62,_63,N,...) N
#define PP_RSEQ_N() \
	63,62,61,60,                   \
	59,58,57,56,55,54,53,52,51,50,49,48,47,46,45,44,43,42,41,40, \
	39,38,37,36,35,34,33,32,31,30,29,28,27,26,25,24,23,22,21,20, \
	19,18,17,16,15,14,13,12,11,10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0

// WARNING: DON'T FORGET TO PUT FLOATING DOT AFTER INPUT NUMBERS
Matrice createmat(int rows, int cols, Tfloat first, ...);

#define colmat(...) \
	createmat(COUNT_NARG(__VA_ARGS__), 1, __VA_ARGS__)

#define rowmat(...) \
	createmat(1, COUNT_NARG(__VA_ARGS__), __VA_ARGS__)

inline Matrice transpose(Matrice &M) { return M.transpose();}

//dot product
inline Tfloat dot(Matrice &A, Matrice &B) { return A.dot(B);}
inline Matrice cross(Matrice &A, Matrice &B) { return A.cross(B);}

//matlab syntax
inline Tfloat norm(Matrice &M) { return M.norm();}
inline Tfloat norm2(Matrice &A) { return A.norm2();}
inline Tfloat rms(Matrice &A) { return A.rms();}

//matlab syntax
inline Tfloat trace(Matrice &M) { return M.trace();}

//matlab syntax is an standard function, not usable
inline Tfloat maxim(Matrice &M) { return M.maxim();}
//matlab syntax is an standard function, not usable
inline Tfloat minim(Matrice &M) { return M.minim();}

#if 0
//#if defined(__linux__) || defined(__WIN32__)
//matlab syntax
inline Tfloat median(Matrice &M) { return M.median();}
#endif

//matlab syntax is an standard function, not usable
inline Matrice absolute(Matrice &M) { return M.abs();}

void operator << (Matrice &output, const Matrice &M);

/** @brief
  * (common syntax)
  * Matrix negation(e.g.: -A)
  * @param BM: Input matrix
  * @return  output matrix
  */
Matrice operator - (Matrice &BM);

//common syntax
//Scalar to matrix multiplication(e.g.: 2.*A)
Matrice operator *(Tfloat f, Matrice &BM);

//matlab & aramdillo syntax
inline Matrice inv(Matrice &M) { return M.inverse();}
inline Matrice inverse(Matrice &A) { return A.inverse();}

#if defined(__linux__) || defined(__WIN32__)
//recursive algorithm not appropriate for microcontrollers
inline Tfloat det(Matrice &mat) { return mat.determinant();}
inline Tfloat determinant(Matrice &mat) { return mat.determinant();}
#endif

//matlab syntax
int size(Matrice &M, int dim);

//Functions not tested under Class heritage yet.
#if 0
//Define fixed size matrice classes without the need for malloc
//Not using template as it is not well behaved using limited Ram(microcontrollers, ...)
#ifdef STORE_ROWWISE

#define DefineMatriceXxX(templated_rows, templated_cols)               \
	class Matrice##templated_rows##x##templated_cols : public Matrice  \
	{                                                                  \
	public:                                                            \
		Tfloat _data[templated_rows*templated_cols];                   \
		Tfloat* p_data[templated_rows];                                \
		\
		Matrice##templated_rows##x##templated_cols()                   \
		/* prevent call of default mother constructor with 0,0 dimension*/ \
			: Matrice(templated_rows, templated_cols, _data, p_data) { \
			int i;                                                     \
			for(i=0; i<templated_rows; i++)                            \
				p_data[i] = &_data[i*templated_cols];                  \
		}                                                              \
	};

#else /*STORE_COLWISE*/

#define DefineMatriceXxX(templated_rows, templated_cols)               \
	class Matrice##templated_rows##x##templated_cols : public Matrice  \
	{                                                                  \
	public:                                                        \
		Tfloat _data[templated_rows*templated_cols];                   \
		Tfloat* p_data[templated_cols];                                \
		\
		Matrice##templated_rows##x##templated_cols()                   \
		/* prevent call of default mother constructor with 0,0 dimension*/ \
			: Matrice(templated_rows, templated_cols, _data, p_data) { \
			int i;                                                     \
			for(i=0; i<templated_cols; i++)                            \
				p_data[i] = &_data[i*templated_rows];                  \
		}                                                              \
	};

#endif

DefineMatriceXxX(2, 1)
DefineMatriceXxX(3, 1)
DefineMatriceXxX(4, 1)
DefineMatriceXxX(3, 3)
DefineMatriceXxX(4, 4)
#endif

#ifdef __cplusplus
}
#endif

#endif /* __NSRMATHLIB_H__ */

