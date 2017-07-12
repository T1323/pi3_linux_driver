#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xb344870e, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x5a041bb8, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0x18391aa, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0xfe990052, __VMLINUX_SYMBOL_STR(gpio_free) },
	{ 0xd5f4eaf7, __VMLINUX_SYMBOL_STR(gpiod_direction_output_raw) },
	{ 0x47229b5c, __VMLINUX_SYMBOL_STR(gpio_request) },
	{ 0x1490a67c, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0xf39cacaa, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0x5f3aede0, __VMLINUX_SYMBOL_STR(cdev_del) },
	{ 0x5aa02962, __VMLINUX_SYMBOL_STR(cdev_add) },
	{ 0xaebc5af3, __VMLINUX_SYMBOL_STR(cdev_init) },
	{ 0x7485e15e, __VMLINUX_SYMBOL_STR(unregister_chrdev_region) },
	{ 0x29537c9e, __VMLINUX_SYMBOL_STR(alloc_chrdev_region) },
	{ 0xd62c833f, __VMLINUX_SYMBOL_STR(schedule_timeout) },
	{ 0x566df243, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0xfd3e178c, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0xf4fa543b, __VMLINUX_SYMBOL_STR(arm_copy_to_user) },
	{ 0xfa2a45e, __VMLINUX_SYMBOL_STR(__memzero) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x28cc25db, __VMLINUX_SYMBOL_STR(arm_copy_from_user) },
	{ 0x2e5810c6, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr1) },
	{ 0x64f8b539, __VMLINUX_SYMBOL_STR(gpiod_set_raw_value) },
	{ 0xcd387ee9, __VMLINUX_SYMBOL_STR(gpio_to_desc) },
	{ 0x8e865d3c, __VMLINUX_SYMBOL_STR(arm_delay_ops) },
	{ 0xb1ad28e0, __VMLINUX_SYMBOL_STR(__gnu_mcount_nc) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "D21DD7E0083EA948F8D89C8");
