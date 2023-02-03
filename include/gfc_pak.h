#ifndef __GFC_PAK_H__
#define __GFC_PAK_H__

#include "simple_json.h"
#include "gfc_types.h"

/**
 * @purpose The Pak manager is meant to obscure game content / assets through zip compression.
 * Pak files (just rename the .zip extenstion to .pak or anything for that matter) are added to the manager.
 * Files can be loaded through the manager where it will first check to see if a file is on disk, and then iterate through all of the registered pak files looking for the file in question before giving up.
 */

/**
 * @brief initialize the internal pak manager, queueing up its cleanup on program exit
 */
void gfc_pak_manager_init();

/**
 * @brief register a pak file to the system
 * @param filename the pak file to load
 * @note a no-op if filename is null.  Fails if the file cannot be loaded or out of memory.  See slog for details
 */
void gfc_pak_manager_add(const char *filename);

/**
 * @brief extract a file from disk or an archive.
 * @param filename the name of the file to extract
 * @param fileSize [output] if provided, fileSize will be populated with the size of the file extracted
 * @return NULL on error or not found.  A pointer to the raw file data otherwise.
 * @note the data returned by this function must be cleaned up with free() when you are done.
 */
void *gfc_pak_file_extract(const char *filename,size_t *fileSize);

/**
 * @brief parse json data from the pak files
 */
SJson *gfc_pak_load_json(const char *filename);

#endif
