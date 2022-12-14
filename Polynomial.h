/**************************
Title: Polynomial.h
Author: Andrew Lounsbury
Date: 6/2/2020
Purpose: for holding EU's
**************************/

/***************************************
1) create nT zero terms in nV variables
2) read in coeff and expo for each term
3) remove zero terms
4) add like terms
***************************************/

#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H
#include "inputValidation.cpp"
#include "Term.h"

#include <algorithm>
#include <cmath>
#include <cctype>
#include <iostream>
#include <vector>
#include <sstream>
using namespace std;

// polynomials of the form [coefficient, exponents] -> ... -> [coefficient, exponents]
class Polynomial
{
	private:
		Term* leading;
		Term* trailing;
		int numTerms = -1;
		int numVariables = -1;
		int totalDegree = -1;
		vector<vector<Polynomial*> > derivatives;
		bool linear;
	public:		
		double getCoefficient(int i) const 				{ return getTerm(i)->coefficient; }
		vector<vector<Polynomial*> > getDerivatives() 	{ return derivatives; }
		int getExponent(int t, int var) const 			{ return getExponents(t).at(var); }
		vector<int> getExponents(int i) const 			{ return this->getTerm(i)->exponents; }
		int getNumTerms() const 						{ return numTerms; }
		int getNumVariables() const 					{ return numVariables; }
		int getSizeDerivatives() 						{ return derivatives.size(); }
		// with respect to v
		int getSizeDerivativesWRT(int v) 				{ return derivatives.at(v).size(); }
		int getTotalDegree() const 						{ return totalDegree; }
		Term* getTrailing() const 						{ return trailing; }
		void setCoefficient(int t, double num) 			{ getTerm(t)->coefficient = num; }
		void setExponent(int t, int v, int num) 		{ getTerm(t)->exponents.at(v) = num; }
		void setExponents(int t, vector<int> expo) 		{ getTerm(t)->exponents = expo; }
		void setNext(int t, Term* term) 				{ getTerm(t)->next = term;}
		void setNumTerms(int num) 						{ numTerms = num; }
		void setTotalDegree(int num) 					{ totalDegree = num; }
		
		Polynomial();
		Polynomial(int, int, int);
		Polynomial(string);
		~Polynomial();
		
		Polynomial* operator+(const Polynomial &p);
		Polynomial operator*(const Polynomial &p);
		Polynomial* operator-(const Polynomial &p);
		bool operator==(const Polynomial &p);
		bool operator!=(const Polynomial &p);
		bool operator<(const Polynomial &p);
		bool operator>(const Polynomial &p);
		
		void addTerm(int, vector<int>);
		void addTerm(Term*);
		void appendTerm(double, vector<int>);
		void computeTotalDegree();
		Polynomial* derivative(int);
		void enterInfo();
		double eval(double);
		Polynomial* getDerivative(int, int);
		int getNonZeroExpo(int) const;
		Term* getTerm(int) const;
		void insertTerm(int, vector<int>, int);
		void insertTerm(int, Term*);
		Polynomial* integrate(int);
		double integrateOverInterval(double, double, int);
		bool isConstant();
		bool isConstantTerm(int);
		bool isLinear();
		void lexOrder();
		void printPolynomial();
		void printPolynomial(int, int);
		void removeTerm(int &);
		void setEUCoefficients(vector<int>, int);
		void setEUExponents(vector<vector<int> >);
		void setTerm(int, Term*);
		void simplify();
		vector<string> split(string, const char);
		void tokenize(string const &, const char, vector<string> &);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief default constructor
Polynomial::Polynomial()
{
	leading = NULL;
	trailing = NULL;
	
	numTerms = -1;
	numVariables = -1;
	totalDegree = -1;
	
	linear = true;
}

// these polynomials are in R[var]
// a_{nT}x^{nT} + a_{nT-1}x^{nT-1} + ... + a_1x^{nT-(nT-1)} + a_0
// FINISH
Polynomial::Polynomial(int nT, int nV, int var)
{
	numVariables = nV;
	
	if (nT == -1 || var == -1) // default is 1x_{var}
	{		
		numTerms = 1;
		
		leading = new Term;
		leading->coefficient = 1;
		
		leading->exponents = vector<int>(numVariables); // numVariables is numPlayers
		for (int i = 0; i < numVariables; i++)
		{
			if (i == var)
				leading->exponents.at(i) = 1;
		}
		leading->next = NULL;
		
		trailing = new Term;
		trailing = leading;
		
		totalDegree = 1;
	}
	else
	{
		numTerms = nT;
		
		leading = new Term;
		leading->coefficient = 0;
		leading->exponents.resize(numVariables);
		for (int v = 0; v < numVariables; v++)
		{
			if (v == var)
				leading->exponents.at(v) = numTerms - 1;
			else
				leading->exponents.at(v) = 0;
		}
		leading->next = NULL;
		
		if (numTerms == 1)
			trailing = leading;
		else if (numTerms == 2)
		{			
			trailing = new Term;
			trailing->coefficient = 0;
			trailing->exponents.resize(numVariables);
			for (int v = 0; v < numVariables; v++)
				trailing->exponents.at(v) = 0;
			trailing->next = NULL;
			leading->next = trailing;		
		}
		else // numTerms >= 3
		{
			Term* temp = new Term;
			Term* term2 = new Term;
			term2->coefficient = 0;
			term2->exponents.resize(numVariables);
			for (int v = 0; v < numVariables; v++)
			{
				if (v == var)
					term2->exponents.at(v) = numTerms - 2;
				else
					term2->exponents.at(v) = 0;
			}
			leading->next = term2;
			
			temp = term2;
			for (int t = 2; t < numTerms - 1; t++) // terms 2,..., numTerms - 1
			{
				Term* newTerm = new Term;
				newTerm->coefficient = 0;
				newTerm->exponents.resize(numVariables);
				for (int v = 0; v < numVariables; v++)
				{
					if (v == var)
						newTerm->exponents.at(v) = numTerms - (t + 1);
					else
						newTerm->exponents.at(v) = 0;
				}
				temp->next = newTerm; // term t - 1 next points to term t
				temp = newTerm; // temp moves up one term
			}
			
			trailing = new Term;
			temp->next = trailing;
			trailing->coefficient = 0;
			trailing->exponents.resize(numVariables);
			for (int v = 0; v < numVariables; v++)
				trailing->exponents.at(v) = 0;
			trailing->next = NULL;
		}
		totalDegree = -1;
	}
	linear = true;
}

/// @brief splits a string into substrings based on the location of the character c in str
/// @param str 
/// @param c 
/// @return 
vector<string> Polynomial::split(string str, const char c)
{
    string substring = "";
	vector<string> subStrings;
    for (auto x : str)
    {
        if (x == c)
        {
            subStrings.push_back(substring);
            substring = "";
        }
        else
            substring = substring + x;
    }
    subStrings.push_back(substring);
	return subStrings;
}

/// @brief creates a polynomial of the form given in s
/// @param s 
Polynomial::Polynomial(string s)
{
	// removing spaces
	s.erase(remove_if(s.begin(), s.end(), ::isspace), s.end());

	// input validation
	for (int i = 0; i < s.length(); i++)
	{
		if (isalpha(s[i]) && s[i] != 'x')
		{
			cout << "ERROR: input string must be of the form ax^n + bx^{n-1} + ... + yx + z" << endl;
			delete this;
			return;
		}
	}

	// getting each term
	vector<string> terms = split(s, '+');
	numTerms = terms.size();

	numVariables = 1;

	// splitting over ^'s, components contains (cx, e)
	vector<vector<string> > components;
	for (auto t : terms)
		components.push_back(split(t, '^'));

	// saving in coeffs and exponents
	vector<double> coeffs;
	vector<int> exponents;
	for (int i = 0; i < terms.size(); i++)
	{
		vector<string> pair = split(components[i][0], 'x');
		if (pair[0] == "")
			coeffs.push_back(1.0);
		else
			coeffs.push_back(stod(pair[0]));

		if (components[i].size() > 1)
			exponents.push_back(stoi(components[i][1]));
		else
			exponents.push_back(1);
	}

	// building the polynomial
	leading = NULL;
	trailing = NULL;

	for (int i = 0; i < numTerms; i++)
	{
		vector<int> temp(numTerms);
		temp.at(0) = exponents.at(i);
		this->appendTerm(coeffs.at(i), temp);
	}

	this->printPolynomial();
}

// destructor
Polynomial::~Polynomial()
{
	Term* curTerm;
	
	curTerm = leading;
	while (curTerm)
	{
		delete curTerm;
		curTerm = curTerm->next;
	}
	delete this;
}

/// @brief polynomial equality
/// @param p 
/// @return 
bool Polynomial::operator==(const Polynomial &p)
{
	// comparing total degree, numTerms
	if (this->getTotalDegree() != p.getTotalDegree() || this->getNumTerms() != p.getNumTerms())
		return false;
	
	for (int i = 0; i < p.getNumTerms(); i++)
	{
		if (this->getCoefficient(i) != p.getCoefficient(i))// || this->getExponents(i) != p.getExponents(i))
			return false;
		
		if (this->getExponents(i) != p.getExponents(i))
			return false;
	}
	return true;
}

/// @brief polynomial inequality
/// @param p 
/// @return 
bool Polynomial::operator!=(const Polynomial &p)
{
	if (*this == p)
		return false;
	else
		return true;
}

void Polynomial::appendTerm(double c, vector<int> e)
{
	Term *newTerm;

	// Allocate a new term
	newTerm = new Term;
	newTerm->coefficient = c;
	newTerm->exponents = e;
	newTerm->next = NULL;

	// If no terms in list make newTerm the first term.
	if (!leading) 
	{
		leading = newTerm;
		trailing = newTerm;
	}
	else  // Otherwise, insert newTerm at end.
	{
		trailing->next = newTerm;
		trailing = newTerm;
	}
}

// get total degree
void Polynomial::computeTotalDegree()
{
	int sum = 0, max = 0;
	for (int t = 0; t < numTerms; t++)
	{
		sum = 0;
		for (int v = 0; v < numVariables; v++)
			sum += getTerm(t)->exponents.at(v);
		
		if (sum > max)
			max = sum;
	}
	setTotalDegree(max);
}

// get derivative
// FINISH: linear derivatives from nonlinear polynomials
Polynomial* Polynomial::derivative(int var)
{
	// computes the derivative of this wrt to the var-th variable
	
	Polynomial* poly = new Polynomial(this->getNumTerms(), this->getNumVariables(), 0);
	for (int t = 0; t < poly->getNumTerms(); t++)
	{
		poly->setCoefficient(t, this->getCoefficient(t));
		for (int v = 0; v < poly->getNumVariables(); v++)
			poly->setExponent(t, v, this->getExponent(t, v));
	}
	
	for (int t = 0; t < poly->getNumTerms(); t++)
	{
		if (poly->getExponent(t, var) != 0) // var-th variable occurs
		{			
			poly->setCoefficient(t, poly->getCoefficient(t) * poly->getExponent(t, var));
			
			for (int v = 0; v < poly->getNumVariables(); v++)
			{
				if (v != var) // if nonlinear, this is taking the partial
					poly->setExponent(t, v, 0);
				else
					poly->setExponent(t, v, poly->getExponent(t, v) - 1);
			}
		}
		else // var-th variable does not occur
		{
			if (numTerms > 1)
				poly->removeTerm(t);
			else
				poly->setCoefficient(t, 0); // is now the zero polynomial
		}
	}
	// poly->printPolynomial();
	return poly;
}

// enter info
/* separate bc mixed strategies doesn't 
require the user to enter info */
void Polynomial::enterInfo()
{
	double c = 0.0;
	int e = -1;
	
	for (int t = 0; t < numTerms; t++)
	{
		cout << "-------------------------------------------" << endl;
		if(t == 0)
			cout << "Enter the 1st coefficient:  ";
		else if (t == 1)
			cout << "Enter the 2nd coefficient:  ";
		else if (t == 2)
			cout << "Enter the 3rd coefficient:  ";
		else
			cout << "Enter the " << t + 1 << "-th coefficient:  ";
		cin >> c;
		validateTypeDouble(c);
		this->setCoefficient(t, c);
		
		for (int v = 0; v < numVariables; v++)
		{
			if (t == 0)
			{
				cout << "Enter the ";
				if (v == 0)
					cout << "1st ";
				else if (v == 1)
					cout << "2nd ";
				else if (v == 2)
					cout << "3rd ";
				else
					cout << v + 1 << "-th ";
				cout << "exponent in the 1st term:  ";
			}
			else if (t == 1)
			{
				cout << "Enter the ";
				if (v == 0)
					cout << "1st ";
				else if (v == 1)
					cout << "2nd ";
				else if (v == 2)
					cout << "3rd ";
				else
					cout << v + 1 << "-th ";
				cout << "exponent in the 2nd term:  ";
			}
			else if (t == 2)
			{
				cout << "Enter the ";
				if (v == 0)
					cout << "1st ";
				else if (v == 1)
					cout << "2nd ";
				else if (v == 2)
					cout << "3rd ";
				else
					cout << v + 1 << "-th ";
				cout << "exponent in the 3rd term:  ";
			}
			else
			{
				cout << "Enter the ";
				if (v == 0)
					cout << "1st ";
				else if (v == 1)
					cout << "2nd ";
				else if (v == 2)
					cout << "3rd ";
				else
					cout << v + 1 << "-th ";
				cout << "exponent in the " << t + 1 << " term:  ";
			}
			cin >> e;
			validateTypeInt(e);
			this->setExponent(t, v, e);			
		}
	}
	computeTotalDegree();
}

// evaluate polynomial function at val
double Polynomial::eval(double val)
{
	double num = 1.0, sum = 0.0;
	
	if (isConstant())
		return getCoefficient(0);
	else
	{
		for(int t = 0; t < numTerms; t++)
		{ 
			num = getCoefficient(t);
			for (int v = 0; v < numVariables; v++)
				num *= pow(val, getExponent(t, v));
			
			sum += num;
		}
		return sum;
	}
}

// computes derivatives up to the (n + 1)-th derivative wrt the v-th variable
Polynomial* Polynomial::getDerivative(int n, int v)
{
	bool loop = false;
	
	// Resizing derivatives
	if (getSizeDerivatives() < v + 1)
		derivatives.resize(v + 1);
	
	if (derivatives.at(v).size() == 0)
	{
		derivatives.at(v).resize(1);
		derivatives.at(v).at(0) = this;
		loop = true;
	}
	else if (derivatives.at(v).at(0) != this)
	{
		derivatives.at(v).at(0) = this;
		loop = true;
	}
	
	// Pushing into derivatives.at(v) until it has spots 0,...,n
	cout << "size: " << getSizeDerivativesWRT(v) << endl;
	cout << "n + 1: " << n + 1 << endl;
	while (getSizeDerivativesWRT(v) < n + 1 && loop)
	{
		cout << "size: " << getSizeDerivativesWRT(v) << endl;
		
		if (getSizeDerivativesWRT(v) == 0)
			derivatives.at(v).push_back(derivative(v));
		else
			derivatives.at(v).push_back(getDerivative(derivatives.at(v).size() - 1, v)->derivative(v));
	}
	cout << "size1: " << derivatives.size() << endl;
	cout << "size2: " << derivatives.at(v).size() << endl;
	return derivatives.at(v).at(n);
}

// get nonzero exponent
// for linear polynomials
int Polynomial::getNonZeroExpo(int t) const
{ 
	for (int v = 0; v < numVariables; v++)
	{
		if (this->getExponent(t, v) != 0)
			return v;
	}
	return (numTerms - 1);
}

// get term
Term* Polynomial::getTerm(int t) const
{
	int count = 0;
	Term* curTerm;
	curTerm = leading;
	
	while (count < t)
	{
		curTerm = curTerm->next;
		count++;
	}
	return curTerm;
}

// insert term
void Polynomial::insertTerm(int t, vector<int> expo, int coeff)
{
	Term* newTerm = new Term;
	
	// putting info into newTerm
	newTerm->coefficient = coeff;
	for (int n = 0; (unsigned)n < expo.size(); n++)
		newTerm->exponents.push_back(expo.at(n));
	
	if (t == 0)
	{
		newTerm->next = leading->next;
		leading = newTerm;
	}
	else
	{
		newTerm->next = getTerm(t - 1)->next; // t - 1 --> new --> t
		getTerm(t - 1)->next = newTerm; // insert at position i
	}
	numTerms++;
}

// insert term
void Polynomial::insertTerm(int t, Term* term)
{	
	if (t == 0)
	{
		term->next = leading->next;
		leading = term;
	}
	else
	{
		term->next = getTerm(t - 1)->next; // i - 1 --> new --> i
		getTerm(t - 1)->next = term; // insert at position i
	}
	numTerms++;
}

// integrate
Polynomial* Polynomial::integrate(int var) // FINISH (... + c)
{
	Polynomial* poly = new Polynomial(getNumTerms(), getNumVariables(), 0);
	
	// copying this pointer
	for (int t = 0; t < numTerms; t++)
	{
		for (int v = 0; v < poly->getNumVariables(); v++)
		{
			poly->setCoefficient(t, getCoefficient(t));
			poly->setExponent(t, v, getExponent(t, v));
		}
	}
	// computing
	for (int t = 0; t < numTerms; t++)
	{
		poly->setExponent(t, var, poly->getExponent(t, var) + 1);
		poly->setCoefficient(t, poly->getCoefficient(t) / static_cast<double>(poly->getExponent(t, var)));
	}
	return poly;
}

// integrate over [a, b]
double Polynomial::integrateOverInterval(double a, double b, int var)
{
	// cout << "INTEGRATEOVER\n";
	
	double num = -1;
	Polynomial* poly = new Polynomial(getNumTerms(), getNumVariables(), 0);
	
	for (int t = 0; t < poly->getNumTerms(); t++)
	{
		for (int v = 0; v < poly->getNumVariables(); v++)
		{
			poly->setCoefficient(t, getCoefficient(t));
			poly->setExponent(t, v, getExponent(t, v));
		}
	}
	poly = poly->integrate(var);
	num = poly->eval(b) - poly->eval(a);
	
	cout << endl;
	
	// cout << "num:  " << num << endl;
	// cout << "end INTEGRATEOVER\n";
	return num;
}

// checks if constant
bool Polynomial::isConstant()
{
	bool allExpoZero = true;
	for (int t = 0; t < numTerms; t++)
	{
		if (!isConstantTerm(t))
		{
			allExpoZero = false;
			return allExpoZero;
		}
	}
	return allExpoZero;
}

// checks if constant term
bool Polynomial::isConstantTerm(int t)
{
	bool allExpoZero = true;
	for (int v = 0; v < numVariables; v++)
	{
		if(getExponent(t, v) != 0)
			allExpoZero = false;
	}
	return allExpoZero;
}

// checks if linear
bool Polynomial::isLinear() // TEST
{
	int numExponents = 0;
	for (int t = 0; t < numTerms; t++)
	{
		numExponents = 0;
		for (int v = 0; v < numVariables; v++)
		{
			if (getExponent(t, v) != 0)
				numExponents++;
			if (numExponents > 1)
				return false;
		}
	}
	return true;
}

// orders using lex order
void Polynomial::lexOrder()
{
	// LME nonzero entry of alpha-beta = (a_1 - b_1, ... , a_n - b_n) is positive
	
	Term* temp = new Term;
	
	for (int t1 = 0; t1 < getNumTerms(); t1++)
	{
		for (int t2 = t1 + 1; t2 < getNumTerms(); t2++)
		{
			for (int var = 0; var < getNumVariables(); var++)
			{
				if (getExponent(t1, var) - getExponent(t2, var) < 0) // switch terms t1 and t2
				{
					temp->coefficient = getCoefficient(t2);
					temp->exponents = getExponents(t2);
					
					setTerm(t2, getTerm(t1));
					setTerm(t1, temp);
				}
			}
		}
	}
}

// print polynomial
void Polynomial::printPolynomial()
{
	bool nonConstant = false, oneMoreNonZero = false;
	int count = 0;

	if (!leading)
		cout << "EMPTY POLYNOMIAL: enter values for coefficients and exponents";
	else
	{
		// simplify();
		for (int t = 0; t < numTerms; t++)
		{
			// check if not constant
			nonConstant = false;
			for (int v = 0; v < numVariables; v++)
			{
				if (getExponent(t, v) != 0)
					nonConstant = true; // at least one is nonzero
			}
			// check if there's one more nonzero term
			oneMoreNonZero = false;
			count = t + 1;
			if (t < numTerms - 1)
			{
				while (!oneMoreNonZero && count < numTerms)
				{
					if (this->getCoefficient(count) != 0)
						oneMoreNonZero = true;
					count++;
				}
			}
		
			if (getCoefficient(t) != 0) // nonzero coefficient
			{
				// if first term negative
				if (t == 0 && getCoefficient(t) < 0)
				{
					if ((getCoefficient(t) != -1 && nonConstant) || !nonConstant)
						cout << getCoefficient(t);
					else
						cout << "-";
				}
				else // not first term OR nonnegative coefficient
				{
					if ((abs(getCoefficient(t)) != 1 && nonConstant) || !nonConstant)
						cout << abs(getCoefficient(t));
				}
				
				// printint x^...
				for (int v = 0; v < numVariables; v++)
				{
					if (getExponent(t, v) != 0)
					{
							cout << "x";
							if (numVariables > 1)
								cout << "_" << v + 1;
							if (getExponent(t, v) != 1)
								cout << "^" << getExponent(t, v);
					}
					else
						cout << "HERE";
				}
				
				if (t < numTerms - 1 && oneMoreNonZero && getCoefficient(t + 1) >= 0) // && exists(a_k)[i < k <= n && a_k != 0]
					cout << " + ";
				else if (t < numTerms - 1 && oneMoreNonZero && getCoefficient(t + 1) < 0)
					cout << " - ";
			}
			else if (getCoefficient(t) == 0) // zero coefficient
			{
				if (numTerms == 1) // in a monomial
					cout << "0";
			}
		}
	}
	cout << endl;
}

// print polynomial
// For EU's
void Polynomial::printPolynomial(int player, int strat)
{
	bool nonConstant = false, oneMoreNonZero = false;
	int count = 0;
	
	simplify();
	for (int t = 0; t < numTerms; t++) // terms
	{
		nonConstant = false;
		for (int v = 0; v < numVariables; v++) // variables
		{
			if (getExponent(t, v) != 0)
				nonConstant = true; // at least one is nonzero
		}
		
		oneMoreNonZero = false;
		count = t + 1;
		if (t < numTerms - 1) // checks if there's one more nonzero term
		{
			while (!oneMoreNonZero && count != numTerms)
			{
				if (getCoefficient(count) != 0)
					oneMoreNonZero = true;
				
				count++;
			}
		}
		
		if (!leading) // empty
			cout << "EMPTY POLYNOMIAL: enter values for coefficients and exponents";
		else // not empty
		{
			if (getCoefficient(t) != 0) // nonzero coefficient
			{
				if (t == 0 && getCoefficient(t) < 0) // first term negative
				{
					if ((getCoefficient(t) != -1 && nonConstant) || !nonConstant)
						cout << getCoefficient(t);
					else
						cout << "-";
				}
				else // not first term OR nonnegative coefficient
				{
					if ((abs(getCoefficient(t)) != 1 && nonConstant) || !nonConstant)
						cout << abs(getCoefficient(t));
				}
				
				for (int v = 0; v < numVariables; v++)
				{
					if (getExponent(t, v) != 0)
					{
							cout << "p_" << player << ", " << strat;
							if (getExponent(t, v) != 1)
								cout << "^" << getExponent(t, v);
					}
				}
				
				if (t < numTerms - 1 && oneMoreNonZero && getCoefficient(t + 1) >= 0) // && exists(a_k)[i < k <= n && a_k != 0]
					cout << " + ";
				else if (t < numTerms - 1 && oneMoreNonZero && getCoefficient(t + 1) < 0)
					cout << " - ";
			}
			else if (getCoefficient(t) == 0) // zero coefficient
			{
				if (numTerms == 1) // in a monomial
					cout << "0";
			}
		}
	}
	cout << endl;
}

// remove term
void Polynomial::removeTerm(int &t)
{	
	int count = 0;
	Term* curTerm;
	Term* previousTerm;
	
	if (t == 0)
	{
		delete leading;
		leading = leading->next;
	}
	else
	{
		curTerm = leading;
		while (count < t)
		{
			previousTerm = curTerm;
			curTerm = curTerm->next;
			count++;
		}
		previousTerm->next = curTerm->next;
		delete curTerm;
		curTerm = curTerm->next;
	}
	numTerms--;
	t--;
}

// set EU coeffs
void Polynomial::setEUCoefficients(vector<int> coeffs, int numPlayers) // TEST
{
	// enters coeffs into the expected utility, leaving the last term as a_{n-1}
	
	// if (numPlayers < 3)
	// {
		/*
		nP == 2:
		--------
		n = numTerms
		a_0p_0 + ... + a_{n-2}p_{n-2} + a_{n-1}(1 - p_0 - ... - p_{n-2})
		a_0p_0 + ... + a_{n-2}p_{n-2} + a_{n-1} - a_{n-1}p_0 - ... - a_{n-1}p_{n-2}
		(a_0p_0 - a_{n-1}p_0) + ... + (a_{n-2}p_{n-2} - a_{n-1}p_{n-2}) + a_{n-1}
		(a_0 - a_{n-1})p_0 + ... + (a_{n-2} - a_{n-1})p_{n-2} + a_{n-1}

		0p + 3(1-p)
		0p + 3 - 3p
		0p - 3p + 3
		(0 - 3)p + 3
		-3p + 3
		*/
			
		// (a_i - a_{n-1})p_i
		for (int t = 0; t < numTerms - 1; t++)
		{
			this->setCoefficient(t, coeffs.at(t) - coeffs.at(numTerms - 1));
			for (int v = 0; v < numVariables; v++)
			{
				if (t == v)
					this->setExponent(t, v, 1);
				else
					this->setExponent(t, v, 0);
			}
		}
		// a_{n-1}
		this->setCoefficient(numTerms - 1, coeffs.at(numTerms - 1));
		for (int v = 0; v < numVariables; v++)
			this->setExponent(numTerms - 1, v, 0);
	// }
	// else
		// cout << "ERROR: numPLayers > 2\n";
}

// set EU exponents
void Polynomial::setEUExponents(vector<vector<int> > exponents) // FINISH: need number of terms per polynomial
{
	// enters exponents into the expected utility
	
	// for (int t = 0; t < numTerms; t++)
		// this->setTerm(t, exponents.at(t));
}

// set term
void Polynomial::setTerm(int t, Term* temp)
{
	this->setCoefficient(t, temp->coefficient);
	this->setExponents(t, temp->exponents);
}

/// @brief simplifies a polynomial and puts its terms in lexicographical order
void Polynomial::simplify()
{	
	bool sameAlpha = true;
	
	// getting rid of zero terms
	for (int t = 0; t < numTerms; t++)
	{
		if (getCoefficient(t) == 0 && numTerms > 1)
			removeTerm(t);
	}
	
	// combining nonzero terms
	for (int t1 = 0; t1 < numTerms; t1++)
	{
		for (int t2 = t1 + 1; t2 < numTerms; t2++)
		{
			sameAlpha = true;
			for (int v = 0; v < numVariables; v++)
			{
				if (getTerm(t1)->exponents.at(v) != getTerm(t2)->exponents.at(v))
					sameAlpha = false;
			}
			if (sameAlpha)
			{
				setCoefficient(t1, getCoefficient(t1) + getCoefficient(t2));
				if (numTerms > 1)
					removeTerm(t2);
			}
		}
	}
	this->lexOrder();
}

#endif