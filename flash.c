#include<linux/module.h>
#include <linux/init.h>
#include <linux/timer.h>
#include<linux/kernel.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/mutex.h>

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
	dev_t dev_num;
	struct class *class;
    	dev_t curr_dev;
	unsigned int size;

};
struct winbod_w25
{
	unsigned int flash_size;
	
};

struct flash_cdev flash;
struct mutex dev_lock;
/* File operations */
int flash_open(struct inode *node, struct file *file)
{
	struct flash_cdev *pflash = NULL;
	pflash = container_of(node->i_cdev, struct flash_cdev, cdev);
	pr_info("Device Size: %d, Majpr: %u, Minor: %u\n",
		pflash->size,	
		imajor(node),
		iminor(node));
	pflash->size = 4096;
	file->private_data = pflash;
	return 0;
}
ssize_t flash_read(struct file *file, char __user *usr, size_t, loff_t *pos)
{
	const char *buf = "Test\r\n";
	int count = 6;
	int sent = copy_to_user(usr,buf, count);
	if (sent)
		return -EFAULT;
	
	*pos += count;
	return count;
}

ssize_t flash_write(struct file * file, const char __user *usr, size_t count, loff_t *pos)
{
	char wbuf[256] = {0};
	if(copy_from_user(wbuf, usr, count)!=0)
	{
		return -EFAULT;
	}
	pr_info("Data to write: %s\n", wbuf);
	*pos += count;
	return count;
}

int flash_release(struct inode *node, struct file *file)
{
	struct flash_cdev *pflash = NULL;
        pflash = container_of(node->i_cdev, struct flash_cdev, cdev);
	mutex_lock(&dev_lock);
	file->private_data = NULL;
	mutex_unlock(&dev_lock);
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
	
	ret = alloc_chrdev_region(&flash.dev_num, 0, FLASH_MINOR, FLASH_NAME);
	
	if(ret != 0)
	{
		goto error;
	}

	cdev_init(&flash.cdev, &flash_ops);
	flash.cdev.owner = THIS_MODULE;
	flash.curr_dev = MKDEV(MAJOR(flash.dev_num), MINOR(flash.dev_num));
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

        unregister_chrdev_region(flash.dev_num, 1);

	return -2;

}
static void cflash_exit(void)
{
	pr_info("remove the flash device");
	device_destroy(flash.class, flash.curr_dev);
    	cdev_del(&flash.cdev);
	class_unregister(flash.class);
	class_destroy(flash.class);
	unregister_chrdev_region(flash.dev_num, 1);
}
module_init(cflash_init);
module_exit(cflash_exit);
