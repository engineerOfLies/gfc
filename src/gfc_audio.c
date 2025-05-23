#include "simple_logger.h"
#include "simple_json.h"

#include "gfc_pak.h"
#include "gfc_hashmap.h"
#include "gfc_config.h"
#include "gfc_audio.h"

typedef struct
{
    GFC_TextLine    name;
    float           volume;
    int             from;
    int             to;
    int             tag;//should corellate to the order it appeared in the list
}ChannelGroup;

typedef struct
{
    Uint32  max_sounds;
    Uint32  channels;
    GFC_Sound * sound_list;
    GFC_List  * sound_sequences;
    GFC_List  * channelGroups;
    float       masterVolume;
    Uint8       musicVolume;
    float       effectsVolume;

}GFC_SoundManager;

static GFC_SoundManager sound_manager={0};

void gfc_sound_sequence_channel_callback(int channel);
void gfc_sound_close();
void gfc_sound_init(Uint32 max);

ChannelGroup *gfc_audio_channel_group_new();
void gfc_audio_channel_group_free(ChannelGroup *cg);

void gfc_audio_init(
    Uint32 maxSounds,
    Uint8  enableMP3,
    Uint8  enableOgg)
{
    int flags = 0;

    if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 64)==-1)
    {
        slog("Failed to open audio: %s\n", SDL_GetError());
        return;
    }
    
    if (enableMP3)
    {
        flags |= MIX_INIT_MP3;
    }
    if (enableOgg)
    {
        flags |= MIX_INIT_OGG;
    }
    if (!(Mix_Init(flags) & flags))
    {
        slog("failed to initialize some audio support: %s",SDL_GetError());
    }
    atexit(Mix_Quit);
    atexit(gfc_sound_close);
    gfc_sound_init(maxSounds);
}



void gfc_sound_close()
{
    gfc_sound_clear_all();
    if (sound_manager.sound_list != NULL)
    {
        free(sound_manager.sound_list);
    }
    if (sound_manager.channelGroups)
    {
        gfc_list_foreach(sound_manager.channelGroups,(gfc_work_func*)gfc_audio_channel_group_free);
        gfc_list_delete(sound_manager.channelGroups);
        sound_manager.channelGroups = NULL;
    }
    sound_manager.sound_list = NULL;
    sound_manager.max_sounds = 0;
}

void gfc_audio_channel_group_free(ChannelGroup *cg)
{
    if (!cg)return;
    free(cg);
}

ChannelGroup *gfc_audio_channel_group_new()
{
    return gfc_allocate_array(sizeof(ChannelGroup),1);
}

ChannelGroup *gfc_audio_get_group(const char *groupName)
{
    int i,c;
    ChannelGroup *group;
    if (!groupName)return NULL;
    c = gfc_list_count(sound_manager.channelGroups);
    for (i = 0; i < c;i++)
    {
        group = gfc_list_nth(sound_manager.channelGroups,i);
        if (!group)continue;
        if (gfc_stricmp(groupName,group->name) == 0)return group;
    }
    return NULL;
}

void gfc_audio_parse_groups(SJson *json)
{
    int from = 0;
    int count;
    int i,c;
    Uint8 groupVolume;
    SJson *item;
    ChannelGroup *group;
    if (!json)return;
    if (!sound_manager.channelGroups)
    {
        sound_manager.channelGroups = gfc_list_new();
    }
    else
    {
        gfc_list_foreach(sound_manager.channelGroups,(gfc_work_func*)gfc_audio_channel_group_free);
        gfc_list_clear(sound_manager.channelGroups);
    }
    c = sj_array_get_count(json);
    for (i = 0;i < c;i++)
    {
        item = sj_array_nth(json,i);
        if (!item)continue;
        group = gfc_audio_channel_group_new();
        if (!group)continue;
        count = 0;
        sj_object_line_value(item,"name",group->name);
        sj_object_get_int(item,"channels",&count);
        if (sj_object_get_uint8(item,"volume",&groupVolume))
        {
            if (groupVolume > MIX_MAX_VOLUME)groupVolume = MIX_MAX_VOLUME;
            group->volume = groupVolume / (float)MIX_MAX_VOLUME;
        }
        group->tag = i;
        group->from = from;
        group->to = from + count - 1;
        from += count;
        gfc_list_append(sound_manager.channelGroups,group);
    }
    sound_manager.channels = Mix_AllocateChannels(from + 8);//padded so -1 channels can still have a try
    for (i = 0;i < c;i++)
    {
        group = gfc_list_nth(sound_manager.channelGroups,i);
        if (!group)continue;
        Mix_GroupChannels(group->from, group->to, group->tag);
    }
}

void gfc_sound_init_config(const char *configFile)
{
    Uint32 maxSounds = 256;
    Uint8 enableMp3 = 0,enableOgg = 0;
    Uint8 masterVolume = MIX_MAX_VOLUME,effectsVolume = MIX_MAX_VOLUME;
    SJson *json,*soundGroups;
    if (!configFile)return;
    json = sj_load(configFile);
    if (!json)
    {
        slog("failed to load audio config, using defaults");
        gfc_audio_init(256,1,1);
        return;
    }
    sj_object_get_uint32(json,"maxSounds",&maxSounds);
    sj_object_get_uint8(json,"enableMp3",&enableMp3);
    sj_object_get_uint8(json,"enableOgg",&enableOgg);
    
    gfc_audio_init(maxSounds,enableMp3,enableOgg);
    
    sj_object_get_uint8(json,"masterVolume",&masterVolume);
    if (masterVolume > MIX_MAX_VOLUME)masterVolume = MIX_MAX_VOLUME;
    sound_manager.masterVolume = masterVolume / (float)MIX_MAX_VOLUME;
    sj_object_get_uint8(json,"musicVolume",&sound_manager.musicVolume);
    sound_manager.musicVolume = (float)sound_manager.musicVolume * sound_manager.masterVolume;
    sj_object_get_uint8(json,"effectsVolume",&effectsVolume);
    if (effectsVolume > MIX_MAX_VOLUME)effectsVolume = MIX_MAX_VOLUME;
    sound_manager.effectsVolume = effectsVolume / (float)MIX_MAX_VOLUME;
    
    soundGroups = sj_object_get_value(json,"soundGroups");
    Mix_VolumeMusic(sound_manager.musicVolume);
    gfc_audio_parse_groups(soundGroups);
}

void gfc_sound_init(Uint32 max)
{
    if (!max)
    {
        slog("cannot intialize a sound manager for Zero sounds!");
        return;
    }
    sound_manager.max_sounds = max;
    sound_manager.sound_list = gfc_allocate_array(sizeof(GFC_Sound),max);
    sound_manager.sound_sequences = gfc_list_new();
    Mix_ChannelFinished(gfc_sound_sequence_channel_callback);
}

void gfc_sound_delete(GFC_Sound *sound)
{
    if (!sound)return;
    if (sound->sound != NULL)
    {
        Mix_FreeChunk(sound->sound);
    }    
    memset(sound,0,sizeof(GFC_Sound));//clean up all other data
}

void gfc_sound_free(GFC_Sound *sound)
{
    if (!sound) return;
    sound->ref_count--;
}

void gfc_sound_clear_all()
{
    int i;
    for (i = 0;i < sound_manager.max_sounds;i++)
    {
        gfc_sound_delete(&sound_manager.sound_list[i]);// clean up the data
    }
}

GFC_Sound *gfc_sound_new()
{
    int i;
    /*search for an unused sound address*/
    for (i = 0;i < sound_manager.max_sounds;i++)
    {
        if ((sound_manager.sound_list[i].ref_count == 0)&&(sound_manager.sound_list[i].sound == NULL))
        {
            sound_manager.sound_list[i].ref_count = 1;//set ref count
            return &sound_manager.sound_list[i];//return address of this array element        }
        }
    }
    /*find an unused sound address and clean up the old data*/
    for (i = 0;i < sound_manager.max_sounds;i++)
    {
        if (sound_manager.sound_list[i].ref_count == 0)
        {
            gfc_sound_delete(&sound_manager.sound_list[i]);// clean up the old data
            sound_manager.sound_list[i].ref_count = 1;//set ref count
            return &sound_manager.sound_list[i];//return address of this array element
        }
    }
    slog("error: out of sound addresses");
    return NULL;
}

GFC_Sound *gfc_sound_get_by_filename(const char * filename)
{
    int i;
    for (i = 0;i < sound_manager.max_sounds;i++)
    {
        if (gfc_line_cmp(sound_manager.sound_list[i].filepath,filename)==0)
        {
            return &sound_manager.sound_list[i];
        }
    }
    return NULL;// not found
}

GFC_Sound *gfc_sound_load(const char *filename,float volume,int defaultChannel)
{
    GFC_Sound *sound;
    if (!filename)return NULL;
    if (strlen(filename) == 0)return NULL;
    sound = gfc_sound_get_by_filename(filename);
    if (sound)
    {
        sound->ref_count++;
        return sound;
    }
    sound = gfc_sound_new();
    if (!sound)
    {
        return NULL;
    }
    sound->sound = Mix_LoadWAV(filename);
    if (!sound->sound)
    {
        slog("failed to load sound file %s",filename);
        gfc_sound_free(sound);
        return NULL;
    }
    sound->volume = volume;
    sound->defaultChannel = defaultChannel;
    gfc_line_cpy(sound->filepath,filename);
    return sound;
}

void gfc_sound_play_to_group(GFC_Sound *sound,int loops,float volume,const char *groupName)
{
    ChannelGroup *group;
    group = gfc_audio_get_group(groupName);
    if (!group)
    {
        gfc_sound_play(sound,loops,volume,-1);
        return;
    }
    gfc_sound_play(sound,loops,volume * group->volume,Mix_GroupAvailable(group->tag));
}

void gfc_sound_play(GFC_Sound *sound,int loops,float volume,int channel)
{
    int chan;
    float netVolume = sound_manager.masterVolume;
    if (!sound)return;
    if (!sound->sound)return;
    if (volume > 0)
    {
        netVolume *= volume;
    }
    if (channel >= 0)
    {
        chan = channel;
    }
    else
    {
        chan = sound->defaultChannel;
    }
    Mix_VolumeChunk(sound->sound, (int)(netVolume * MIX_MAX_VOLUME));
    Mix_PlayChannel(chan, sound->sound, loops);

}

void gfc_sound_pack_play(GFC_HashMap *pack, const char *name,int loops,float volume,int channel)
{
    GFC_Sound *sound;
    if ((!pack)||(!name))return;
    sound = gfc_hashmap_get(pack,name);
    if (!sound)return;
    gfc_sound_play(sound,loops,volume,channel);
}

void gfc_sound_pack_play_to_group(GFC_HashMap *pack, const char *name,int loops,float volume,const char *group)
{
    GFC_Sound *sound;
    if ((!pack)||(!name))return;
    sound = gfc_hashmap_get(pack,name);
    if (!sound)return;
    gfc_sound_play_to_group(sound,loops,volume,group);
}

void gfc_sound_pack_load_sound(GFC_HashMap *pack, const char *name,const char *file)
{
    GFC_Sound *sound;
    if ((!pack)||(!name)||(!file))return;
    sound = gfc_hashmap_get(pack,name);
    if (sound)
    {
        gfc_sound_free(sound);//delete the old one
        gfc_hashmap_delete_by_key(pack,name);
    }
    sound = gfc_sound_load(file,1,-1);
    if (!sound)return;
    gfc_hashmap_insert(pack,name,sound);
}

void gfc_sound_pack_free(GFC_HashMap *pack)
{
    if (!pack)return;
    gfc_hashmap_foreach(pack, (gfc_work_func*)gfc_sound_free);
    gfc_hashmap_free(pack);
}

GFC_HashMap *gfc_sound_pack_parse_file(const char *filename)
{
    SJson *file;
    SJson *sounds;
    GFC_HashMap *pack;
    if (!filename)return NULL;
    file = gfc_pak_load_json(filename);
    if (!file)return NULL;
    
    sounds = sj_object_get_value(file,"sounds");
    if (!sounds)
    {
        slog("failed to parse sound pack file, no 'sounds' object");
        sj_free(file);
        return NULL;
    }
    pack = gfc_sound_pack_parse(sounds);
    sj_free(file);
    return pack;
}

GFC_HashMap *gfc_sound_pack_parse(SJson *sounds)
{
    int i,c;
    const char *name;
    const char *text;
    SJson *sound;
    GFC_HashMap *pack = NULL;
    if (!sounds)return NULL;
    
    c = sj_array_get_count(sounds);
    if (!c)return NULL;
    pack = gfc_hashmap_new();
    if (!pack)return NULL;
    for (i = 0; i < c; i++)
    {
        sound = sj_array_get_nth(sounds,i);
        if (!sound)continue;
        name = sj_get_string_value(sj_object_get_value(sound,"name"));
        text = sj_get_string_value(sj_object_get_value(sound,"file"));
        gfc_sound_pack_load_sound(pack, name,text);
    }
    return pack;
}

void gfc_sound_sequence_free(GFC_SoundSequence *sequence)
{
    if (!sequence)return;
    if (sequence->sequence)gfc_list_delete(sequence->sequence);
    free(sequence);
}

GFC_SoundSequence *gfc_sound_sequence_new()
{
    GFC_SoundSequence *sequence;
    sequence = gfc_allocate_array(sizeof(GFC_SoundSequence),1);
    if (!sequence)return NULL;
    sequence->sequence = gfc_list_new();
    return sequence;
}

void gfc_sound_queue_sequence(GFC_List *sounds,int channel)
{
    GFC_SoundSequence *sequence;
    if (!sounds)return;
    sequence = gfc_sound_sequence_new();
    if (!sequence)return;
    sequence->channel = channel;
    sequence->sequence = gfc_list_copy(sounds);
    gfc_list_append(sound_manager.sound_sequences,sequence);
    if (!Mix_Playing(channel))
    {
        gfc_sound_sequence_channel_callback(channel);
    }
}

void gfc_sound_sequence_channel_callback(int channel)
{
    GFC_Sound *sound;
    GFC_SoundSequence *sequence;
    int i,c;
    c = gfc_list_get_count(sound_manager.sound_sequences);
    for (i = 0; i < c;i++)
    {
        sequence = gfc_list_get_nth(sound_manager.sound_sequences,i);
        if (!sequence)continue;
        if (sequence->channel != channel)continue;
        sound = gfc_list_get_nth(sequence->sequence,sequence->current);
        if (!sound)continue;
        sequence->current++;
        gfc_sound_play(sound,0,sound->volume,channel);
        if (sequence->current >= gfc_list_get_count(sequence->sequence))//we are finished with this sequence
        {
            gfc_list_delete_data(sound_manager.sound_sequences,sequence);
            gfc_sound_sequence_free(sequence);
        }
        return;
    }
}

Mix_Music *gfc_sound_load_music(const char *filename)
{
    Mix_Music *music;
    music = Mix_LoadMUS(filename);
    return music;
}
/*eol@eof*/
