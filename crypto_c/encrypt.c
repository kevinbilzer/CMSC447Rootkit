#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char* encrypt(const char* msg, const unsigned int msg_len, const char* key, const unsigned int key_len) {
	char* encrypted = (char*)malloc(msg_len + 1);
	memset(encrypted, 0, msg_len + 1);
	for (unsigned int i = 0; i < msg_len; i++) {
		encrypted[i] = msg[i] ^ key[i % key_len];
	}

	return encrypted;
}