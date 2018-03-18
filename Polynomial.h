/*
#include<iostream>
using namespace std;
#include<string>
#include<gmp.h>
#include <gmpxx.h>
#include<cstring>
#include<iostream>
#include<fstream>
#include <stdlib.h>
*/
#include "Hashtable.h"
typedef mpz_t bigint;





class Polynomial{

public: 
	Polynomial(){};
	Polynomial(bigint* elem, string poly_ID, bigint * xpoints,int elem_size, int xpoints_size, bigint pubmoduli); 
	//~Polynomial(){delete[]values;};
	bigint *  evaluate (bigint *,bigint * ,int,int,bigint);// given a set of polynomials root, it constructs (x-e_1)...(x-x_n) and 
//evaluates it at a set of x_i.


	bigint* evaluate_coeff(bigint* coeff,int size ,bigint x, bigint pubmoduli); //given a polynomial's coefficients it evalutes the 
//polynomial  at x_i
	bigint * get_values(); 
	void   blind_poly (bigint , bigint); 
	void set_values(bigint*vals, int size);

string get_poly_ID(){return poly_ID;}


	bigint * values;  //xxxx make it private later on
private:
	string  poly_ID; 

	int val_size;
};





