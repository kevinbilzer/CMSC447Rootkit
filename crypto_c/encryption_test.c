// C Encryption Unit Tests

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


char* encrypt(const char*, const unsigned int, const char*, const unsigned int);
void print_bytes(const char*, const unsigned int);

void test1();
void test2();
void test3();
//...

int main() {
	
	test1();
	test2();
	test3();

	printf("\n");
	return 0;
}

char* encrypt(const char* msg, const unsigned int msg_len, const char* key, const unsigned int key_len) {
	char* encrypted = (char*)malloc(msg_len + 1);
	memset(encrypted, 0, msg_len + 1);
	for (unsigned int i = 0; i < msg_len; i++) {
		encrypted[i] = msg[i] ^ key[i % key_len];
	}

	return encrypted;
}

void test1() {
	printf("Running Test #1: \n");

	char key[4] = { 0xde, 0xad, 0xbe, 0xef };
	unsigned int key_len = 4;

	char* msg = "blah";
	unsigned int msg_len = 4;

	printf("Encrypting %s\n", msg);

	printf("Encryption: ");
	char* encryption = encrypt(msg, msg_len, key, key_len);

	for (unsigned int i = 0; i < sizeof(encryption); i++) {
		if (i >= 0) printf("\\x");
		printf("%02X", encryption[i]);
	}
	printf("\n");

	printf("Decryption; should be: %s\nActual: ", msg);
	char* decrypted = encrypt(encryption, msg_len, key, key_len);
	printf(decrypted);
	printf("\n");
	printf("\n");
}

void test2() {
	printf("Running Test #2: \n");

	char key[4] = { 0xde, 0xad, 0xbe, 0xef };
	unsigned int key_len = 4;

	char msg[] = { 0x01, 0x07, 0x00, 0x00, 0x00, 0x6c, 0x73, 0x20, 0x2f, 0x74, 0x6d, 0x70 };

	unsigned int msg_len = sizeof(msg);
	
	printf("Encrypting: ");
	print_bytes(msg, msg_len);

	printf("Encryption: ");
	char* encryption = encrypt(msg, msg_len, key, key_len);
	print_bytes(encryption, msg_len);

	printf("Decryption; should be: ");
	print_bytes(msg, msg_len);

	printf("\nActual: ");
	char* decrypted = encrypt(encryption, msg_len, key, key_len);
	print_bytes(decrypted, msg_len);

	printf("\n");
}

// Test input lengths not divisible by 4
void test3() {
	printf("Running Test #3: \n");
	
	char key[4] = { 0xde, 0xad, 0xbe, 0xef };
	unsigned int key_len = 4;

	// Length = 11
	char msg[] = { 0x01, 0x07, 0x00, 0x00, 0x00, 0x6c, 0x73, 0x20, 0x2f, 0x74, 0x6d };

	unsigned int msg_len = sizeof(msg);

	printf("Encrypting: ");
	print_bytes(msg, msg_len);

	printf("Encryption: ");
	char* encryption = encrypt(msg, msg_len, key, key_len);
	print_bytes(encryption, msg_len);

	printf("Decryption; should be: ");
	print_bytes(msg, msg_len);

	printf("\nActual: ");
	char* decrypted = encrypt(encryption, msg_len, key, key_len);
	print_bytes(decrypted, msg_len);

	printf("\n");
}

void print_bytes(const char* msg, const unsigned int msg_len) {
	for (unsigned int i = 0; i < msg_len; i++) {
		if (i >= 0) printf("\\x");
		printf("%02X", msg[i]);
	}
	printf("\n");
}