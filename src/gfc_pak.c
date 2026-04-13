#include "physfs.h"
#include "simple_logger.h"
#include "simple_json_parse.h"
#include "gfc_decode.h"
#include "gfc_text.h"
#include "gfc_list.h"
#include "gfc_pak.h"

static Uint8 GFC_PAK_INIT = 0;

int gfc_pak_initialized()
{
    return GFC_PAK_INIT;
}

void gfc_pak_manager_close()
{
    PHYSFS_deinit();
    GFC_PAK_INIT = 0;
}

void gfc_pak_manager_init()
{
    if (!PHYSFS_init(NULL))
    {
        slog("failed to initialize physics fs re: %i",PHYSFS_getLastErrorCode());
        return;
    }
    if (!PHYSFS_mount("./", NULL,0))
    {
        slog("failed to initialize physics fs re: %i",PHYSFS_getLastErrorCode());
        PHYSFS_deinit();
        return;
    }
    GFC_PAK_INIT = 1;
    atexit(gfc_pak_manager_close);
}

void gfc_pak_manager_add(const char *filename)
{
    if (!filename)
    {
        slog("gfc_pak_manager_add: no filename provided");
        return;
    }
    if (!PHYSFS_mount(filename, NULL,1))
    {
        slog("gfc_pak_manager_add: failed to load %s, error code: %i",filename, PHYSFS_getLastErrorCode());
    }
}

void *gfc_pak_load_file_from_disk(const char *filename,size_t *fileSize)
{
    long size;
    void *data;
    FILE *file;
    if (!filename)return NULL;
    file = fopen(filename,"r+b");
    if (!file)return NULL;
    size = get_file_Size(file);
    if (!size)
    {
        slog("file %s is empty",filename);
        fclose(file);
        return NULL;
    }
    data = gfc_allocate_array(size + 1,1);
    if (!data)
    {
        fclose(file);
        return NULL;
    }
    fread(data, size, 1, file);
    if (fileSize)
    {
        *fileSize = size;
    }
    return data;
}

void *gfc_pak_file_extract(const char *filename,size_t *fileSize)
{
    char *buffer= NULL;
    PHYSFS_file* file;
    PHYSFS_sint64 file_size = 0;
    if (!gfc_pak_initialized())
    {
        return gfc_pak_load_file_from_disk(filename,fileSize);
    }
    file = PHYSFS_openRead(filename);
    if (!file)
    {
        slog("failed to open file: %s",filename);
        return NULL;
    }
    file_size = PHYSFS_fileLength(file);
    if (file_size <= 0)
    {
        slog("failed to read file: %s",filename);
        PHYSFS_close(file);
        return NULL;
    }
    file_size ++;//padding for terminal
    buffer = gfc_allocate_array(sizeof(char),file_size);
    if (!buffer)
    {
        slog("failed to allocated memory for file: %s",filename);
        PHYSFS_close(file);
        return NULL;
    }
    PHYSFS_readBytes (file, buffer,file_size);
    PHYSFS_close(file);
    if (fileSize)*fileSize = file_size;
    return buffer;
}

SJson *gfc_pak_load_json(const char *filename)
{
    char *buffer= NULL;
    SJson *json = NULL;
    unsigned long file_size = 0;
    if (!gfc_pak_initialized())
    {
        return sj_load(filename);
    }
    buffer = gfc_pak_file_extract(filename,&file_size);
    if (!buffer)
    {
        slog("failed to extract json buffer for file %s",filename);
        return NULL;
    }
    json = sj_parse_buffer(buffer,file_size);
    free(buffer);//no longer needed
    if (!json)
    {
        slog("failed to parse file %s",filename);
        return NULL;
    }
    return json;
    
}

/*eol@eof*/
