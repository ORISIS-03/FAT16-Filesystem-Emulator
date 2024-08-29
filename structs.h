#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <gtk/gtk.h>

#define MAX_CLUSTERS 65524 // Maximum number of clusters in a FAT16 filesystem
#define FAT_SIZE 16
#define CLUSTER_SIZE (1024 * 4) // 4KB
#define FILE_SIZE (1024 * 10)   // 10KB
#define MAX_FILENAME 50
#define MAX_ENTRIES 512

#define USER 20;

typedef enum
{
    BytesInSector = 512,
    SectorsInCluster = 16,
    NumberOfFatTables = 1,
    RootDirectorySize = 512,
    TotalSectors = (SectorsInCluster * MAX_CLUSTERS),
    FAT16 = 10
} bootsector_constants;

typedef struct
{
    size_t numberOfClusters;
    size_t clusterSize;
    int numberOfFatTables;
    // int rootDirectoryEntries;         // Number of entries in the root directory
    //  int rootDirectorySize;            // Size of the root directory in bytes
    int sectorsPerCluster;
    size_t bytesPersector;
    size_t totalSectors;
    size_t maxClusters;
    char filesystemType[FAT16];
} bootsector;
typedef struct 
{
    char ownerp;
    char groupp;
    char othersp;
}permissions;

typedef struct
{
 
    char filename[MAX_FILENAME];
    // int attributes;
    unsigned short int first_cluster;
    int file_size;
    char creationTime[50];
    char creationDate[50];
    char permissions[30];
} inode; // This is a directory entry struct that contains information about a file

typedef struct
{
    char folder_name[MAX_FILENAME];
    inode Entries[MAX_ENTRIES];
    unsigned short int first_cluster;
    size_t file_size;
} folder; // directory

typedef struct
{
    char data[CLUSTER_SIZE];
} cluster;

void superblock_init(bootsector *superblock)
{
    // initialize the boot sector structure
    memset(superblock, 0, sizeof(bootsector));
    superblock->numberOfClusters = MAX_CLUSTERS;
    superblock->clusterSize = BytesInSector;
    superblock->numberOfClusters = NumberOfFatTables;
    // superblock->root_directory_size = 512;
    superblock->sectorsPerCluster = SectorsInCluster;
    superblock->bytesPersector = BytesInSector;
    superblock->totalSectors = TotalSectors;
    superblock->maxClusters = MAX_CLUSTERS;
    // superblock->reserved_sectors = 1;
    strncpy(superblock->filesystemType, "FAT16", sizeof(superblock->filesystemType));
}

GtkWidget *window;
GtkWidget *fixed;
GtkWidget *button_create;
GtkWidget *btn_delete;
GtkWidget *button_write;
GtkWidget *button_read;
GtkWidget *input;
GtkWidget *data;
GtkWidget *btn_rename;
GtkWidget *btn_createfolder;
GtkWidget *btn_deletefolder;
GtkWidget *output;
GtkWidget *new_name;
