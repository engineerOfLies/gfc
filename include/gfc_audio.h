#ifndef __GFC_AUDIO_H__
#define __GFC_AUDIO_H__

#include <SDL.h>
#include <SDL_mixer.h>

#include "simple_json.h"

#include "gfc_types.h"
#include "gfc_text.h"
#include "gfc_list.h"
#include "gfc_hashmap.h"

typedef struct
{
    int channel;        /**<which channel to play it on*/
    int current;        /**<which sound is currently being played*/
    GFC_List *sequence;     /**<list of GFC_Sound pointers to be played in sequence*/
}GFC_SoundSequence;

typedef struct
{
    Uint32 ref_count;
    GFC_TextLine filepath;  /**<the sound file that was loaded*/
    Mix_Chunk *sound;
    float volume;
    int defaultChannel;
}GFC_Sound;

/**
 * @brief initializes the audio system based on the passed in parameters
 * @param maxSounds the maximum number of sounds that can be loaded into memory at once
 * @param enableMP3 if true, initializes audio system with mp3 support, if available
 * @param enableOgg if true, initializes audio system with ogg vorbis support, if available
 */
void gfc_audio_init(
    Uint32 maxSounds,
    Uint8  enableMP3,
    Uint8  enableOgg);

/**
 * @brief initialize the audio subsystem based on the provided config.  
 * @note If it fails to load the config file, it will still initialize, but with default settings
 * @param configFile the filepath to the json file containing the config for it
 */
void gfc_sound_init_config(const char *configFile);

/**
 * @brief load a music file (ogg or mp3 or other supported music file) from disk or pak
 * @param filename the name of the file (or path to it in the pak file)
 * @return NULL on error or a pointer to the music file.  Clean it up with Mix_FreeMusic(Mix_Music *music)
 */
Mix_Music *gfc_sound_load_music(const char *filename);

/**
 * @brief load a WAV or RIFF from file and return it as a sound pointer
 * @param filename the file to load
 * @param volume how loud the sound should be on a scale from 0 to 1.0
 * @param defaultChannel which channel to play this sound on if not specified
 * @return NULL on error or a pointer to the sound file
 */
GFC_Sound *gfc_sound_load(const char *filename,float volume,int defaultChannel);

/**
 * @brief play a sound file that has been loaded
 * @param sound the sound to play
 * @param loops number of times to loop,  0 means play once, no loops, -1 is infinite loops
 * @param volume how loud to play it as a percentage
 * @param channel which channel to play on, -1 means use any
 */
void gfc_sound_play(GFC_Sound *sound,int loops,float volume,int channel);

/**
 * @brief play a sound file to a channel group
 * @param sound the sound to play
 * @param loops number of times to loop,  0 means play once, no loops, -1 is infinite loops
 * @param volume how loud to play it as a percentage
 * @param groupName the name of the group to play on. If not found, it will use any if available
 */
void gfc_sound_play_to_group(GFC_Sound *sound,int loops,float volume,const char *groupName);

/**
 * @brief decrement references to the sound.  Free it when needed
 * @param sound the sound file to free
 */
void gfc_sound_free(GFC_Sound *sound);

/**
 * @brief frees all sounds from memory.  This will invalidate any help GFC_Sound pointers
 */
void gfc_sound_clear_all();

/**
 * @brief parse a sound pack (hashmap of sound names to sound files) from a json file
 * @param filename the name of the json file containing the sounds list to parse
 * @return NULL on error or the sound pack
 */
GFC_HashMap *gfc_sound_pack_parse_file(const char *filename);

/**
 * @brief parse a sound pack (hashmap of sound names to sound files) from a json object
 * @param filename the name of the json file containing the sounds list to parse
 * @return NULL on error or the sound pack
 */
GFC_HashMap *gfc_sound_pack_parse(SJson *sounds);

/**
 * @brief play a sound from a sound pack by its name
 * @param pack the sound pack to play from
 * @param name the name of the sound to play
 * @param loops number of times to loop,  0 means play once, no loops
 * @param volume how loud to play it
 * @param channel which channel to play on, -1 means use any available
 */
void gfc_sound_pack_play(GFC_HashMap *pack, const char *name,int loops,float volume,int channel);

/**
 * @brief play a sound to a channel group from a sound pack by its name
 * @param pack the sound pack to play from
 * @param name the name of the sound to play
 * @param loops number of times to loop,  0 means play once, no loops
 * @param volume how loud to play it
 * @param group which group to play on, NULL means use any available
 */
void gfc_sound_pack_play_to_group(GFC_HashMap *pack, const char *name,int loops,float volume,const char *group);

/**
 * @brief free a previously loaded sound pack
 * @param pack the sound pack to free
 */
void gfc_sound_pack_free(GFC_HashMap *pack);

/**
 * @brief queue a sequence of sounds to play on a specific channel
 * @param sounds a list of pointers to GFC_Sound's
 * @param channel the channel to play the sounds on
 * @note copies the sound list, so feel free to free the list provided.
 * @note it will not free or change the refcount for the sounds in the list, so keep them alive while needed
 */
void gfc_sound_queue_sequence(GFC_List *sounds,int channel);


#endif
