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
#include"Rand.h"

typedef mpz_t bigint;



class Hashtable{

public:
	Hashtable ( int elem_in_bucket, bigint* elemen, int elem_size,int table_size);
	//~Hashtable(){ delete[] T;}
	bigint* get_bucket(int index);
private:
	bigint **T;// hash table
	int *oc_buckets; //index of occupied buckets.
	int oc_buk_size;
	int *find_oc_buckets(int*b, int size,int& new_size);
	bool exits(int b,int *a,int size);
	//These two are not used in the protocol
	int T_size;//table_length
	int max_bucket_load;
};




