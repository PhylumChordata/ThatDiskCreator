#ifndef THATDISKCREATOR__FAT_H
#define THATDISKCREATOR__FAT_H


#include "decl.h"
#include <stdint.h>
#include <time.h>


// FAT 32 Extended BIOS Parameter Block
// 512 bytes long
typedef struct {
  uint8_t jump_boot[3]; // fixed value
  uint8_t oem[8];
  uint16_t bytes_per_sector;
  uint8_t sectors_per_cluster;
  uint16_t reserved_sectors_count; // usually set to 32
  uint8_t number_of_fats; // commonly set to 2
  uint16_t root_entries_count; // always set to 0 for FAT32
  uint16_t total_sectors_count; // too small for FAT32. set to 0. use large_total_sectors_count instead.
  uint8_t media_descriptor; // must reflect the media descriptor stored (in the entry for cluster 0) in the first byte of each copy of the FAT
  uint16_t sectors_per_fat; // unused by FAT32. set to 0. use sectors_per_fat_32 instead.
  uint16_t sectors_per_track;
  uint16_t number_of_heads;
  uint32_t hidden_sectors_count;
  uint32_t large_total_sectors_count;

  uint32_t sectors_per_fat_32;
  uint16_t flags; // set to 0 in our use case as the FATs are mirrored
  uint16_t version_of_fat;
  uint32_t root_directory_cluster_number;
  uint16_t fsinfo_sector_number; // usually set to 1
  uint16_t backup_boot_sector_cluster_number; // usually set to 6
  uint8_t reserved_fat_32[12];

  uint8_t drive_number; // usually set to 0x80
  uint8_t reserved;
  uint8_t extended_boot_signature; // usually set to 0x29
  uint32_t serial_number;
  uint8_t volume_label[11];
  uint8_t system_identifier[8];
  uint8_t bootstrap[420];
  uint8_t boot_signature[2];
} __attribute__((packed)) __attribute__((aligned(FAT_32_CLUSTER_SIZE_B))) fat_32_ebpb_t;


// 512 bytes long
typedef struct {
  uint32_t lead_signature; // always set to 0x41615252
  uint8_t reserved_1[480];
  uint32_t struct_signature; // always set to 0x61417272
  uint32_t free_cluster_count; // set to 0xFFFFFFFF for unknown
  uint32_t next_free_cluster; // set to 0xFFFFFFFF for unknown
  uint8_t reserved_2[12];
  uint32_t trail_signature; // always set to 0xAA550000
} __attribute__((packed)) __attribute__((aligned(FAT_32_CLUSTER_SIZE_B))) fat_32_fsinfo_t;


uint32_t get_fat_size(uint32_t size, uint16_t reserved_sectors_count, uint8_t sectors_per_cluster, uint8_t number_of_fats) {
  // http://msdn.microsoft.com/en-us/windows/hardware/gg463080.aspx
  uint32_t magic = (128 * sectors_per_cluster) + number_of_fats / 2;
  uint32_t fat_size = (size - reserved_sectors_count + magic - 1) / magic;
  return fat_size;
}


uint32_t get_serial_number() {
  time_t timestamp = time(NULL);
  struct tm *now = localtime(&timestamp);

  uint16_t top_word_1 = ((now->tm_hour & 0xFF) << 8) | (now->tm_min & 0xFF);
  uint16_t top_word_2 = 1900 + (now->tm_year & 0xFF);

  uint16_t bottom_word_1 = (((now->tm_mon + 1) & 0xFF) << 8) | (now->tm_mday & 0xFF);
  uint16_t bottom_word_2 = (now->tm_sec & 0xFF) << 8;

  return ((top_word_1 + top_word_2) << 16) | (bottom_word_1 + bottom_word_2);
}


#endif // THATDISKCREATOR__FAT_H
