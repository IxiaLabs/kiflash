#include <linux/build-salt.h>
#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("pci:v0000A400d0000903Fsv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00009038sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00009028sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00009018sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00009034sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00009024sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00009014sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00009032sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00009022sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00009012sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00009031sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00009021sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00009011sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00008011sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00008012sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00008014sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00008018sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00008021sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00008022sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00008024sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00008028sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00008031sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00008032sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00008034sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00008038sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00007011sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00007012sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00007014sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00007018sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00007021sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00007022sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00007024sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00007028sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00007031sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00007032sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00007034sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00007038sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00006828sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00006830sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00006928sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00006930sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00006A28sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00006A30sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00006D30sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00004808sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00004828sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00004908sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00004A28sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00004B28sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000A400d00002808sv*sd*bc*sc*i*");

MODULE_INFO(srcversion, "DD086064EDE22CA95301F01");
