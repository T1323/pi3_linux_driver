/*
 * 
 *  My first device driver pratice
 * 
 * 
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <asm/current.h>
#include <asm/uaccess.h>
#include <linux/device.h>

#include <linux/gpio.h>
#include <linux/io.h>

MODULE_LICENSE ("Dual BSD/GPL");

#define DRIVER_NAME	"devone"
#define LED0 22
#define LED1 27

static unsigned int devone_major = 0;
module_param(devone_major, uint, 0);
static struct cdev devone_cdev;
static struct class *devone_class;

static int devone_open(struct inode *inode, struct file *filp);
static int devone_close(struct inode *inode, struct file *filp);
static ssize_t devone_read(struct file *filp, char *buf, size_t len, loff_t *off);
static ssize_t devone_write(struct file *filp, const char *buf, size_t len, loff_t *off);

struct devone_data {
	int val;
};

struct file_operations devone_fops  = {
	.open = devone_open,
	.release = devone_close,
	.read = devone_read,
	.write = devone_write,
};

static int devone_init(void)
{
	dev_t dev= MKDEV(devone_major, 0);
	int alloc_ret = 0;
	int major = 0;
	int cdev_err = 0;

	alloc_ret = alloc_chrdev_region(&dev, 0, 1, DRIVER_NAME);
	if (alloc_ret)
		goto error;
	devone_major = major = MAJOR(dev);
	
	cdev_init(&devone_cdev, &devone_fops);
	devone_cdev.owner = THIS_MODULE;
	
	cdev_err = cdev_add(&devone_cdev, MKDEV(devone_major, 0), 1);
	if (cdev_err)
		goto error;
	
	if (gpio_request(LED0, "LED0")) {
		printk("request %s failed\n", "LED0");
		goto error;
	}
	
	if (gpio_request(LED1, "LED1")) {
		printk("request %s failed\n", "LED1");
		goto error;
	}
	
	gpio_direction_output(LED0, 1);
	gpio_direction_output(LED1, 1);
	
	printk(KERN_ALERT "%s driver (major %d) installed \n", DRIVER_NAME, devone_major);
	
	devone_class = class_create(THIS_MODULE, DRIVER_NAME);
	if(IS_ERR(devone_class)) {
		printk(KERN_ALERT "failed in class create\r");
		goto error;
	}
	
	device_create(devone_class, NULL, MKDEV(devone_major, 0), NULL, "devone0");

	return 0;

error:
	if (cdev_err)
		cdev_del(&devone_cdev);
		
	if (alloc_ret)
		unregister_chrdev_region(dev, 1);
		
	return -1;
}

static void devone_exit(void)
{
	gpio_set_value(LED0, 0);
	gpio_set_value(LED1, 0);
	gpio_free(LED0);
	gpio_free(LED1);
	
	device_destroy(devone_class, MKDEV(devone_major, 0));
	class_destroy(devone_class);
	
	cdev_del(&devone_cdev);
	unregister_chrdev_region(MKDEV(devone_major, 0), 1);
	printk(KERN_ALERT "%s (%d, 0) uninstalled\n", DRIVER_NAME, devone_major);
}

static int devone_open(struct inode *inode, struct file *filp)
{
	struct devone_data *p;
	printk(KERN_ALERT "%s (%d, %d) open\n", __func__, imajor(inode), iminor(inode));
	
	p = kmalloc(sizeof(int), GFP_KERNEL);
	p->val = 0;
	filp->private_data = p;
	
	return 0;
}

static int devone_close(struct inode *inode, struct file *filp)
{
	printk(KERN_ALERT "%s (%d, %d) release\n", __func__, imajor(inode), iminor(inode));
	
	if (filp->private_data) {
		kfree(filp->private_data);
		filp->private_data = NULL;
	}
	
	return 0;
}

static ssize_t devone_read(struct file *filp, char *buf, size_t len, loff_t *off)
{
	int retval, tmp, led0Val, led1Val, i;
	
	printk(KERN_ALERT "%s \n", __func__);
	
	tmp = 0;
	led0Val = gpio_get_value(LED0);
	led1Val = gpio_get_value(LED1);
	tmp = (led1Val << 1) | led0Val;
	
	for (i=0; i<len; i++){
		if (copy_to_user(&buf[i], &tmp, 1)) {
			retval = -EFAULT;
			goto out;
		}
	}
	retval = len;

out:
	return retval;
}

static ssize_t devone_write(struct file *filp, const char *buf, size_t len, loff_t *off)
{
	int retval, tmp;
	struct devone_data *p = filp->private_data;
	
	printk(KERN_ALERT "%s \n",__func__);
	
	if (len >=1){
		if (copy_from_user(&tmp, &buf[0], 1)) {
			retval = -EFAULT;
			goto out;
		}
	}
	
	p->val = tmp;
	gpio_set_value(LED0, (tmp & 0x01));
	gpio_set_value(LED1, ((tmp >> 1) & 0x01));

	retval = len;
out:
	return retval;
}

module_init(devone_init);
module_exit(devone_exit);

















