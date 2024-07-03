#include "simple_logger.h"

#include "gfc_actions.h"

static const char *actionTypes[] =
{
    "none",
    "loop",
    "pass"
};

typedef struct
{
    GFC_ActionList *action_lists;
    Uint32          action_max;
}GFC_ActionListManager;

static GFC_ActionListManager action_list_manager = {0};

/**
 * @brief clean from memory
 */
void gfc_action_list_delete(GFC_ActionList *list);

void gfc_action_close()
{
    int i;
    for (i = 0; i < action_list_manager.action_max; i++)
    {
        if (!action_list_manager.action_lists[i]._refCount)continue;
        gfc_action_list_delete(&action_list_manager.action_lists[i]);
    }
    free(action_list_manager.action_lists);
    memset(&action_list_manager,0,sizeof(GFC_ActionListManager));
}

void gfc_action_init(Uint32 maxActionLists)
{
    action_list_manager.action_max = maxActionLists;
    action_list_manager.action_lists = gfc_allocate_array(sizeof(GFC_ActionList),maxActionLists);
    atexit(gfc_action_close);
}

const char *gfc_action_type_to_text(GFC_ActionType type)
{
    if (type >= AT_MAX)return 0;
    return actionTypes[type];
}

void gfc_action_list_delete(GFC_ActionList *list)
{
    GFC_Action *action;
    if (!list)return;
    int i,c;
    if (list->actions)
    {
        c = gfc_list_get_count(list->actions);
        for (i = 0; i < c;i++)
        {
            action = gfc_list_get_nth(list->actions,i);
            if (!action)continue;
            gfc_action_free(action);
        }
        gfc_list_delete(list->actions);
    }
    memset(list,0,sizeof(GFC_ActionList));
}

GFC_Action *gfc_action_new()
{
    return gfc_allocate_array(sizeof(GFC_Action),1);
}

void gfc_action_free(GFC_Action *action)
{
    if (!action)return;
    free(action);
}

GFC_Action *gfc_action_json_parse(SJson *actionSJ)
{
    GFC_Action *action;
    int tempInt;
    float tempFloat;
    const char *tempStr;
    if (!actionSJ)
    {
        return NULL;
    }
    action = gfc_action_new();
    if (!action)return NULL;
    tempStr = sj_get_string_value(sj_object_get_value(actionSJ,"action"));
    if (tempStr)
    {
        gfc_line_cpy(action->name,tempStr);
    }
    tempStr = sj_get_string_value(sj_object_get_value(actionSJ,"type"));
    if (gfc_strlcmp(tempStr,"loop")==0)
    {
        action->type = AT_LOOP;
    }
    else if (gfc_strlcmp(tempStr,"pass")==0)
    {
        action->type = AT_PASS;
    }
    else if (gfc_strlcmp(tempStr,"none")==0)
    {
        action->type = AT_NONE;
    }
    sj_get_integer_value(sj_object_get_value(actionSJ,"startFrame"),&tempInt);
    action->startFrame = tempInt;
    sj_get_integer_value(sj_object_get_value(actionSJ,"endFrame"),&tempInt);
    action->endFrame = tempInt;
    sj_get_float_value(sj_object_get_value(actionSJ,"frameRate"),&tempFloat);
    action->frameRate = tempFloat;
    return action;
}

SJson *gfc_action_to_json(GFC_Action *action)
{
    SJson *save;
    if (!action)return NULL;
    save = sj_object_new();
    if (!save)return NULL;
    sj_object_insert(save,"action",sj_new_str(action->name));
    sj_object_insert(save,"type",sj_new_str(actionTypes[action->type]));
    sj_object_insert(save,"startFrame",sj_new_int(action->startFrame));
    sj_object_insert(save,"endFrame",sj_new_int(action->endFrame));
    sj_object_insert(save,"frameRate",sj_new_float(action->frameRate));
    return save;
}

SJson *gfc_action_list_to_json(GFC_ActionList *actions)
{
    int i,c;
    SJson *actionJS;
    GFC_Action *action;
    if (!actions)return NULL;
    if (!actions->actions)return NULL;
    actionJS = sj_array_new();
    if (!actionJS)return NULL;
    c = gfc_list_get_count(actions->actions);
    for (i = 0;i < c;i++)
    {
        action = gfc_list_get_nth(actions->actions,i);
        if (!action)continue;
        sj_array_append(actionJS,gfc_action_to_json(action));
    }
    return actionJS;
}

void gfc_action_list_free(GFC_ActionList *list)
{
    if (!list)return;
    list->_refCount--;
    if (list->_refCount <= 0)gfc_action_list_delete(list);
}

void gfc_action_list_append(GFC_ActionList *list,GFC_Action *action)
{
    if (!list)return;
    if (!action)return;
    gfc_list_append(list->actions,action);
}

GFC_ActionList *gfc_action_list_new()
{
    int i;
    for (i = 0; i < action_list_manager.action_max; i++)
    {
        if (action_list_manager.action_lists[i]._refCount)continue;
        action_list_manager.action_lists[i]._refCount = 1;
        action_list_manager.action_lists[i].actions = gfc_list_new();
        return &action_list_manager.action_lists[i];
    }
    return NULL;
}

GFC_ActionList *gfc_action_list_load(const char *filename)
{
    GFC_ActionList *actionList;
    SJson *json;
    if (!filename)return NULL;
    json = sj_load(filename);
    if (!json)return NULL;
    actionList = gfc_action_list_parse(sj_object_get_value(json,"actionList"));
    sj_free(json);
    return actionList;
}

GFC_ActionList *gfc_action_list_parse(SJson *actionList)
{
    GFC_ActionList *al;
    SJson *item;
    int i,c;
    if (!actionList)return NULL;
    al = gfc_action_list_new();
    if (!al)return NULL;
    c = sj_array_get_count(actionList);
    for (i = 0; i < c; i++)
    {
        item = sj_array_get_nth(actionList,i);
        if (!item)continue;
        gfc_action_list_append(al,gfc_action_json_parse(item));
    }
    return al;
}

GFC_Action *gfc_action_list_get_action_frame(GFC_ActionList *al, const char *name,float *frame)
{
    GFC_Action *action;
    action = gfc_action_list_get_action(al, name);
    if (!action)return NULL;
    if (frame)*frame = action->startFrame;
    return action;
}


GFC_Action *gfc_action_list_get_action(GFC_ActionList *al, const char *name)
{
    GFC_Action *action;
    int i,c;
    if ((!al)||(!al->actions))
    {
        slog("no action list provided");
        return NULL;
    }
    if (!name)
    {
        slog("no filename provided");
        return NULL;
    }
    c = gfc_list_get_count(al->actions);
    for (i = 0; i < c;i++)
    {
        action = gfc_list_get_nth(al->actions,i);
        if (!action)continue;
        if (gfc_strlcmp(action->name,name) == 0)
        {
            return action;
        }
    }
    return NULL;// not found
}

Uint32 gfc_action_next_frame_after(GFC_Action *action,float frame)
{
    Uint32 ret = 0;
    if (!action)return 0;
    if (frame < action->startFrame)ret= action->startFrame + 1;
    else
    {
        ret = ceil(frame);
        if (ret > action->endFrame)
        {
            if (action->type == AT_LOOP)ret = action->startFrame;
            ret = action->endFrame;
        }
    }
    return ret;
}

Uint32 gfc_action_list_get_framecount(GFC_ActionList *list)
{
    GFC_Action *action;
    int i,c;
    Uint32 count = 0;
    if ((!list)||(!list->actions))return 0;
    c = gfc_list_get_count(list->actions);
    for (i = 0; i < c; i++)
    {
        action = gfc_list_get_nth(list->actions,i);
        if (!action)continue;
        if (action->endFrame > count)count = action->endFrame;
    }
    return count;
}


GFC_Action *gfc_action_list_get_next_action(GFC_ActionList *list,GFC_Action *action)
{
    int i;
    if ((!list)||(!list->actions))return NULL;
    if (!action)return gfc_list_get_nth(list->actions,0);
    i = gfc_list_get_item_index(list->actions,action);
    if ((i == -1)||(i >= (gfc_list_get_count(list->actions) - 1)))
    {
        return gfc_list_get_nth(list->actions,0);
    }
    return gfc_list_get_nth(list->actions,i + 1);
}

GFC_Action *gfc_action_list_get_action_by_index(GFC_ActionList *list,Uint32 index)
{
    if ((!list)||(!list->actions))return NULL;
    return gfc_list_get_nth(list->actions,index);
}

void gfc_action_list_frame_deleted(GFC_ActionList *list,Uint32 index)
{
    GFC_Action *action;
    int i,c;
    if (!list)return;
    c = gfc_list_get_count(list->actions);
    for (i = 0;i < c; i++)
    {
        action = gfc_list_get_nth(list->actions,i);
        if (!action)continue;
        if (action->startFrame > index)action->startFrame--;
        if (action->endFrame > index)action->endFrame--;
    }
}

GFC_ActionReturnType gfc_action_next_frame(GFC_Action *action,float *frame)
{
    if ((!action)||(!frame))
    {
        return ART_ERROR;
    }
    if (*frame < action->startFrame)
    {
        *frame = action->startFrame;
        return ART_START;
    }
    *frame = *frame + action->frameRate;
    if (*frame >= action->endFrame)
    {
        switch (action->type)
        {
            case AT_MAX:
            case AT_NONE:
                break;
            case AT_LOOP:
                *frame = action->startFrame;
                break;
            case AT_PASS:
                *frame = action->endFrame;
                return ART_END;
        }
    }
    return ART_NORMAL;
}

void gfc_action_list_frame_inserted(GFC_ActionList *list,Uint32 index)
{
    GFC_Action *action;
    int i,c;
    if (!list)return;
    c = gfc_list_get_count(list->actions);
    for (i = 0;i < c; i++)
    {
        action = gfc_list_get_nth(list->actions,i);
        if (!action)continue;
        if (action->startFrame > index)action->startFrame++;
        if (action->endFrame > index)action->endFrame++;
    }
}

GFC_Action *gfc_action_list_get_action_by_frame(GFC_ActionList *list,Uint32 frame)
{
    GFC_Action *action;
    int i,c;
    if (!list)return NULL;
    c = gfc_list_get_count(list->actions);
    for (i = 0; i < c; i++)
    {
        action = gfc_list_get_nth(list->actions,i);
        if (!action)continue;
        if ((frame >= action->startFrame)&&(frame <= action->endFrame))return action;
    }
    return NULL;
}

GFC_Action *gfc_action_list_get_action_by_name(GFC_ActionList *list,const char *name)
{
    GFC_Action *action;
    int i,c;
    if ((!list)||(!name))return NULL;
    c = gfc_list_get_count(list->actions);
    for (i = 0; i < c; i++)
    {
        action = gfc_list_get_nth(list->actions,i);
        if (!action)continue;
        if (gfc_strlcmp(action->name,name)==0)return action;
    }
    return NULL;
}

Uint32 gfc_action_get_framecount(GFC_Action *action)
{
    if (!action)return 0;
    return action->endFrame - action->startFrame;
}

int gfc_action_get_animation_frames(GFC_Action *action)
{
    if (!action)
    {
        return -1;
    }
    if (action->frameRate == 0)
    {
        return -1;//infinite!!! we never stop!
    }
    return (int)((action->endFrame - action->startFrame)/action->frameRate);
}

float gfc_action_get_percent_complete(GFC_Action *action,float frame)
{
    float total,passed;
    if (!action)
    {
        return -1;
    }
    if (action->frameRate == 0)
    {
        return -1;//infinite!!! we never stop!
    }
    total = (action->endFrame - action->startFrame)/action->frameRate;
    passed = (frame - action->startFrame)/action->frameRate;
    if (!total)return 0;
    return (passed/total);
}

int gfc_action_get_frames_remaining(GFC_Action *action,float frame)
{
    float total,passed;
    if (!action)
    {
        return -1;
    }
    if (action->frameRate == 0)
    {
        return -1;//infinite!!! we never stop!
    }
    total = (action->endFrame - action->startFrame)/action->frameRate;
    passed = (frame - action->startFrame)/action->frameRate;
    return (int)(total - passed);
}

int gfc_action_get_action_frame(GFC_Action *action,float frame)
{
    if (!action)
    {
        return -1;
    }
    if (action->frameRate == 0)
    {
        return -1;//infinite!!! we never stop!
    }
    return (int)((frame - action->startFrame)/action->frameRate);
}

GFC_ActionType gfc_action_type_from_text(const char *text)
{
    if (!text)return AT_NONE;
    if (gfc_strlcmp(text,"pass")==0)return AT_PASS;
    if (gfc_strlcmp(text,"loop")==0)return AT_LOOP;
    return AT_NONE;
}


/*eol@eof*/
