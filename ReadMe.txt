


1- Since byte used for cryptopp was not recognised in mac, it has been modified to CryptoPP::byte. -----27.2.18


2- In this version client only sends one master seed to the server. We achieve this using Key derivation function (KDF) from cryptopp. ----modified in 2017
