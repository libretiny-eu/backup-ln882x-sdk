# GCC 编译遇到问题

## 链接器无法把库全部放入 **FLASH** 区域
链接脚本 `gcc/ln882x.ld` 中指定的 **FLASH** 容量初始值为 800KB，链接的时候会报错，因为编译
的 qcloud 库文件太大无法放入到 **FLASH** 区域。

## 解决办法
修改链接脚本 `gcc/ln882x.ld` 中 **FLASH** 区域容量为 1200KB，如下

```
/* Linker script to configure memory regions. */
MEMORY
{
  FLASH     (rx)  : ORIGIN = 0x1000A100, LENGTH = 1200K
  RAM0      (rwx) : ORIGIN = 0x1FFE0000, LENGTH = 128K
  RAM1      (rwx) : ORIGIN = 0x20000000, LENGTH = 160K
  RETENTION (rwx) : ORIGIN = 0x20028000, LENGTH = 8K
}
```

## mkimage 工具问题

上面解决了编译生成 `wifi_mcu_qcloud_misc_demo.elf` 文件的问题，但是 **mkimage** 工具在生成 **flashimage.bin** 的时候会出现 **app.bin is too large** 的提示，需要修改 `cfg/flash_partition_cfg.json` 文件，**APP** 分区需要和链接脚本设置的一样大。

最终成功加工生成 **flashimage.bin** 文件，大小约为 1.2MB 字节。

# 运行问题

上述的 **flashimage.bin** 文件烧录到 LN8825B 无法运行，没有日至打印。
