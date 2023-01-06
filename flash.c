#include<linux/module.h>
#include <linux/init.h>
#include <linux/timer.h>
#include<linux/kernel.h>
#include <linux/cdev.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Javad Rahimi");
MODULE_DESCRIPTION("Kernel Flash char device example");
MODULE_VERSION("0.01");

#define FLASH_MAJOR 233
#define FLASH_MINOR 1
#define FLASH_NAME	"norflash"
#define FLASH_CLASS "flash-class"
static struct flash_cdev 
{
	struct cdev cdev;
	dev_t dev;
	struct class *class;
        dev_t curr_dev;

};

struct flash_cdev flash;
/* File operations */
int flash_open(struct inode *node, struct file *file)
{

	return 0;
}
ssize_t flash_read(struct file *file, char __user *usr, size_t, loff_t *loff)
{
	return 0;
}

ssize_t flash_write(struct file * file, const char __user *usr, size_t, loff_t *loff)
{

	return 0;
}

int flash_release(struct inode *node, struct file *file)
{
	return 0;
}
const struct file_operations flash_ops = {
	.owner = THIS_MODULE,
	.open = flash_open,
	.read = flash_read,
	.write = flash_write,
	.release = flash_release
};
static int __init cflash_init(void)
{
	int ret = 0;
	
	ret = alloc_chrdev_region(&flash.dev, 0, FLASH_MINOR, FLASH_NAME);
	
	if(ret != 0)
	{
		goto error;
	}

	cdev_init(&flash.cdev, &flash_ops);
	flash.cdev.owner = THIS_MODULE;
	flash.curr_dev = MKDEV(MAJOR(flash.dev), MINOR(flash.dev));
	ret = cdev_add(&flash.cdev, flash.curr_dev, 1);
	if(ret != 0)
		goto cdev_err;

	flash.class = class_create(THIS_MODULE, FLASH_CLASS);
	if(IS_ERR(flash.class))
	{
		ret = PTR_ERR(flash.class);
		goto out_chrdev;
	}

	device_create(flash.class, NULL, flash.curr_dev, NULL, "norflsh0");

	return 0;
error:
	pr_err("chrdev alloc error");
	return -1;
cdev_err:
	pr_err("Unable to add char device");
	cdev_del(&flash.cdev);
out_chrdev:
        pr_err("Unable to add char class");

        unregister_chrdev_region(flash.dev, 1);

	return -2;

}
static void cflash_exit(void)
{
	pr_warn("remove the flash device");
	device_destroy(flash.class, MKDEV(MAJOR(flash.dev), MINOR(flash.dev)));
        cdev_del(&flash.cdev);
	class_unregister(flash.class);
	class_destroy(flash.class);
	unregister_chrdev_region(flash.dev, 1);
}
module_init(cflash_init);
module_exit(cflash_exit);
