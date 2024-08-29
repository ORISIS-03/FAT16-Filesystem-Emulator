// NAME : SONGISO DM MOONO                      SIN: 22107716
// NAME : CALEB NYIRONGO                        SIN: 22108809

#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "structs.h"
#include <gtk/gtk.h>
#include <time.h>

folder *main_folder = NULL; // This is the root directory
uint16_t FAT[FAT_SIZE];
cluster disk_block[FAT_SIZE];

char user[40];
char password[40];

const char *owner = "Admin";
const char *owner_password = "CS225";

void rename_file(const char *old_filename, const char *new_filename);
int check_folderTable(const char *filename);
int free_directory();
int free_cluster();
void create_directory(const char *directory_name);
void delete_directory(const char *directory_name);
void mainfolder_init();
void fatTable_init();
void create_file(const char *filename);
void delete_file(const char *filename);
void write_data(const char *filename, const char *data);
void read_data(const char *filename);

void createFile(GtkWidget *widget, gpointer data)
{
    GtkWidget *entry = (GtkWidget *)data;
    const char *filename = gtk_entry_get_text(GTK_ENTRY(entry));
    create_file(filename);
}
void deleteFile(GtkWidget *widget, gpointer data)
{
    GtkWidget *entry = (GtkWidget *)data;
    const char *filename = gtk_entry_get_text(GTK_ENTRY(entry));
    delete_file(filename);
}

void writeFile(GtkWidget *widget, gpointer data)
{ // write file
    GtkWidget **entries = (GtkWidget **)data;
    const char *filename = gtk_entry_get_text(GTK_ENTRY(entries[0]));
    const char *file_data = gtk_entry_get_text(GTK_ENTRY(entries[1]));
    write_data(filename, file_data);
}

void renameFile(GtkWidget *widget, gpointer data)
{ // rename file
    GtkWidget **entries = (GtkWidget **)data;
    const char *filename = gtk_entry_get_text(GTK_ENTRY(entries[0]));
    const char *new_name = gtk_entry_get_text(GTK_ENTRY(entries[1]));
    write_data(filename, new_name);
}

void readFile(GtkWidget *widget, gpointer data)
{
    GtkWidget *entry = (GtkWidget *)data;
    const char *filename = gtk_entry_get_text(GTK_ENTRY(entry));
    read_data(filename); 
}


void createFolder(GtkWidget *widget, gpointer data)
{
    GtkWidget *entry = (GtkWidget *)data;
    const char *filename = gtk_entry_get_text(GTK_ENTRY(entry));
    create_directory(filename);
}
void deleteFolder(GtkWidget *widget, gpointer data)
{
    GtkWidget *entry = (GtkWidget *)data;
    const char *filename = gtk_entry_get_text(GTK_ENTRY(entry));
    delete_directory(filename);
}

void css()
{
    GtkCssProvider *provider;
    GdkScreen *screen;
    provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
                                    "window {background-color: #1f3749;}"
                                    "button {border-radius: 5px; padding: 10px;}"
                                    "button:hover {background-color: #6d7c85;}"
                                    "*data{padding-top: 0;}",
                                    -1, NULL);
    screen = gdk_screen_get_default();
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(provider);
}

int main(int argc, char *argv[])
{
    printf("\nFAT FILESYSTEM ");
    printf("\n\nUsername: ");
    scanf("%s", user);
    printf("Password: ");
    scanf("%s", password);
    printf("\n");

    if (strcmp(user, owner) == 0)
    {
        if (strcmp(password, owner_password) != 0)
        {
            printf("Invalid password.\n\n");
            exit(EXIT_FAILURE);
        }
    }

    mainfolder_init();
    fatTable_init();

    gtk_init(&argc, &argv);
    css();
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "FILE SYSTEM");
    gtk_window_set_default_size(GTK_WINDOW(window), 550, 600);

    fixed = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(window), fixed);

    input = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(input), "Enter filename");
    gtk_fixed_put(GTK_FIXED(fixed), input, 20, 50);
    gtk_widget_set_size_request(input, 200, 45);

    new_name = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(new_name), "New filename");
    gtk_fixed_put(GTK_FIXED(fixed), new_name, 20, 100);
    gtk_widget_set_size_request(new_name, 200, 45);

    button_create = gtk_button_new_with_label("Create file");
    g_signal_connect(button_create, "clicked", G_CALLBACK(createFile), input);
    gtk_fixed_put(GTK_FIXED(fixed), button_create, 360, 50);
    gtk_widget_set_size_request(button_create, 120, 45);

    btn_delete = gtk_button_new_with_label("Delete file");
    g_signal_connect(btn_delete, "clicked", G_CALLBACK(deleteFile), input);
    gtk_fixed_put(GTK_FIXED(fixed), btn_delete, 360, 100);
    gtk_widget_set_size_request(btn_delete, 120, 45);

    data = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(data), "Text field");
    gtk_fixed_put(GTK_FIXED(fixed), data, 20, 190);
    gtk_widget_set_size_request(data, 250, 200);

    GtkWidget *write_entries[2] = {input, data};
    button_write = gtk_button_new_with_label("Write");
    g_signal_connect(button_write, "clicked", G_CALLBACK(writeFile), write_entries);
    gtk_fixed_put(GTK_FIXED(fixed), button_write, 360, 150);
    gtk_widget_set_size_request(button_write, 120, 45);

    button_read = gtk_button_new_with_label("Read");
    g_signal_connect(button_read, "clicked", G_CALLBACK(readFile), input);
    gtk_fixed_put(GTK_FIXED(fixed), button_read, 360, 200);
    gtk_widget_set_size_request(button_read, 120, 45);

    GtkWidget *file_names[2] = {input, new_name};
    btn_rename = gtk_button_new_with_label("Rename");
    g_signal_connect(btn_rename, "clicked", G_CALLBACK(renameFile), file_names);
    gtk_fixed_put(GTK_FIXED(fixed), btn_rename, 360, 250);
    gtk_widget_set_size_request(btn_rename, 120, 45);

    btn_createfolder = gtk_button_new_with_label("Create folder");
    g_signal_connect(btn_createfolder, "clicked", G_CALLBACK(createFolder), input);
    gtk_fixed_put(GTK_FIXED(fixed), btn_createfolder, 348, 390);
    gtk_widget_set_size_request(btn_createfolder, 120, 45);

    btn_deletefolder = gtk_button_new_with_label("Delete folder");
    g_signal_connect(btn_deletefolder, "clicked", G_CALLBACK(deleteFolder), input);
    gtk_fixed_put(GTK_FIXED(fixed), btn_deletefolder, 348, 440);
    gtk_widget_set_size_request(btn_deletefolder, 120, 45);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);

    gtk_main();

    // free(FAT);
    return 0;
}

void rename_file(const char *old_filename, const char *new_filename)
{
    int index = check_folderTable(old_filename);
    if (index == -1)
    {
        printf("File not found\n");
    }
    else
    {
        main_folder->Entries[index].filename[sizeof(new_filename) - 1] == '\0';
        strncpy(main_folder->Entries[index].filename, new_filename, sizeof(new_filename));
        printf("File renamed sucessfully\n");
    }
}
int check_folderTable(const char *directory_name)
{
    for (int i = 0; i < MAX_CLUSTERS; i++)
    {
        if (strcmp(main_folder->Entries[i].filename, directory_name) == 0)
        {
            return i;
        }
    }
    return -1;
}
int free_directory()
{
    for (int i = 0; i < MAX_CLUSTERS; i++)
    {
        if (main_folder->Entries[i].filename[0] == 0)
        {
            return i;
        }
    }
    return -1;
}
int free_cluster()
{
    for (int i = 0; i < FAT_SIZE; i++)
    {
        if (FAT[i] == 0)
        {
            return i;
        }
    }
    return -1;
}

void create_directory(const char *directory_name)
{
    // Directory root_directory[MAX_ENTRIES];
    clock_t start = clock();
    int index = free_cluster();
    if (index == -1)
    {
        printf("No free clusters available\n");
        return;
    }
    folder subfolder;
    strncpy(main_folder->Entries[index].filename, directory_name, MAX_FILENAME);
    main_folder->Entries[index].filename[MAX_FILENAME - 1] = '\0'; // Null termination
    // new_directory_entry[i].attributes = 0x10; // Directory attribute
    main_folder->Entries[index].first_cluster = index;

    FAT[index] = -1;

    clock_t end = clock();
    double timeTaken = (double)(end - start) / CLOCKS_PER_SEC;
    int speed = (main_folder->Entries[index].file_size / 1024) / timeTaken;

    printf("\nFolder created successfully:\n");
}

// Function to delete a directory (folder)
void delete_directory(const char *directory_name)
{

    int index = check_folderTable(directory_name);
    // inode *entry = &main_folder.Entries[index];
    if (index == -1)
    {
        printf("Folder not found\n");
        return;
    }

    memset(main_folder->Entries[index].filename, 0, sizeof(main_folder->Entries));
    main_folder->Entries[index].first_cluster = 0;
    FAT[index] = 0;

    printf("Folder '%s' deleted successfully\n", directory_name);
}

void mainfolder_init()
{
    // folder **main_folder;
    main_folder = malloc(sizeof(folder) * MAX_ENTRIES);
    // malloc(*main_folder, 0, sizeof(folder) * MAX_CLUSTERS);
    if (main_folder == NULL)
    {
        printf("Memory allocation error for root directory\n");
    }
    // memset(main_folder, 0, sizeof(folder) * MAX_CLUSTERS);
}
void fatTable_init()
{
    memset(FAT, 0, sizeof(int) * FAT_SIZE);
    for (int i = 0; i < FAT_SIZE; i++)
    {
        FAT[i] = 0;
    }
}

// Function to create the file
void create_file(const char *filename)
{

    clock_t start = clock();
    int file_size = FILE_SIZE;
    int index_dir = free_directory();
    time_t seconds = time(NULL);
    struct tm Time = *localtime(&seconds);

    if (index_dir == -1)
    {
        printf("Directory is full");
    }

    int index_cluster = free_cluster(); // First free cluster
    size_t unallocatedFileSize = file_size;
    // allocate clusters
    while (unallocatedFileSize > 0)
    {
        FAT[index_cluster] = -1;
        int next_cluster = free_cluster();
        break;
        if (next_cluster == -1)
        {
            printf("File Allocation Table is full\n");
            exit(-1);
            break;
        }

        // update FAT

        FAT[index_cluster] = next_cluster;
        index_cluster = next_cluster;

        unallocatedFileSize -= CLUSTER_SIZE;
    }

    // update directory
    strncpy(main_folder->Entries[index_dir].filename, filename, MAX_FILENAME);
    main_folder->Entries[index_dir].file_size = FILE_SIZE;
    main_folder->Entries[index_dir].first_cluster = index_cluster;
    snprintf(main_folder->Entries[index_dir].creationTime, sizeof(main_folder->Entries[index_dir].creationTime), "%02d:%02d", Time.tm_hour, Time.tm_min);
    snprintf(main_folder->Entries[index_dir].creationDate, sizeof(main_folder->Entries[index_dir].creationDate), "%04d-%02d-%02d", Time.tm_year + 1900, Time.tm_mon + 1, Time.tm_mday);

    clock_t end = clock();
    double timeTaken = (double)(end - start) / CLOCKS_PER_SEC;
    int speed = (main_folder->Entries[index_dir].file_size / 1024) / timeTaken;
    printf("\nFILE METADATA:\n");
    printf("File name: %s\n", main_folder->Entries[index_dir].filename);

    printf("First cluster: %d\n", main_folder->Entries[index_dir].first_cluster);
    printf("File size: %d\n", main_folder->Entries[index_dir].file_size);
    printf("Creation Time: %s\n", main_folder->Entries[index_dir].creationTime);
    printf("Creation Date: %s\n", main_folder->Entries[index_dir].creationDate);
    printf("\nPERFORMANCE METRICS:\n");
    printf("Operation Start: %ld\n", start);
    printf("Operation End: %ld\n", end);
    printf("Operation time: %f Milliseconds\n", timeTaken * 1000);
    printf("Operation speed: %d MBPS\n\n", speed);

    char buffer[50];
    snprintf(buffer, sizeof(buffer), "%d", main_folder->Entries[index_dir].first_cluster);
}

void delete_file(const char *filename)
{
    clock_t start = clock();
    int index = check_folderTable(filename);
    int current_cluster = main_folder->Entries[index].first_cluster;
    int file_size = FILE_SIZE;
    if (index == -1)
    {
        printf("\nFile not found\n\n");
        return;
    }

    size_t unallocatedFileSize = file_size;
    // deallocate clusters
    int next_cluster;

    while (current_cluster != -1)
    {
        next_cluster = FAT[current_cluster];
        FAT[current_cluster] = 0;
        current_cluster = next_cluster;
    }

    memset(main_folder->Entries[index].filename, 0, sizeof(main_folder->Entries));
    main_folder->Entries[index].first_cluster = 0;
    clock_t end = clock();
    double timeTaken = (double)(end - start) / CLOCKS_PER_SEC;
    int speed = (main_folder->Entries[index].file_size / 1024) / timeTaken;

    printf("\nFile deleted successfully");
    printf("\nPERFORMANCE METRICS:\n");
    printf("Operation Start: %ld\n", start);
    printf("Operation End: %ld\n", end);
    printf("Operation time: %f Milliseconds\n", timeTaken * 1000);
    printf("Operation speed: %d MBPS\n\n", speed);
}

void write_data(const char *filename, const char *data)
{
    if (strcmp(user, owner) == 0)
    {
        clock_t start = clock();
        if (filename == NULL)
        {
            printf("Invalid file\n");
            return;
        }

        int dir_index = check_folderTable(filename);
        if (dir_index == -1)
        {
            printf("File not found\n");
            return;
        }

        int first_cluster = main_folder->Entries[dir_index].first_cluster;
        size_t data_size = strlen(data);
        size_t index = 0;

        if (data_size == 0)
        {
            printf("Data cannot be empty\n");
            return;
        }

        if (first_cluster == -1)
        {
            printf("No free cluster\n");
            return;
        }

        size_t amount;
        int cluster = first_cluster;
        while (index < data_size)
        {
            if ((data_size - index) > CLUSTER_SIZE)
            {
                amount = CLUSTER_SIZE;
            }
            else
            {
                amount = data_size - index;
            }

            memcpy(disk_block[cluster].data, data + index, amount);
            index += amount;

            if (index < data_size)
            {
                int next_cluster = free_cluster();
                if (next_cluster == -1)
                {
                    printf("Error: No free cluster.\n");
                    return;
                }
                FAT[cluster] = next_cluster;
                cluster = next_cluster;
            }
            else
            {
                FAT[cluster] = -1;
                break;
            }
        }
        clock_t end = clock();
        double timeTaken = (double)(end - start) / CLOCKS_PER_SEC;
        int speed = (data_size) / timeTaken;

        main_folder->Entries[dir_index].file_size = data_size;
        printf("Data written to file  successfully.\n");
        printf("\nPERFORMANCE METRICS:\n");
        printf("Operation Start: %ld\n", start);
        printf("Operation End: %ld\n", end);
        printf("Operation time: %f Milliseconds\n", timeTaken * 1000);
        printf("Operation speed: %d KBPS\n\n", speed);
    }
    else
    {
        printf("\nPermissions denied.\n");
    }
}

void read_data(const char *filename)
{
        clock_t start = clock();
        if (filename == NULL)
        {
            printf("Invalid file\n");
            return;
        }
        int dir_index = check_folderTable(filename);
        if (dir_index == -1)
        {
            printf("File not found\n");
            return;
        }

        int cluster = main_folder->Entries[dir_index].first_cluster;
        size_t total_size = main_folder->Entries[dir_index].file_size;
        char *buffer = malloc(total_size+1);
        if (buffer == NULL)
        {
            printf("Failed to allocate memory\n");
            return;
        }

        size_t index = 0;
 while (cluster != -1)
    {
        size_t amount = (total_size - index) > CLUSTER_SIZE ? CLUSTER_SIZE : (total_size - index);
        memcpy(buffer + index, disk_block[cluster].data, amount);
        index += amount;
        cluster = FAT[cluster];
    }

        buffer[index] = '\0';
        clock_t end = clock();
        double timeTaken = (double)(end - start) / CLOCKS_PER_SEC;
        int speed = (sizeof(data) / (1024 * 1024)) / timeTaken;
       
  
   
        printf("File read successfully.\n");
        printf("Information read: %s\n", buffer);
        printf("\nPERFORMANCE METRICS:\n");
        printf("Operation Start: %ld\n", start);
        printf("Operation End: %ld\n", end);
        printf("Operation time: %f Milliseconds\n", timeTaken * 1000);
        printf("Operation speed: %d MBPS\n\n", speed);
      
        return;
        free(buffer);
    
}
