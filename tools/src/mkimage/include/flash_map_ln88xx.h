#ifndef __FLASH_MAP_LN88XX_H__
#define __FLASH_MAP_LN88XX_H__

#define SIZE_4KB                    (0x1000)
#define SIZE_32KB                   (8 * SIZE_4KB)
#define SIZE_64KB                   (16 * SIZE_4KB)
#define SIZE_2MB                    (32 * SIZE_64KB)

////-------------------partition---------------------------------////
typedef enum {
    PARTITION_TYPE_APP         = 0u,     /*< application */
    PARTITION_TYPE_OTA         = 1u,     /*< ota temp*/
    PARTITION_TYPE_KV          = 2u,     /*< Key-Value */
    PARTITION_TYPE_NVDS        = 3u,     /*< nvds(NVRAM) */
    PARTITION_TYPE_SIMU_EEPROM = 4u,     /*< simulation eeprom*/
    PARTITION_TYPE_USER        = 5u,     /*< user defined */
    PARTITION_TYPE_PLACE = 0x12345678,   /*< make different IDEs compatible */
} partition_type_t;

typedef struct {
    partition_type_t  type;              /*< what's the type of partition */
    uint32_t          start_addr;        /*< start address */
    uint32_t          size;              /*< partition size */
    uint32_t          crc32;             /*< checksum of the partition info except itself */
} partition_info_t;


////-------------------image header---------------------------------////
typedef enum {
    IMAGE_ATTACHE           = 0u,        /*< it's an attache drvice image */
    IMAGE_TYPE_ORIGINAL     = 1u,        /*< it's an original image */
    IMAGE_TYPE_ORIGINAL_XZ  = 2u,        /*< it's an original image which is zipped */
    IMAGE_TYPE_DIFF         = 3u,        /*< it's a diff image */
    IMAGE_TYPE_DIFF_XZ      = 4u,        /*< it's a diff image which is zipped */
    IMAGE_TYPE_PLACE = 0x12345678,       /*< make different IDEs compatible */
} image_type_t;

typedef struct {
    uint8_t           ver_major;         /*< major version number */
    uint8_t           ver_minor;         /*< minor version number */
} image_ver_t;

typedef struct {
    image_type_t      image_type;        /*< what's the image type */
    image_ver_t       ver;               /*< image version number */
    image_ver_t       ver_diff_depend;   /*< which version of the file that diff image depend on */
    uint32_t          img_size_orig;     /*< size of original image  */
    uint32_t          img_size_orig_xz;  /*< size of original image which is zipped */
    uint32_t          img_size_diff;     /*< size of diff image */
    uint32_t          img_size_diff_xz;  /*< size of diff image which is zippede */
    uint32_t          img_crc32_orig;    /*< checksum of original image  */
    uint32_t          img_crc32_orig_xz; /*< checksum of original image which is zipped */
    uint32_t          img_crc32_diff;    /*< checksum of diff image */
    uint32_t          img_crc32_diff_xz; /*< checksum of diff image which is zippede */
    uint8_t           res[212];          /*< reserved space */
    uint32_t          header_crc32;      /*< checksum of the image header except for itself */
} image_hdr_t;


////-------------------Json Key-Value define---------------------------------////
#define V_BOOT           "BOOT"
#define V_PART_TAB       "PART_TAB"

#define V_KV             "KV"
#define V_SIMU_EEPROM    "SIMU_EEPROM"
#define V_APP            "APP"
#define V_OTA            "OTA"
#define V_NVDS           "NVDS"
#define V_USER           "USER"

#define K_PARTITION_TYPE "partition_type"
#define K_START_ADDR     "start_addr"
#define K_SIZE_KB        "size_KB"






#endif /* __FLASH_MAP_LN88XX_H__ */


