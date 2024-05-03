#include "miniz.h"
#include "simple_logger.h"
#include "simple_json_parse.h"
#include "gfc_text.h"
#include "gfc_list.h"
#include "gfc_pak.h"

typedef struct
{
    TextLine filename;
    mz_zip_archive zipFile;
}GFC_PakFile;

typedef struct
{
    List *pak_files;
}GFC_PakManager;

static GFC_PakManager pak_manager = {0};

void gfc_pak_file_free(GFC_PakFile *pakFile);
GFC_PakFile *gfc_pak_file_new();


void gfc_pak_manager_close()
{
    //clear out all loaded pak files
    if (pak_manager.pak_files)
    {
        gfc_list_foreach(pak_manager.pak_files,(gfc_work_func*)gfc_pak_file_free);
        gfc_list_delete(pak_manager.pak_files);
    }
    pak_manager.pak_files = NULL;
}

void gfc_pak_manager_init()
{
    atexit(gfc_pak_manager_close);
    pak_manager.pak_files = gfc_list_new();
}

GFC_PakFile *gfc_pak_manager_get_by_filename(const char *filename)
{
    GFC_PakFile *pakFile = NULL;
    int i,c;
    if (!filename)return NULL;
    c = gfc_list_get_count(pak_manager.pak_files);
    for (i = 0; i< c; i++)
    {
        pakFile = gfc_list_get_nth(pak_manager.pak_files,i);
        if (!pakFile)continue;
        if (gfc_strlcmp(filename,pakFile->filename)==0)return pakFile;
    }
    return NULL;
}

void gfc_pak_manager_add(const char *filename)
{
    GFC_PakFile *pakFile = NULL;
    if (!filename)return;
    if (!pak_manager.pak_files)
    {
        slog("pak manager not initialized");
        return;
    }
    pakFile = gfc_pak_manager_get_by_filename(filename);
    if (pakFile)return;// already loaded
    pakFile = gfc_pak_file_new();
    if (!pakFile)
    {
        slog("failed to allocate data for pak file");
        return;
    }
    if (!mz_zip_reader_init_file(&pakFile->zipFile, filename, 0))
    {
        slog("loading of archive file %s failed.",filename);
        gfc_pak_file_free(pakFile);
        return;
    }
    gfc_list_append(pak_manager.pak_files,pakFile);
}

void gfc_pak_file_free(GFC_PakFile *pakFile)
{
    if (!pakFile)return;
    mz_zip_reader_end(&pakFile->zipFile);
    free(pakFile);
}

GFC_PakFile *gfc_pak_file_new()
{
    return gfc_allocate_array(sizeof(GFC_PakFile),1);
}

void *gfc_pak_load_file_from_disk(const char *filename,size_t *fileSize)
{
    long size;
    void *data;
    FILE *file;
    if (!filename)return NULL;
    file = fopen(filename,"r");
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

SJson *gfc_pak_load_json(const char *filename)
{
    void *data;
    SJson *json;
    size_t fileSize;
    data = gfc_pak_file_extract(filename,&fileSize);
    if (!data)return NULL;
    json = sj_parse_buffer(data,fileSize);
    free(data);
    return json;
}

void *gfc_pak_file_extract(const char *filename,size_t *fileSize)
{
    GFC_PakFile *pakFile = NULL;
    mz_zip_archive_file_stat pStat = {0};
    void *fileData;
    int index;
    int i,c;
    if (!filename)return NULL;
    //first we see if there is a local override to a pak file
    fileData = gfc_pak_load_file_from_disk(filename,fileSize);
    if (fileData)return fileData;
    c = gfc_list_get_count(pak_manager.pak_files);
    for (i = 0; i< c; i++)
    {
        pakFile = gfc_list_get_nth(pak_manager.pak_files,i);
        if (!pakFile)continue;
        index = mz_zip_reader_locate_file(&pakFile->zipFile, filename, NULL, 0);
        if (index == -1)continue;// not in this file
        if (!mz_zip_reader_file_stat(&pakFile->zipFile, index, &pStat))
        {
            slog("failed to read archive for file %f",filename);
            return NULL;
        }
        fileData = gfc_allocate_array(pStat.m_uncomp_size,1);
        if (!fileData)
        {
            slog("failed to allocate data to extract file %f",filename);
            return NULL;
        }
        if (!mz_zip_reader_extract_to_mem(&pakFile->zipFile, index, fileData, pStat.m_uncomp_size, 0))
        {
            slog("failed to extract file %f",filename);
            free(fileData);
            return NULL;
        }
        if (fileSize)*fileSize = pStat.m_uncomp_size;
        return fileData;
    }
    //nope, couldn't find it
    return NULL;
}
/*eol@eof*/
