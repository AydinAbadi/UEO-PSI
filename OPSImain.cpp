/*
#include<iostream>
#include<gmp.h>
#include <gmpxx.h>
#include<sstream>
using namespace std;
#include<iomanip>
#include <string>
#include<cstring>
#include<fstream>
#include<math.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <assert.h>

*/

//#include "cryptopp/pwdbased.h"
//#include <cryptopp/pwdbased.h>
//#include "cryptopp/hmac.h"

//#include <cryptopp/hmac.h>
//#include <cryptopp/cryptlib.h>
#include "OPSImain.h"
//#include "flint.h"
//#include"mpfr.h"
//#include "fmpz_mod_poly.h"
typedef mpz_t bigint;
//#include <omp.h>
//#include <time.h>
//typedef mp_limb_t * fmpz_t;


//#include <cryptopp/sha.h>
//#include <cryptopp/filters.>
//#include <cryptopp/hex.h>



//  Fisher-Yates (pseudorandom) Shuffling
bigint*  PR_shuffle(bigint*elem, int size, bigint seed){

	bigint *res;
	bigint buf;
	res=(mpz_t*)malloc(size*sizeof(mpz_t));
	for(int i=0;i<size;i++){
		mpz_init(res[i]);
		mpz_set(res[i],elem[i]);
	}
	int indx=0;
	// use the seed to generate a random value between [1,j]
	gmp_randstate_t rand;
	gmp_randinit_default(rand);
	gmp_randseed(rand,seed);
	bigint temp,big_j;
	mpz_init(temp);
	mpz_init(big_j);
	for (int  j=size-1; j>0; j--){
		mpz_urandomb(temp,rand,110);
		mpz_set_ui(big_j,j);
		mpz_mod(temp,temp,big_j);
		indx=mpz_get_ui(temp);
		mpz_init_set(buf,res[j]);
		mpz_init(res[j]);
		mpz_set(res[j],res[indx]);
		mpz_set(res[indx],buf);
	}
	return res;
}


// Random shuffle: Fisher-Yates algorithm
bigint**  R_shuffle(bigint**elem, int size){

	bigint **res;
	res=(mpz_t**)malloc(size*sizeof(mpz_t));
	for (int i=0;i<size;i++){
		res[i]=(mpz_t*)malloc(2*sizeof(mpz_t));
	}
	for(int i=0;i<size;i++){
		mpz_init(res[i][0]);
		mpz_init(res[i][1]);
		mpz_set(res[i][0],elem[i][0]);
		mpz_set(res[i][1],elem[i][1]);
	}
	int indx=0;
	bigint buf,buf1;
	Random rd;
	gmp_randstate_t rand;
	bigint ran;
	rd.init_rand3(rand, ran, 8);
	bigint temp,big_j;
	mpz_init(temp);
	mpz_init(big_j);
	for (int  j=size-1; j>0; j--){
		mpz_urandomb(temp,rand,110);
		mpz_set_ui(big_j,j);
		mpz_mod(temp,temp,big_j);
		indx=mpz_get_ui(temp);
		mpz_init_set(buf,res[j][0]);
		mpz_init_set(buf1,res[j][1]);
		mpz_init(res[j][0]);
		mpz_init(res[j][1]);
		mpz_set(res[j][0],res[indx][0]);
		mpz_set(res[j][1],res[indx][1]);
		mpz_set(res[indx][0],buf);
		mpz_set(res[indx][1],buf1);
	}
	return res;
}






// for test only
bigint* encode(bigint* a,int a_size){


	bigint *res;
	res=(mpz_t*)malloc(a_size*sizeof(mpz_t));
	string bin_val, bin_hash,bin_enc;
	bigint hash;
	string s_val;
	CryptoPP::SHA1 hash2;
	string tmpk;

	for(int i=0;i<a_size;i++){
		CryptoPP::byte digest[CryptoPP::SHA1::DIGESTSIZE];
// converts each element to string
		s_val=mpz_get_str(NULL, 10,a[i]);
// computes hash value
		hash2.CalculateDigest(digest,(CryptoPP:: byte*)s_val.c_str(), s_val.length());
		mpz_init(hash);
//converts hash value to big integer
		mpz_import(hash,sizeof(digest),1,sizeof(digest[0]),0,0,digest);
// converts hash to string of binary
		bin_hash=mpz_get_str(NULL,2,hash);//xxxxx In order to have smaller size encoded value we can reduce the hash

// fixes has to 160-bit
		if(bin_hash.length() < 160){ // the Sha1 output length varies between 158 and 160. Here we make sure the output is 160-bit string by padding 0.
			int dif=160-bin_hash.length();
			for (int j=0;j<dif;j++){tmpk+="0";}
			}
		bin_hash=tmpk+bin_hash;
		if(hash_length<160){bin_hash=bin_hash.substr(0,hash_length);}
// converts element to string of binary
		bin_val=mpz_get_str(NULL, 2,a[i]);
// concatenates value with hash value (both are in form of string of binary)
		bin_enc=bin_val+bin_hash;//the result contains the encoded file in binary
// converts the encoded binary string to big integer
		char tmp[bin_enc.length()];//XXXXX thie size should be smaller, revise it
		strcpy(tmp,bin_enc.c_str());
		mpz_init_set_str(res[i],tmp,2);
			s_val.clear();
			mpz_clear(hash);
			bin_val.clear();
			bin_hash.clear();
			bin_enc.clear();
			tmpk.clear();

	}
return res;

}
//end of the test

























bigint* gen_randSet (int size, int max_bitsize){
	Random rd;
	mpz_t *pr_val;
	pr_val=(mpz_t*)malloc(size*sizeof(mpz_t));
	int max_bytesize=max_bitsize;
	gmp_randstate_t rand;
	bigint ran;
	rd.init_rand3(rand, ran, max_bytesize);

	for(int i=0;i<size;i++){
		mpz_init(pr_val[i]);
		mpz_urandomb(pr_val[i],rand,max_bitsize);// The last argument is in bit
	}
	return pr_val;
}



bigint* gen_labels (int size, int max_bitsize, bigint seed){

	mpz_t *labels;
	labels=(mpz_t*)malloc(size*sizeof(mpz_t));
	gmp_randstate_t rand;
	gmp_randinit_default(rand);
	gmp_randseed(rand,seed);
	for(int i=0;i<size;i++){
		mpz_init(labels[i]);
		mpz_urandomb(labels[i],rand,max_bitsize);// The last argument is in bit
	}
// Check if all the labels are distinct.
	for(int i=0;i<size;i++){
		for(int j=0;j<size;j++){
			if (j!=i){
				if(mpz_cmp(labels[i],labels[j])==0) {
					cout<<"\nPick another Label key"<<endl;break;
					}
			}
		}
	}
	return labels;
}






bigint** gen_map (int size, int max_bitsize, bigint seed1, bigint seed2){

	//mpz_t *labels1,*labels2;
	bigint **labels,**labels2;
	labels= (mpz_t**)malloc(size*sizeof(mpz_t));
	for(int i=0;i<size;i++){
		labels[i]=(mpz_t*)malloc(2*sizeof(mpz_t));
	}
	gmp_randstate_t rand1,rand2;
	gmp_randinit_default(rand1);
	gmp_randinit_default(rand2);
	gmp_randseed(rand1,seed1);
	gmp_randseed(rand2,seed2);

	for(int i=0;i<size;i++){
		//mpz_init(labels1[i]);
		mpz_init(labels[i][0]);
		mpz_init(labels[i][1]);
		mpz_urandomb(labels[i][0],rand1,max_bitsize);// The last argument is in bit
		mpz_urandomb(labels[i][1],rand2,max_bitsize);
	}


// the label pairs must be randomly permuted.

	labels2=R_shuffle(labels,  size);

	return labels2;
}




int get_size(char* a_sou){


	int count=0;
	ifstream rd(a_sou);
	//bigint temp;
	char c[1024];
	//count number of elem
	while(rd>>c){
		count++;
	}
	rd.close();
	return count;
}



bigint* get_vals(char* a_sou,int & size){

	int count=0;
	ifstream rd1(a_sou);
	//bigint temp;
	char c1[1024];
	//count number of elem
	while(rd1>>c1){
		count++;
	}
	rd1.close();
	size=count;

	char c2[1024];
	bigint *vals;
	int h=0;
	vals=(mpz_t*)malloc(count*sizeof(mpz_t));
	ifstream rd2(a_sou);
	//copy to bigint array
	while(rd2>>c2){
		mpz_init_set_str(vals[h],c2,10);
		h++;
	}
	rd2.close();
	return vals;

}



bool exits(string a,bigint *b,int size){
	bigint d;
	char*c_con=new char[a.length()];
		strcpy(c_con,a.c_str());
		mpz_init_set_str(d,c_con,10);
	for(int i=0;i<size;i++){

		if(mpz_cmp(d,b[i])==0){return true;break;}
	}
	return  false;
}


bool exits2(bigint a,bigint *b,int size){

	for(int i=0;i<size;i++){

		if(mpz_cmp(a,b[i])==0){return true;break;}
	}
	return  false;
}






//for test onyl.
bigint*  evaluate(bigint* coeff ,int size ,bigint x, bigint pubmoduli){


	bigint*tmp,*tmp2;
	tmp2=(mpz_t*)malloc(1*sizeof(mpz_t));
	mpz_init_set_str(tmp2[0],"0",10);
	tmp=(mpz_t*)malloc(size*sizeof(mpz_t));
	for(int i=0;i<size; i++){
		mpz_init(tmp[i]);
		mpz_powm_ui(tmp[i],x,i,pubmoduli);
		mpz_mul(tmp[i],tmp[i],coeff[i]);
		mpz_add(tmp2[0],tmp[i],tmp2[0]);
	}
	mpz_mod(tmp2[0],tmp2[0],pubmoduli);
	return tmp2;

}
//The end of the test





















int main(){




int xsize=3;//242	//***** Note that the number of x is determined by bucket_max_load
int pub_mod_bitsize=100;
int max_setsize=1;//
int bucket_max_load=1;//185
int table_length=3;//13000
int hash_len=4;


/*
int xsize=15;	//***** Note that the number of x is determined by bucket_max_load
int pub_mod_bitsize=210;
int max_setsize=1000000;//
int bucket_max_load=5;
int table_length=20;
int hash_len=15;

*/

//int elem_bit_size=100;
if(xsize< (2*bucket_max_load)+1) {cout<<"\nxsize must be greater than 2*bucket_max_load)+1, reset it\n";return 0;}





double exp=1;//Number of experiments

double outsource=0;
double auth=0;
double cloudcomp=0;
double clientresultret=0;


int count=1;
for(int l=0;l<exp;l++){
Server serv(xsize,2,pub_mod_bitsize,max_setsize, bucket_max_load,table_length);// generates random xpoints// For now max_setsize make no sense but I left it there as I may find an accurate formula with which the server can set HT parameters
Server * serv_ptr (& serv);


// Assigning random values to two sets a and b.


mpz_t *aa,*bb;
int elem_bit_size=32;
int size_a=1;
int size_b=1;
aa=(mpz_t*)malloc(size_a*sizeof(mpz_t));
bb=(mpz_t*)malloc(size_b*sizeof(mpz_t));
aa=serv.gen_randSet (size_a, elem_bit_size);
bb=serv.gen_randSet (size_b, elem_bit_size);
int interSec_size=1;

cout<<"\n("<<count<<")"<<endl;
cout<<"\nElem in common:"<<endl;







for(int i=0;i< interSec_size;i++){

mpz_set(bb[i],aa[i]);
cout<<"\t\t"<<bb[i]<<endl;


}


cout<<"\n----------------------------------------------------------------\n";
cout<<"\t Set_a_size:       "<<size_a<<endl;
cout<<"\t Set_b_size:       "<<size_b<<endl;
cout<<"\t Pub_mod_bitsize:  "<<pub_mod_bitsize<<endl;
cout<<"\t Bucket_max_load:  "<<bucket_max_load<<endl;
cout<<"\t Table_length:     "<<table_length<<endl;

cout<<"\n----------------------------------------------------------------\n";






















double start000=omp_get_wtime();
Client A(serv_ptr,aa,size_a,hash_len);
string as="A_ID";



A.outsource_poly(as);

double end000=omp_get_wtime();
cout<<"\nOutsource Time :"<<end000-start000<<endl;
outsource+=end000-start000;




/*
//only for test
bigint *x,*y,N;
y=(mpz_t*)malloc(3*sizeof(mpz_t));
x=(mpz_t*)malloc(3*sizeof(mpz_t));
mpz_init_set_str(N,"103",10);


mpz_init_set_str(x[0],"5",10);
mpz_init_set_str(x[1],"8",10);
mpz_init_set_str(x[2],"10",10);

mpz_init_set_str(y[0],"1",10);
mpz_init_set_str(y[1],"4",10);
mpz_init_set_str(y[2],"6",10);

bigint *val=A.interpolate(3,x,y, N);
cout<<"\n\n\n\nOnly for texsting interpolate:"<<endl;
for (int i=0;i<3;i++){
 cout<<"\n val["<<i<<"]:"<<val[i]<<endl;
}

bigint ell;
mpz_init_set_str(ell,"4",10);
bigint *ress=evaluate(val,3 ,ell, N);
cout<<"\n Res:"<<ress[0]<<endl;
*/
// end of the test













Client B(serv_ptr,bb,size_b,hash_len);
string bs="B_ID";

B.outsource_poly(bs);
cout<<"\n----------------"<<endl;


double start5=omp_get_wtime();

double start1=omp_get_wtime();
CompPerm_Request*req=B.gen_compPerm_req();
double end1=omp_get_wtime();
cout<<"\nTime to compute comp_request:"<<end1-start1<<endl;

cout<<"\n----------------"<<endl;

bigint **q;

double start2=omp_get_wtime();
GrantComp_Info*ptr1=A.grant_comp(req,q,true);
double end2=omp_get_wtime();
cout<<"\nTime to  Grant computation:"<<end2-start2<<endl;
auth+=end2-start2;
cout<<"\n----------------"<<endl;

double start3=omp_get_wtime();
Server_Result*res=serv.compute_result(ptr1);
double end3=omp_get_wtime();
cout<<"\nTime to run Serv.comput():"<<end3-start3<<endl;
cloudcomp+=end3-start3;
cout<<"\n----------------"<<endl;
string **values;
int* sz;




double start4=omp_get_wtime();
B.find_intersection(res,sz,q);


cout<<"\n----------------"<<endl;


double end5=omp_get_wtime();
cout<<"\nTotal Computation Time Excluding outsource Time:"<<end5-start5<<endl;
cout<<endl;
cout<<endl;

cout<<"\nExtracting VALID roots\n";
string sou="temproots.txt";
string des="roots.txt";


char *c_sou=new char[sou.length()];
char *c_des=new char[des.length()];

strcpy( c_sou, sou.c_str());
strcpy(c_des,des.c_str());
//double start9=omp_get_wtime();
B.extract(c_sou,c_des);
//double end9=omp_get_wtime();
//cout<<"\nExtraction time:"<<end9-start9<<endl;

double end4=omp_get_wtime();
cout<<"\nTime to find intersection:"<<end4-start4<<endl;
clientresultret+=end4-start4;


count++;




// for test only
bigint lab;
bigint *aaa=encode(aa,1);
cout<<"\nin main= aa[0]:"<<aa[0]<<endl;
A.update(aaa[0],"insertion", lab,as);
// End of the test


}

cout<<endl;
cout<<"\n====================================\n"<<endl;
cout<<"\n\n\nAverage Outsource Time:"<<outsource/exp<<endl;
cout<<"\n\n\nAverage authorization Time:"<<auth/exp<<endl;
cout<<"\n\n\nAverage Cloud-side Computation Time:"<<cloudcomp/exp<<endl;
cout<<"\n\n\nClient side result retieval Time:"<<clientresultret/exp<<endl;




















/*
// Start test1: testing PRS, R_shuffle.

bigint*a,seed,seed2;
mpz_init_set_str(seed,"80446654463",10);
mpz_init_set_str(seed2,"574543",10);
int size=10;

	bigint *ii,*iii;
	ii=(mpz_t*)malloc(size*sizeof(mpz_t));
	iii=(mpz_t*)malloc(size*sizeof(mpz_t));

ii=gen_labels (size, 32,seed);
iii=gen_labels (size, 32,seed2);



bigint**ee;
ee= gen_map (size, 32,  seed,  seed2);



cout<<endl;
	for(int i=0;i<size;i++){
		cout<<"ii["<<i<<"]:"<<ii[i]<<endl;
	}

	cout<<"\n\n-----------------"<<endl;


for(int i=0; i<size; i++){
	cout<<"ee["<<i<<"][0]:"<<ee[i][0]<<endl;
}


	cout<<"\n\n==============="<<endl;

for(int i=0;i<size;i++){
		cout<<"iii["<<i<<"]:"<<iii[i]<<endl;
	}



	cout<<"\n\n-----------------"<<endl;

for(int i=0; i<size; i++){
	cout<<"ee["<<i<<"][1]:"<<ee[i][1]<<endl;
}



mpz_t *aa;
int elem_bit_size=32;


aa=(mpz_t*)malloc(size*sizeof(mpz_t));

aa=gen_randSet (size, elem_bit_size);



ofstream wr("res.txt");
ofstream wr1("res1.txt");
ofstream wr2("aa.txt");
cout<<endl;
for(int i=0;i<size;i++){

//cout<<"aa["<<i<<"]:"<<aa[i]<<endl;
wr2<<aa[i];
wr2<<"\r"<<endl;
}



bigint *rs,*rs1;

rs=PR_shuffle(aa, size, seed);


cout<<"\n------1st-----------"<<endl;

for(int i=0;i<size;i++){

wr<<rs[i];
wr<<"\r"<<endl;

}


rs1=PR_shuffle(aa, size, seed);


cout<<"\n--------2nd---------"<<endl;

for(int i=0;i<size;i++){

wr1<<rs1[i];
wr1<<"\r"<<endl;

//End of test1.

}
*/


cout<<endl;

return 0;

}

//$ g++ -I$home/win7/include  polynomial.o server.o client.o OPSImain.cpp -lgomp  -L/cygdrive/c/cygwin/home/Win7/libpaillier -l:libpaillier.a  -L$home/win7/lib -lntl -lgmpxx -lgmp -lm
