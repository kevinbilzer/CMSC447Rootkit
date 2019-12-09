#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

static uint8_t ENCRYPTION_KEY[] = {0xde, 0xad, 0xbe, 0xef};
static uint8_t LOWERCASE[] = "abcdefghijklmnopqrstuvwxyz";
static uint8_t UPPERCASE[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

static uint8_t* _crypto(const uint8_t* msg, unsigned int msg_len, const uint8_t* key, unsigned int key_len) {
    unsigned int i;
    uint8_t* encrypted = (uint8_t*) malloc(msg_len + 1);
    if (!encrypted) {
        return NULL;
    }
    memset(encrypted, 0, msg_len + 1);
    for (i = 0; i < msg_len; i++) {
        encrypted[i] = msg[i] ^ key[i % key_len];
    }
    return encrypted;
}

static void _test_crypto(const uint8_t * message, size_t msg_len) {
    printf("Message to be encrypted: %.*s\n", msg_len, message);
    uint8_t * encrypted = _crypto(message, msg_len, ENCRYPTION_KEY, sizeof(ENCRYPTION_KEY));
    uint8_t * decrypted = _crypto(encrypted, msg_len, ENCRYPTION_KEY, sizeof(ENCRYPTION_KEY));
    if (strncmp(message, decrypted, msg_len)) {
        printf("RESULT: FAILURE\n");
    } else {
        printf("RESULT: OK\n");
    }
    free(encrypted);
    free(decrypted);
}

void test_simple() {
    printf("-----test_simple-----\n");
    uint8_t message[] = "deadbeef";
    _test_crypto(message, sizeof(message));
}

void test_unaligned_less() {
    printf("-----test_unaligned_less-----\n");
    uint8_t message[] = "a";
    _test_crypto(message, sizeof(message));
}

void test_unaligned_greater() {
    printf("-----test_unaligned_greater-----\n");
    uint8_t message[] = "deadbeefa";
    _test_crypto(message, sizeof(message));
}

void test_random_string_lowercase() {
    printf("-----test_random_string_lowercase-----\n");
    uint8_t message[16] = {0};
    for (int i = 0; i < 16; i++) {
        message[i] = LOWERCASE[rand() % sizeof(LOWERCASE)];
    }
    _test_crypto(message, sizeof(message));
}

void test_random_string_lowercase_random_length() {
    printf("-----test_random_string_lowercase_random_length-----\n");
    int size = rand() % 100;
    uint8_t * message = calloc(1, size);
    for (int i = 0; i < size; i++) {
        message[i] = LOWERCASE[rand() % sizeof(LOWERCASE)];
    }
    _test_crypto(message, size);
    free(message);
}

void test_random_string_uppercase() {
    printf("-----test_random_string_uppercase-----\n");
    uint8_t message[16] = {0};
    for (int i = 0; i < 16; i++) {
        message[i] = UPPERCASE[rand() % sizeof(UPPERCASE)];
    }
    _test_crypto(message, sizeof(message));
}

void test_random_string_uppercase_random_length() {
    printf("-----test_random_string_uppercase_random_length-----\n");
    int size = rand() % 100;
    uint8_t * message = calloc(1, size);
    for (int i = 0; i < size; i++) {
        message[i] = UPPERCASE[rand() % sizeof(UPPERCASE)];
    }
    _test_crypto(message, size);
    free(message);
}

int main() {
    srand(time(0));
    printf("Select a test to execute from the following menu:\n");
    printf("\t 1. test_simple\n");
    printf("\t 2. test_unaligned_less\n");
    printf("\t 3. test_unaligned_greater\n");
    printf("\t 4. test_random_string_lowercase\n");
    printf("\t 5. test_random_string_uppercase\n");
    printf("\t 6. test_random_string_lowercase_random_length\n");
    printf("\t 7. test_random_string_uppercase_random_length\n");
    printf("\t 8. All tests\n");

    switch(getchar()) {
        case '1':
            test_simple();
            break;
        case '2':
            test_unaligned_less();
            break;
        case '3':
            test_unaligned_greater();
            break;
        case '4':
            test_random_string_lowercase();
            break;
        case '5':
            test_random_string_uppercase();
            break;
        case '6':
            test_random_string_lowercase_random_length();
            break;
        case '7':
            test_random_string_uppercase_random_length();
            break;
        case '8':
            test_simple();
            test_unaligned_less();
            test_unaligned_greater();
            test_random_string_lowercase();
            test_random_string_uppercase();
            test_random_string_lowercase_random_length();
            test_random_string_uppercase_random_length();
            break;
        default:
            break;
    }

    return 0;
}
