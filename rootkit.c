#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Robert W. Oliver II");
MODULE_DESCRIPTION("A simple example Linux module.");
MODULE_VERSION("0.01");


static int __init rootkit_init(void) {
    printk(KERN_INFO "Loaded kernel module");
    return 0;
}

static void __exit rootkit_exit(void) {
    printk(KERN_INFO "Removed kernel module");
}

module_init(rootkit_init);
module_exit(rootkit_exit);
