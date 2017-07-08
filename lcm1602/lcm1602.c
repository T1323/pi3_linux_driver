/*
 * 
 * Driver for lcm1602
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
#include <linux/delay.h>

#include <linux/gpio.h>
#include <linux/io.h>

MODULE_LICENSE ("Dual BSD/GPL");

#define DRIVER_NAME "lcm1602"

static unsigned int lcm_major = 0;
static struct cdev lcm_cdev;
static struct class *lcm_class;

#define PIN_RS	5
#define PIN_RW	6
#define PIN_EN	12
#define PIN_D7	20
#define PIN_D6	26
#define PIN_D5	16
#define PIN_D4  19

static int lcm_open(struct inode *inode, struct file *filp);
static int lcm_close(struct inode *inode, struct file *filp);
static ssize_t lcm_write(struct file *filp, const char *buf, size_t len, loff_t *off);
static loff_t lcm_lseek(struct file *filp, loff_t offset, int whence);

struct lcm_data {
	char msg[1024];
	uint8_t length;
	uint8_t mode;
	int pos;
};

struct file_operations lcm_ops = {
	.owner = THIS_MODULE,
	.open = lcm_open,
	.release = lcm_close,
	.write = lcm_write,
	.llseek = lcm_lseek,
};

static int lcm_init(void)
{
	dev_t dev = MKDEV(lcm_major, 0);
	int alloc_ret = 0;
	int major = 0;
	int cdev_err = 0;
	
	alloc_ret = alloc_chrdev_region(&dev, 0, 1, DRIVER_NAME);
	if (alloc_ret)
		goto error;
	
	lcm_major = major = MAJOR(dev);
	
	cdev_init(&lcm_cdev, &lcm_ops);
	lcm_cdev.owner = THIS_MODULE;
	
	cdev_err= cdev_add(&lcm_cdev, MKDEV(lcm_major, 0), 1);
	if (cdev_err)
		goto error;
	printk(KERN_ALERT "%s (%d, 0) installed\n", DRIVER_NAME, lcm_major);
	
	lcm_class = class_create(THIS_MODULE, DRIVER_NAME);
	if (IS_ERR(lcm_class)) {
		printk(KERN_ALERT "failed in class create\n");
		goto error;
	}
	device_create(lcm_class, NULL, MKDEV(lcm_major, 0), NULL, "lcm_dev0");
	printk(KERN_ALERT " lcmdev0 is created\n");
	
	if (gpio_request(PIN_RS, "RS"))
		printk(KERN_ALERT "request %s failed\n", "RS");
	if (gpio_request(PIN_RW, "Rw"))
		printk(KERN_ALERT "request %s failed\n", "RW");
	if (gpio_request(PIN_EN, "EN"))
		printk(KERN_ALERT "request %s failed\n", "EN");
	if (gpio_request(PIN_D4, "D4"))
		printk(KERN_ALERT "request %s failed\n", "D4");
	if (gpio_request(PIN_D5, "D5")) 
		printk(KERN_ALERT "request %s failed\n", "D5");
	if (gpio_request(PIN_D6, "D6"))
		printk(KERN_ALERT "request %s failed\n", "D6");
	if (gpio_request(PIN_D7, "D7"))
		printk(KERN_ALERT "request %s failed\n", "D7");

	gpio_direction_output(PIN_RS, 0);
	gpio_direction_output(PIN_RW, 0);
	gpio_direction_output(PIN_EN, 0);
	gpio_direction_output(PIN_D4, 0);
	gpio_direction_output(PIN_D5, 0);
	gpio_direction_output(PIN_D6, 0);
	gpio_direction_output(PIN_D7, 0);	

	return 0;

error:
	if (cdev_err)
		cdev_del(&lcm_cdev);
		
	if (alloc_ret)
		unregister_chrdev_region(dev, 1);
		
	return -1;
}

static void lcm_exit(void)
{
	gpio_set_value(PIN_RS, 0);
	gpio_set_value(PIN_RW, 0);
	gpio_set_value(PIN_EN, 0);
	gpio_set_value(PIN_D4, 0);
	gpio_set_value(PIN_D5, 0);
	gpio_set_value(PIN_D6, 0);
	gpio_set_value(PIN_D7, 0);
	
	gpio_free(PIN_RS);
	gpio_free(PIN_RW);
	gpio_free(PIN_EN);
	gpio_free(PIN_D4);
	gpio_free(PIN_D5);
	gpio_free(PIN_D6);
	gpio_free(PIN_D7);
	
	
	device_destroy(lcm_class, MKDEV(lcm_major, 0));
	class_destroy(lcm_class);
	
	cdev_del(&lcm_cdev);
	unregister_chrdev_region(MKDEV(lcm_major, 0), 1);
	printk(KERN_ALERT "%s (%d, 0) uninstalled\n", DRIVER_NAME, lcm_major);
}

static void lcm_set_cmd_4bits(uint8_t byte)
{
	udelay(100);
	gpio_set_value(PIN_RS, 0);
	gpio_set_value(PIN_RW, 0);
	
	gpio_set_value(PIN_D7, (byte & (1 << 7)) ? 1 : 0);
	gpio_set_value(PIN_D6, (byte & (1 << 6)) ? 1 : 0);
	gpio_set_value(PIN_D5, (byte & (1 << 5)) ? 1 : 0);
	gpio_set_value(PIN_D4, (byte & (1 << 4)) ? 1 : 0);
	
	gpio_set_value(PIN_EN, 1);
	
	udelay(100);
	gpio_set_value(PIN_EN, 0);
}

static void lcm_set_cmd(uint8_t byte)
{
	udelay(100);
	gpio_set_value(PIN_RS, 0);
	gpio_set_value(PIN_RW, 0);
	
	gpio_set_value(PIN_D7, (byte & (1 << 7)) ? 1 : 0);
	gpio_set_value(PIN_D6, (byte & (1 << 6)) ? 1 : 0);
	gpio_set_value(PIN_D5, (byte & (1 << 5)) ? 1 : 0);
	gpio_set_value(PIN_D4, (byte & (1 << 4)) ? 1 : 0);
	
	gpio_set_value(PIN_EN, 1);
	
	udelay(100);
	gpio_set_value(PIN_EN, 0);
	
	gpio_set_value(PIN_D7, (byte & (1 << 3)) ? 1 : 0);
	gpio_set_value(PIN_D6, (byte & (1 << 2)) ? 1 : 0);
	gpio_set_value(PIN_D5, (byte & (1 << 1)) ? 1 : 0);
	gpio_set_value(PIN_D4, (byte & (1 << 0)) ? 1 : 0);
	
	gpio_set_value(PIN_EN, 1);
	
	udelay(100);
	gpio_set_value(PIN_EN, 0);
}

static int lcm_set_pos(uint8_t pos)
{
	if (pos >= 32) {
		return -1;
	}
	
	if (pos < 16) {
		lcm_set_cmd(0x80 + pos);
	}
	else {
		lcm_set_cmd(0xC0 + pos - 16);
	}
	
	return 0;
} 

static void lcm_set_word(uint8_t byte)
{
	udelay(100);
	gpio_set_value(PIN_RS, 1);
	gpio_set_value(PIN_RW, 0);
	
	udelay(100);
	gpio_set_value(PIN_D7, (byte & (1 << 7)) ? 1 : 0);
	gpio_set_value(PIN_D6, (byte & (1 << 6)) ? 1 : 0);
	gpio_set_value(PIN_D5, (byte & (1 << 5)) ? 1 : 0);
	gpio_set_value(PIN_D4, (byte & (1 << 4)) ? 1 : 0);
	
	gpio_set_value(PIN_EN, 1);
	
	udelay(100);
	gpio_set_value(PIN_EN, 0);
	
	gpio_set_value(PIN_RS, 1);
	gpio_set_value(PIN_RW, 0);
	
	udelay(100);
	gpio_set_value(PIN_D7, (byte & (1 << 3)) ? 1 : 0);
	gpio_set_value(PIN_D6, (byte & (1 << 2)) ? 1 : 0);
	gpio_set_value(PIN_D5, (byte & (1 << 1)) ? 1 : 0);
	gpio_set_value(PIN_D4, (byte & (1 << 0)) ? 1 : 0);
	
	gpio_set_value(PIN_EN, 1);
	
	udelay(100);
	gpio_set_value(PIN_EN, 0);
}

static void lcm_set_word_pos(uint8_t byte, uint8_t pos)
{
	pos %= 32;
	
	if (lcm_set_pos(pos)) {
		printk(KERN_ALERT "set_pos fail\n");
		lcm_set_pos(0);
	}
	
	lcm_set_word(byte);
}

static int lcm_open(struct inode *inode, struct file *filp)
{
	struct lcm_data *p;
	p = kmalloc(sizeof(struct lcm_data), GFP_KERNEL);
	filp->private_data = p;
	p->mode = 0;
	p->length = 0;
	
	printk(KERN_ALERT "lcm open\n");
	
	lcm_set_cmd_4bits(0x30);
	mdelay(100);
	lcm_set_cmd_4bits(0x30);
	mdelay(6);
	lcm_set_cmd_4bits(0x30);
	mdelay(6);
	
	lcm_set_cmd_4bits(0x20);
	udelay(100);
	lcm_set_cmd(0x28);
	udelay(100);
	lcm_set_cmd(0x0E);
	udelay(100);
	lcm_set_cmd(0x01);
	mdelay(3);
	lcm_set_cmd(0x06);
	udelay(100);
	lcm_set_cmd(0x02);
	mdelay(3);

	lcm_set_word_pos('R', 0);
	lcm_set_word_pos('e', 1);
	lcm_set_word_pos('a', 2);
	lcm_set_word_pos('d', 3);
	lcm_set_word_pos('y', 4);	
	lcm_set_word_pos('!', 5);

	return 0;
}

static int lcm_close(struct inode *inode, struct file *filp)
{
	struct lcm_data *p = filp->private_data;
	
	if (p) {
		kfree(p);
		filp->private_data = NULL;
	}
	
	printk(KERN_ALERT "lcm close\n");
	lcm_set_cmd(0x01);
	mdelay(3);
	lcm_set_cmd(0x02);
	mdelay(3);

	lcm_set_word_pos('G', 0);
	lcm_set_word_pos('o', 1);
	lcm_set_word_pos('o', 2);
	lcm_set_word_pos('d', 3);
	lcm_set_word_pos(' ', 4);
	lcm_set_word_pos('b', 5);
	lcm_set_word_pos('y', 6);
	lcm_set_word_pos('e', 7);
	
	return 0;
}

static void displayMode0(struct file *filp)
{
	int len, i;
	uint8_t offset;
	struct lcm_data *p = filp->private_data;

	printk(KERN_ALERT "display mode 0 %d\n", p->length);	
	
	if (p->length > 32) {
		len = 32;
	}
	else {
		len = p->length;
	}

	if (p->pos < 0) {
		offset = 32 + p->pos;
	}
	else if (p->pos >= 32) {
		offset = p->pos % 32;
	}
	else {
		offset = p->pos;
	}	
	
	lcm_set_cmd(0x01);
	mdelay(3);
		
	for (i=0; i<len; i++)
	{
		lcm_set_word_pos(p->msg[i], (offset + i) % 32);
	}	
}

static ssize_t lcm_write(struct file *filp, const char *buf, size_t len, loff_t *off)
{
	struct lcm_data *p = filp->private_data;
	int res;

	printk(KERN_ALERT "lcm write, off %d\n", (int)*off);
	
	p->length = len;
	res = copy_from_user(p->msg, buf, len);
	p->pos = *off;
	
	switch(p->mode) {
		case 0:
			displayMode0(filp);
		break;
		
		case 1:
			//displayMode1(filp);
		break;
		
		default:
		break;
	}
	
	return len;
}

static loff_t lcm_lseek(struct file *filp, loff_t offset, int whence)
{

	if (whence == SEEK_SET)
	{
		filp->f_pos = offset % 32;
		printk(KERN_ALERT "lcm lseek offset %d\n", (int)filp->f_pos);
		return 0;
	}
	else
	{
		return -1;
	}
}

module_init(lcm_init);
module_exit(lcm_exit);











