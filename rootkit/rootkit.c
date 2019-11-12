#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>


MODULE_LICENSE("GPL");

<<<<<<< Updated upstream
//
=======
struct CmdMsg {
	char type;
	uint32_t len;
	char* val; //dynamically allocated
};
//out_cmdMsg passed by reference
static void tlv2cmdMsg(int bufLen, char* buf, CmdMsg& out_cmdMsg)
{
	memcpy(out_cmdMsg.type, *buf, 1);
	memcpy(out_cmdMsg.len, *(buf+1), 4);

	out_cmdMsg.val = vmalloc(out_cmdMsg.len); //use vfree(out_cmdMsg.val) later
	memcpy(out_cmdMsg.val, *(buf + 5), out_cmdMsg.len);
}

//returns retBuf; fills in out_bufLen
static char* cmdMsg2tlv(CmdMsg cmdMsg, int& out_bufLen)
{
	char* retBuf = vmalloc(cmdMsg.len+5); //+5 for (4byte)len and (1byte)type

	memcpy(retBuf, &cmdMsg.type, 1);
	memcpy((retBuf + 1), &cmdMsg.len, 4);
	memcpy((retBuf + 5), cmdMsg.val, cmdMsg.len);

	//consider "vfree(out_cmdMsg.val);" here

	return retBuf;
}

>>>>>>> Stashed changes
static int __init rootkit_init(void) {
    printk(KERN_INFO "Loaded kernel module");
    return 0;
}

static void __exit rootkit_exit(void) {
    printk(KERN_INFO "Removed kernel module");
}

module_init(rootkit_init);
module_exit(rootkit_exit);
