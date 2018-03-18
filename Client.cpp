#include"Client.h"

bigint * Client:: update(bigint elem, string updt, bigint &  label, string id){


// gen elem's bin address
	bigint zz;
	bigint*labels;
	bigint zero;
	mpz_init_set_str(zero,"0",10);
	//mpz_init_set_str(one,"1",10);
	bigint *bin;
	bin=(mpz_t*)malloc(xpoint_size*sizeof(mpz_t));
	mpz_init(zz);
	mpz_set_ui(zz,table_size);
	bigint b;
	string s_val;
	CryptoPP::SHA512 hash2;
 	s_val=mpz_get_str(NULL,10,elem);
	unsigned int nDataLen = s_val.length();
	CryptoPP::byte  digest[CryptoPP::SHA512::DIGESTSIZE];
	hash2.CalculateDigest(digest,( CryptoPP::byte*)s_val.c_str(), nDataLen);
	s_val.clear();
       	mpz_init(b);
	mpz_import(b,sizeof(digest),1,sizeof(digest[0]),0,0,digest);
	mpz_mod(b,b,zz);
	int j=mpz_get_ui (b);
// gen bin's label
 	labels=gen_labels (table_size,120,label_key);
	mpz_init_set(label,labels[j]);
// retrieve bin:o_j from the server
	bin=serv->get_client_bin(label,id);

// for text only
/*
cout<<"\nLabel:"<<label<<", bin[0]:"<<bin[0]<<",bin[xpoint_size-1]:"<<bin[xpoint_size-1]<<endl;
*/
// end of the test

// re-gen keys and blinding factors
	bigint *keys;// pseudorandom keys
	keys=(mpz_t*)malloc((j+1)*sizeof(mpz_t));
	gmp_randstate_t rand;
	gmp_randinit_default(rand);
	gmp_randseed(rand,seed);
	for(int i=0;i<=j;i++){
		mpz_init(keys[i]);
		mpz_urandomb(keys[i],rand,115);
}
	gmp_randstate_t rand1;
	gmp_randinit_default(rand1);
//cout<<"\nkeys["<<j<<"]:"<<keys[j]<<endl;
	bigint* bld_factors;
	bld_factors=(mpz_t*)malloc(xpoint_size*sizeof(mpz_t));

	if(counter[j]==0){

		gmp_randseed(rand1,keys[j]);

	}

	else{
		gmp_randstate_t rand2;
		gmp_randinit_default(rand2);
		gmp_randseed(rand2,counter_key);
		bigint *counter_keys;

		counter_keys=(mpz_t*)malloc((j+1)*sizeof(mpz_t));
		for(int i=0;i<=j;i++){
			mpz_init(counter_keys[i]);
			mpz_urandomb(counter_keys[i],rand2,115);
		}

		bigint tmp_key;
		gmp_randstate_t rand3;
		gmp_randinit_default(rand3);
		gmp_randseed(rand3,counter_keys[j]);
		for(int k=0;k<counter[j];k++){
			mpz_init(tmp_key);
			mpz_urandomb(tmp_key,rand3,115);
		}
		bigint tmp2;
		mpz_init(tmp2);
		mpz_add(tmp2,keys[j],tmp_key);
		gmp_randseed(rand1,tmp2);
	}
	for(int i=0;i<xpoint_size;i++){
mpz_init(bld_factors[i]);
			mpz_urandomb(bld_factors[i],rand1,115);
		}
	bigint *un_bl;
	un_bl=(mpz_t*)malloc(xpoint_size*sizeof(mpz_t));
// remove the blinding factors
	for(int k=0;k<xpoint_size;k++){
				mpz_init(un_bl[k]);
				mpz_sub(un_bl[k],pubmoduli,bld_factors[k]);
				mpz_add(un_bl[k],un_bl[k],bin[k]);
			}
//interpolate a poly
	bigint *values=interpolate(xpoint_size,xpoints,un_bl,pubmoduli);
//increment counter [j]
	counter[j]+=1;
//check if \phi(value)=0;
	Polynomial pl;

	bigint*eval=pl.evaluate_coeff(values,xpoint_size , elem, pubmoduli);

	if(updt=="insertion"){
		if (mpz_cmp(eval[0],zero)!=0){
			//find the roots
			cout<<"\nin find roots-- "<<endl;
			bigint*roots= find_roots(values, xpoint_size,  pubmoduli);
		}



		if (mpz_cmp(eval[0],zero)==0){

		}

	}


}




bigint* Client:: file_to_bigint(char* a_sou,int & size){

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






bigint * Client::find_roots(bigint* eval, int size, bigint  pubmoduli){
	char * tmp_mod = mpz_get_str(NULL,10,pubmoduli);
	ZZ p=to_ZZ(tmp_mod);
	ZZ_p::init(p);
	ZZ_pX P;
	ZZ one(1);
	for(int j=0;j<size;j++){
		char * tmp = mpz_get_str(NULL,10,eval[j]);
		ZZ_p dd=to_ZZ_p(conv<ZZ> (tmp));
		SetCoeff(P,j,dd);
	}
	ZZ_p a=LeadCoeff(P);
	ZZ aa=rep(a);
	if(aa>one){ MakeMonic(P);}
	Vec< Pair< ZZ_pX, long > > factors;
	CanZass(factors, P);
			//cout<<"factors"<<factors<< " ------ " <<factors.length()<<endl;
	vec_ZZ_p root;
	ofstream wr("temproots.txt");
	for(int j=0;j<factors.length() ;j++){
		if(factors[j].a.rep.length()==2){
			root=FindRoots(factors[j].a);
			for(int k=0;k<root.length();k++){
				wr<<root[k];
				wr<<"\r"<<endl;
			}
		}
	}
	wr.close();
	string sou="temproots.txt";
	string des="roots.txt";
	char *c_sou=new char[sou.length()];
	char *c_des=new char[des.length()];
	strcpy( c_sou, sou.c_str());
	strcpy(c_des,des.c_str());
	extract(c_sou,c_des);


	bigint *res;
	int size_a;
	res=file_to_bigint(c_des,size_a);
	for(int i=0;i<size_a; i++){

		cout<<"\nres[i]:"<<res[i]<<endl;
	}

	return res;




}
//bigint * find_roots(bigint* eval, int size, bigint  pubmoduli);



/*
//find the roots
	ofstream wr("temproots2.txt");
	for(int j=0;j<xpoint_size;j++){
		char * tmp = mpz_get_str(NULL,10,values[j]);
		ZZ_p dd=to_ZZ_p(conv<ZZ> (tmp));
		SetCoeff(P,j,dd);
	}
		ZZ_p a=LeadCoeff(P);
		ZZ aa=rep(a);
		if(aa>one){ MakeMonic(P);}
		Vec< Pair< ZZ_pX, long > > factors;
		CanZass(factors, P);
			//cout<<"factors"<<factors<< " ------ " <<factors.length()<<endl;
		vec_ZZ_p root;
			for(int j=0;j<factors.length() ;j++){
				if(factors[j].a.rep.length()==2){
					root=FindRoots(factors[j].a);
					for(int k=0;k<root.length();k++){

						wr<<root[k];
						wr<<"\r"<<endl;
					}
				}
			}
	wr.close();

*/


















bigint* Client::gen_labels (int size, int max_bitsize, bigint seed){

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


bigint** Client::gen_map (int size, int max_bitsize, bigint seed1, bigint seed2){

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
		mpz_init(labels[i][0]);
		mpz_init(labels[i][1]);
		mpz_urandomb(labels[i][0],rand1,max_bitsize);// The last argument is in bit
		mpz_urandomb(labels[i][1],rand2,max_bitsize);
	}


// the label pairs must be randomly permuted.

	labels2=R_shuffle(labels,  size);
	return labels2;
}







bigint* Client::encode(bigint* a,int a_size){


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
		hash2.CalculateDigest(digest,( CryptoPP::byte*)s_val.c_str(), s_val.length());
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



bigint*  Client::PR_shuffle(bigint*elem, int size, bigint seed){

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
		mpz_urandomb(temp,rand,115);
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




Polynomial*  Client::PR_shuffle(Polynomial*pol, int size, bigint seed){

	Polynomial *ply_tmp,buf;
	ply_tmp=new Polynomial[size];

	for(int i=0;i<size;i++){
		ply_tmp[i]=pol[i];
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
		mpz_urandomb(temp,rand,115);
		mpz_set_ui(big_j,j);
		mpz_mod(temp,temp,big_j);
		indx=mpz_get_ui(temp);

		buf=ply_tmp[j];
		ply_tmp[j]=ply_tmp[indx];
		ply_tmp[indx]=buf;
	}
	return ply_tmp;
}






bigint**  Client:: R_shuffle(bigint**elem, int size){

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
		mpz_urandomb(temp,rand,115);
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







bigint ** Client::decode(bigint *a,int a_size){

	bigint** res;
	string s_val;
	res= (mpz_t**)malloc(a_size*sizeof(mpz_t));
	string bin_enc;
	for(int i=0;i<a_size;i++){
		res[i]=(mpz_t*)malloc(2*sizeof(mpz_t));
// converts encoded value to bit string
		bin_enc=mpz_get_str(NULL, 2, a[i]);
// extracts element
		s_val=bin_enc.substr(0,bin_enc.length()-hash_length);//outputs [0,bin_enc.length()-hash_output_size)
// converts it to bigint
		char c_val[s_val.length()];
		strcpy(c_val,s_val.c_str());
		mpz_init_set_str(res[i][0],c_val,2);
// extracts hash value (the remaining part of encoded value)
		string s_hash=bin_enc.substr(bin_enc.length()-hash_length,bin_enc.length());
// converts it to bigint
		char c_hash[s_hash.length()];
		strcpy(c_hash,s_hash.c_str());
		mpz_init_set_str(res[i][1],c_hash,2);
		s_val.clear();
		bin_enc.clear();
	}
bin_enc.clear();
return res;

}



void Client::extract(char* source,char* destination){

	int count=0;
	bigint** dec;
	int h=0;
	ifstream rd1(source);
	//bigint temp;
// counts the number of elements from source file (a file contaiing roots and some errors)
	char c1[1024];
	//count number of elem
	while(rd1>>c1){
		count++;
	}
	rd1.close();
	char c2[1024];
	bigint *vals;
	vals=(mpz_t*)malloc(count*sizeof(mpz_t));
	ifstream rd2(source);
	//copy to bigint array
// fetch the elements from the file and store them in an array 	of big int
	while(rd2>>c2){
		mpz_init_set_str(vals[h],c2,10);
		h++;
	}
	rd2.close();
 	ofstream wr(destination);
// separates the last len bits of encoded value and put the last len-bit in dec[i][1] and the rest of that in dec[i][0]
	dec=decode(vals, count);
// stores those value that hash(dec[i][1])=dec[i][0]
	for(int i=0;i<count;i++){
		if(verify(dec[i])){// veryfies whether hash(dec[i][0])=dec[i][1]
			wr<<dec[i][0];
			wr<<"\r"<<endl;
			mpz_clear(dec[i][0]);
		}
	}
	wr.close();
}


// it checks whether hash(a[1])=a[0]; if yes return true
bool Client::verify(bigint* a){

	bigint hash;
	CryptoPP::SHA1 hash1;
// converts bigint to string
	string	s_val=mpz_get_str(NULL, 10,a[0]);
// compute the value's hash
	CryptoPP::byte  digest[CryptoPP::SHA1::DIGESTSIZE];
	hash1.CalculateDigest(digest,( CryptoPP::byte*)s_val.c_str(), s_val.length());
	s_val.clear();
	mpz_init(hash);
// converts hash to bigint and fixed its size to 160
	mpz_import(hash,sizeof(digest),1,sizeof(digest[0]),0,0,digest);
	if(hash_length<160){
		string bin_hash=mpz_get_str(NULL,2,hash);
		string tmpk;
		if(bin_hash.length() < 160){
			int dif=160-bin_hash.length();
			for (int j=0;j<dif;j++){tmpk+="0";}
		}

		bin_hash=tmpk+bin_hash;
// extract the required len of hash (if its smaller than 160)
		bin_hash=bin_hash.substr(0,hash_length);
		char c[bin_hash.length()];
		strcpy(c,bin_hash.c_str());
		mpz_init_set_str(hash,c,2);
		bin_hash.clear();
		tmpk.clear();
	}
// checks whether h(a[1])=a[0]
	if(mpz_cmp(hash,a[1])==0){mpz_clear(hash); return true;}
	else {mpz_clear(hash);return false;}
}


Client::Client(){}

Client::Client(Server*server, bigint *elemenets, int el_size, int hash_len){

	hash_length=hash_len;
// encode set element
	elem=(mpz_t*)malloc(el_size*sizeof(mpz_t));
	elem=encode(elemenets,el_size);
	serv=server;
	//keep size;
	elem_size=el_size;
	//seed is generated
	Random rd;
	gmp_randstate_t rand,rand2,rand3,rand4;
	bigint ran;
	rd.init_rand3(rand, ran, 8);
	mpz_init_set(seed, ran);
mpz_init(ran);
	rd.init_rand3(rand2, ran, 8);
	mpz_init_set(counter_key, ran);
mpz_init(ran);
	rd.init_rand3(rand3, ran, 8);
	mpz_init_set(label_key, ran);
mpz_init(ran);
	rd.init_rand3(rand4, ran, 8);
	mpz_init_set(shuffle_key, ran);

	int size;
	get_xpoints(size);
	xpoint_size=size;
	get_pubModuli();
	get_NoElem_in_bucket();
	get_tablesize();
	//set the counter
	counter=new int [table_size];
	for(int i=0;i<table_size;i++){
		counter[i]=0;
	}
}


void Client::get_NoElem_in_bucket(){
	NoElem_in_bucket=serv->get_NoElem_in_bucket();
}

void Client::get_xpoints(int&size){
	xpoints=serv->get_xpoints (size);
	xpoint_size=size;
}

void Client::get_pubModuli(){
	bigint *ptr=(mpz_t*)malloc(1*sizeof(mpz_t));
	ptr=serv->send_pubModuli();
	mpz_init_set(pubmoduli, ptr[0]);
}

void Client::get_tablesize(){
	table_size=serv->get_table_size();
}


void Client::outsource_poly(string & poly_ID){
	Client_Dataset db;
 // contructs a hash table
	Hashtable HT(NoElem_in_bucket, elem, elem_size,table_size);
	bigint minus_one;
	mpz_init_set_str(minus_one,"-1",10);
	Polynomial *poly;
	poly=new Polynomial [table_size];
	outpoly_ID=poly_ID;
	gmp_randstate_t rand2;
	gmp_randinit_default(rand2);
	gmp_randseed(rand2,seed);
	bigint Der_pass;//derived seed. So seed is the master key to generate the other seeds
	mpz_init(Der_pass);
// for every index in hash table contruct a polynomial (in poly is decided whether dummy values shuold be used
	for(int i=0;i<table_size;i++){
		Polynomial pol(HT.get_bucket(i), poly_ID, xpoints,NoElem_in_bucket, xpoint_size,pubmoduli);
		poly[i]=pol;
// assign a seed to every index of HT.	Each seed is used to blind corresponding poly.
		mpz_urandomb(Der_pass,rand2,115);

cout<<"\n Der_pass"<<Der_pass<<endl;
// blind every poly.
		poly[i].blind_poly(Der_pass,pubmoduli);

        }

	bigint *labels;
	labels=(mpz_t*)malloc(table_size*sizeof(mpz_t));
 	labels=gen_labels (table_size, 120,  label_key);

	db.labels=PR_shuffle(labels, table_size, shuffle_key);
	db.poly=PR_shuffle(poly, table_size, shuffle_key);



// only for test
/*
bigint *tt;
tt=(mpz_t*)malloc (xpoint_size*sizeof(mpz_t));
for(int i=0;i<table_size;i++){

tt=db.poly[i].get_values();
cout<<"\n Labels["<<i<<"]:"<<db.labels[i]<<", tt[0]["<<i<<"]:"<<tt[0]<<" , tt[xpoint_size-1]["<<i<<"]:"<<tt[xpoint_size-1]<<endl;

}
*/
// end of the test
	serv->store_poly(db);
}




CompPerm_Request * Client::gen_compPerm_req(){

	CompPerm_Request* ptr;
	ptr=new CompPerm_Request;
	mpz_init_set(ptr->seed,seed);
	ptr->id=outpoly_ID;
	return ptr;
}







GrantComp_Info * Client::grant_comp(CompPerm_Request * com_req,bigint **&qq, bool accept){
	GrantComp_Info * ptr;
	ptr=new GrantComp_Info;
		if(!accept){
	ptr=NULL;
	return ptr;}
	bigint **passW,*a,*Sw1,*Sw2,*z_A,*z_B,**q;
	bigint*PassWord_A,*PassWord_B,*PassWord_C;
	// the seeds s_k in PassWord_A, is used to re-generate z^A_i for bucket k
	PassWord_A=(mpz_t*)malloc(table_size*sizeof(mpz_t));
	PassWord_B=(mpz_t*)malloc(table_size*sizeof(mpz_t));
	PassWord_C=(mpz_t*)malloc(table_size*sizeof(mpz_t));
	gmp_randstate_t randA,randB,randC,rand_Pas_C;
	gmp_randinit_default(randA);
	gmp_randinit_default(randB);
	gmp_randinit_default(randC);
	gmp_randinit_default(rand_Pas_C);
	gmp_randseed(randA,seed);
	gmp_randseed(randB,com_req->seed);
	q=(mpz_t**)malloc(table_size*sizeof(mpz_t));
	passW=(mpz_t**)malloc(table_size*sizeof(mpz_t));

	z_A=(mpz_t*)malloc(xpoint_size*sizeof(mpz_t));
	z_B=(mpz_t*)malloc(xpoint_size*sizeof(mpz_t));
	a=(mpz_t*)malloc(xpoint_size*sizeof(mpz_t));
	Sw1=(mpz_t*)malloc(NoElem_in_bucket*sizeof(mpz_t));
	Sw2=(mpz_t*)malloc(NoElem_in_bucket*sizeof(mpz_t));

	gmp_randstate_t rand00,rand01,rand02,rand03,rand04,rand05;
	gmp_randinit_default(rand00);
	gmp_randinit_default(rand01);
	gmp_randinit_default(rand02);
	gmp_randinit_default(rand03);
	gmp_randinit_default(rand04);

	bigint ran1;
	string w_1;
	string w_2;
	Random rd;
	for(int i=0;i<xpoint_size;i++){
		mpz_init(z_A[i]);
		mpz_init(a[i]);
		mpz_init(z_B[i]);
	}
	for(int j=0;j<NoElem_in_bucket;j++){
		mpz_init(Sw1[j]);
		mpz_init(Sw2[j]);
	}


	gmp_randstate_t rand;
	bigint ran;
	rd.init_rand3(rand,ran,8);// generate a fresh master seed. ran->seed contains a fresh seed

	gmp_randseed(randC,ran);//************************** Client A need to send only ran->seed to the server. XXXXX Apply the change



	for(int i=0;i<table_size;i++){
		q[i]=(mpz_t*)malloc(xpoint_size*sizeof(mpz_t));

		mpz_init(PassWord_A[i]);
		mpz_init(PassWord_B[i]);
		mpz_init(PassWord_C[i]);
		mpz_urandomb(PassWord_A[i],randA,115);
		mpz_urandomb(PassWord_B[i],randB,115);
		mpz_urandomb(PassWord_C[i],randC,115);
		gmp_randseed(rand_Pas_C,PassWord_C[i]);
		passW[i]=(mpz_t*)malloc(3*sizeof(mpz_t));

		for(int j=0;j<3;j++){// we need three seeds for each bucket.
			//1:to geenrate a[i] 2,3:to generate sw1 and sw2
			mpz_init(passW[i][j]);
			mpz_urandomb(passW[i][j],rand_Pas_C,115);
		}
		//re-gen seed for each bucket of client A
		gmp_randseed(rand00,PassWord_A[i]);

		//re-gen seed for each bucket of client B

		gmp_randseed(rand01,PassWord_B[i]);

		gmp_randseed(rand02,passW[i][0]);
		//gen sw1  // picks a set of pr-values

		gmp_randseed(rand03,passW[i][1]);
		//gensw2
		gmp_randseed(rand04,passW[i][2]);

		for(int j=0;j<NoElem_in_bucket;j++){
			mpz_urandomb(Sw1[j],rand03,115);
			mpz_urandomb(Sw2[j],rand04,115);}

			//gen w1
			Polynomial w1(Sw1,w_1,xpoints,NoElem_in_bucket,xpoint_size,pubmoduli);
			//gen w2
			Polynomial w2(Sw2,w_2,xpoints,NoElem_in_bucket,xpoint_size,pubmoduli);
			//compute q[i]
			bigint* temp_w1=w1.get_values();
			bigint* temp_w2=w2.get_values();
			for(int j=0;j<xpoint_size;j++){

				mpz_urandomb(z_A[j],rand00,115);
				mpz_urandomb(a[j],rand02,115);
				mpz_urandomb(z_B[j],rand01,115);
				mpz_init(q[i][j]);
				mpz_mul(z_A[j],z_A[j],temp_w1[j]);
				mpz_mul(z_B[j],z_B[j],temp_w2[j]);
				mpz_add(z_A[j],z_A[j],z_B[j]);
				mpz_add(q[i][j],z_A[j],a[j]);
				mpz_mod(q[i][j],q[i][j],pubmoduli);
			}
	}

	// it needs to compute (1) qi  (2) store the three seeds in GrantComp_Info (3) store
	qq=q;
	mpz_init_set(ptr->seed,ran);
	ptr->id=new string[2];
	ptr->id[0]=com_req->id;
	ptr->id[1]=outpoly_ID;
	return ptr;
}


void Client::find_intersection(Server_Result * res,int*& size,bigint**q){

	bigint *un_bl;
	char * tmp_mod = mpz_get_str(NULL,10,pubmoduli);
	ZZ p=to_ZZ(tmp_mod);
	ZZ_p::init(p);
	ZZ_pX P;
	un_bl=(mpz_t*)malloc(xpoint_size*sizeof(mpz_t));
	ZZ one(1);
	ofstream wr("temproots.txt");
	for(int i=0;i<table_size;i++){

			// removes the blinding factors

				for(int j=0;j<xpoint_size;j++){
				mpz_init(un_bl[j]);
				mpz_sub(un_bl[j],pubmoduli,q[i][j]);
				mpz_add(un_bl[j],un_bl[j],res->result[i][j]);
				//mpz_mod(un_bl[j],un_bl[j],pubmoduli);
			}
			//interpolate
			bigint *values=interpolate(xpoint_size,xpoints,un_bl,pubmoduli);
			//find the roots
			for(int j=0;j<xpoint_size;j++){
			char * tmp = mpz_get_str(NULL,10,values[j]);
			ZZ_p dd=to_ZZ_p(conv<ZZ> (tmp));
			SetCoeff(P,j,dd);
			}
			ZZ_p a=LeadCoeff(P);
			ZZ aa=rep(a);
			if(aa>one){ MakeMonic(P);}


			Vec< Pair< ZZ_pX, long > > factors;
			CanZass(factors, P);
			//cout<<"factors"<<factors<< " ------ " <<factors.length()<<endl;
			vec_ZZ_p root;
			for(int j=0;j<factors.length() ;j++){
				if(factors[j].a.rep.length()==2){
					root=FindRoots(factors[j].a);
					for(int k=0;k<root.length();k++){

						wr<<root[k];
						wr<<"\r"<<endl;
					}
				}
			}
	}
	wr.close();

}



bigint* Client::interpolate(int size, bigint* a, bigint* b,bigint N)// a:x, b:y-coordinate
{
   long m = size;
   bigint* prod;
      prod=(mpz_t*)malloc(size*sizeof(mpz_t));
	  for(int i=0;i<size;i++){
		  mpz_init_set(prod[i],a[i]);
	  }
   //prod = a;
   bigint t1, t2;
   mpz_init(t1);
   mpz_init(t2);
   int k, i;
   bigint* res;
   res=(mpz_t*)malloc(size*sizeof(mpz_t));
	   bigint aa;
   for (k = 0; k < m; k++) {
      mpz_init_set(aa ,a[k]);
      mpz_init_set_str(t1,"1",10);
      for (i = k-1; i >= 0; i--) {
         mpz_mul(t1, t1, aa);
		 mpz_mod(t1, t1,N);
         mpz_add(t1, t1, prod[i]);
      }
      mpz_init_set_str(t2,"0",10);
      for (i = k-1; i >= 0; i--) {
         mpz_mul(t2, t2, aa);
		 mpz_mod(t2, t2,N);
         mpz_add(t2, t2, res[i]);
      }
      mpz_invert(t1, t1,N);
      mpz_sub(t2, b[k], t2);
      mpz_mul(t1, t1, t2);

      for (i = 0; i < k; i++) {
         mpz_mul(t2, prod[i], t1);
		 mpz_mod(t2, t2,N);

         mpz_add(res[i], res[i], t2);
		 mpz_mod(res[i], res[i],N);
      }
      mpz_init_set(res[k], t1);
	  mpz_mod(res[k], res[k],N);
      if (k < m-1) {
         if (k == 0)
            mpz_neg(prod[0], prod[0]);
         else {
            mpz_neg(t1, a[k]);
            mpz_add(prod[k], t1, prod[k-1]);
            for (i = k-1; i >= 1; i--) {
               mpz_mul(t2, prod[i], t1);
			   mpz_mod(t2, t2,N);
               mpz_add(prod[i], t2, prod[i-1]);
            }
            mpz_mul(prod[0], prod[0], t1);
			mpz_mod(prod[0], prod[0],N);
         }
      }
   }

   while (m > 0 && (res[m-1]==0)) m--;

   mpz_clear(t1);
   mpz_clear(t2);
   return res;
}
