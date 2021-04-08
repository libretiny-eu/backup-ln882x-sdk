#ifndef __BOOT_TYPE_H__
#define __BOOT_TYPE_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define CRP_VALID_FLAG                  (0x46505243)
#define FLASH_VALID_FLAG                (0x32545241)

typedef void(*entry_point_t)(void);


#pragma pack(4)
typedef union
{
	struct {
		uint32_t        art_flag;
		uint8_t       * target_address;
		uint16_t        ram_header_length;
		uint16_t        length;
		uint32_t        bootram_crc;
		entry_point_t   entry_point;
		uint32_t        crp_value;
	};

	struct {
		uint8_t  * bootram_target_addr; // bootram load addr
		uint16_t   bootram_bin_length;  // Tools build
		uint16_t   bootram_crc_offset;  // >= (sizeof(boot_header_t) + sizeof(boot_header_ext_t))
		uint32_t   bootram_crc_value;   // Tools build (Depends on the bootram_crc_offset.)
		uint32_t * bootram_vector_addr; // bootram vector addr (At least 256 Byte aligned.)
		uint32_t   crp_flag;            // Tools build
		uint32_t   boot_hearder_crc;    // Tools build
	};
} boot_header_t;
#pragma pack()




#endif
