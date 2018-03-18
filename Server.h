#include<iostream>
using namespace std;
#include<string>
#include<gmp.h>
#include <gmpxx.h>
#include<cstring>
#include<iostream>
#include<fstream>
#include <stdlib.h>
#include"Polynomial.h"

/*
#include <cryptopp/sha.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include "cryptopp/cryptlib.h"
#include"cryptopp/hmac.h"
#include"cryptopp/secblock.h"

*/
typedef mpz_t bigint;

struct CompPerm_Request{
	bigint seed;
	string id;
};

struct GrantComp_Info{
	string *id;// the result reciepent id is in id[0].
	//int set_size;
	bigint seed; //seed[0]=Ka to generate a[i],seed[1] to generate Kw1,seed[2] to generate Kw2;
};


struct Server_Result{ 
	bigint ** result;
};


struct Client_Dataset{
	Polynomial*poly;
	bigint *labels;
};



class Server{

public:
	Server();
	Server(int num_xpoints, int dbs_size,int pu_mod_bitsize,int maxSetsize,int NoEl_bucket,int tb_size);

	bigint * get_xpoints (int& size); 
	Server_Result * compute_result (GrantComp_Info * grantComp_info);
	bigint*  send_pubModuli(); 
	void store_poly(Client_Dataset&);

	bigint* get_client_bin(bigint label, string ID);
	int get_maxSetsize();
	int get_NoElem_in_bucket();
	int get_table_size(){return table_size;}
	bigint * gen_randSet (int size,int max_bitsize); 
private:
	Client_Dataset get_db(int index){return db[index];}
	void set_db(int index, Client_Dataset &p);// it adds a new poly to its storage
	//bigint*  gen_publicModuli (int bit_size);
	int find_db_index(string id);
	bigint * xpoints;
	int xpoint_size;
	bigint* pu_moduli;
	Client_Dataset *db;
	int db_size;int count;int table_size;int pu_moduli_bitsize;
	int max_setsize;int NoElem_in_bucket;
};




