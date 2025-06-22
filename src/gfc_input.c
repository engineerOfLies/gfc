#include <simple_json.h>

#include "simple_logger.h"

#include "gfc_list.h"
#include "gfc_pak.h"
#include "gfc_config.h"
#include "gfc_input.h"

typedef struct
{
    GFC_List       *commandList;
    const Uint8    *input_keys;     //points into SDL, we don't own this one
    Uint8          *input_old_keys;
    int             input_key_count;
    int             mouse_wheel_x;
    int             mouse_wheel_y;
    int             mouse_wheel_x_old;
    int             mouse_wheel_y_old;
    GFC_List       *controllers;
    GFC_List       *controllerMaps;
    //TODO make input callbacks a seperate list that can be cleared or set
}GFC_InputManager;

static GFC_InputManager gfc_input_manager = {0};

static const char * gfc_input_types[] =
{
    "key",
    "button",
    "axis",
    "mouseMotion",
    "mouseButton",
    "mouseWheel",
    NULL
};

static const char *gfc_input_trigger_type[] = 
{
    "none",
    "any",
    "combo",
    NULL
};

static const char * gfc_input_axis_style[] =
{
    "whole",
    "positive",
    "negative",
    NULL
};

void gfc_input_close();
GFC_Input *gfc_input_new();
GFC_Command *gfc_command_new();
SDL_Scancode gfc_input_key_to_keyCode(const char * buffer);
GFC_InputController *gfc_input_controller_load(SJson *json,Uint8 index);
GFC_InputControllerMap *gfc_input_controller_map_load(SJson *json);
void gfc_input_commands_load(SJson *commands);
void gfc_input_free(GFC_Input *input);
GFC_Input *gfc_input_command_get_controller_input(const char *commandName,const char *name,Uint8 controllerId);


void gfc_input_init(char *configFile)
{
    SJson *json;
    SJson *array,*item;
    GFC_InputController *controller;
    GFC_InputControllerMap *map;
    int i,c;
    if (gfc_input_manager.commandList != NULL)
    {
        slog("gfc_input_init: error, gfc_input_manager.input_list not NULL");
        return;
    }
    gfc_input_manager.commandList = gfc_list_new();

    gfc_input_manager.input_keys = SDL_GetKeyboardState(&gfc_input_manager.input_key_count);
    if (!gfc_input_manager.input_key_count)
    {
        slog("failed to get keyboard count!");
    }
    else
    {
        gfc_input_manager.input_old_keys = (Uint8*)malloc(sizeof(Uint8)*gfc_input_manager.input_key_count);
        memcpy(gfc_input_manager.input_old_keys,gfc_input_manager.input_keys,sizeof(Uint8)*gfc_input_manager.input_key_count);
    }
    atexit(gfc_input_close);
    
    json = sj_load(configFile);
    if (!json)
    {
        return;//nothing else to do
    }
    //controller support
    array = sj_object_get_value(json,"controllerMaps");
    c = sj_array_count(array);
    if (c > 0)
    {
        gfc_input_manager.controllerMaps = gfc_list_new();
        for (i = 0; i < c; i++)
        {
            item = sj_array_nth(array,i);
            if (!item)continue;
            map = gfc_input_controller_map_load(item);
            if (map)
            {
                gfc_list_append(gfc_input_manager.controllerMaps,map);
                break;
            }
        }
    }
    //MAPS MUST COME FIRST
    array = sj_object_get_value(json,"controllers");
    c = sj_array_count(array);
    if (c > 0)
    {
        gfc_input_manager.controllers = gfc_list_new();
        for (i = 0; i < c; i++)
        {
            item = sj_array_nth(array,i);
            if (!item)continue;
            controller = gfc_input_controller_load(item,i);
            if (controller)
            {
                gfc_list_append(gfc_input_manager.controllers,controller);
                break;
            }
        }
    }
    gfc_input_commands_load(sj_object_get_value(json,"commands"));
    sj_free(json);
}

GFC_InputButtonConf *gfc_controller_get_button_conf(GFC_InputControllerMap *con,const char *name)
{
    int i,c;
    GFC_InputButtonConf *conf = NULL;
    if (!name)return NULL;
    if ((!con)||(!con->buttonMap))return NULL;
    c = gfc_list_count(con->buttonMap);
    for (i = 0;i < c; i++)
    {
        conf = gfc_list_nth(con->buttonMap,i);
        if (!conf)continue;
        if (gfc_word_cmp(name,conf->name) == 0)return conf;
    }
    return NULL;
}

GFC_InputAxisConf *gfc_controller_get_axis_conf(GFC_InputControllerMap *con,const char *name)
{
    int i,c;
    GFC_InputAxisConf *conf = NULL;
    if (!name)return NULL;
    if ((!con)||(!con->axisMap))return NULL;
    c = gfc_list_count(con->axisMap);
    for (i = 0;i < c; i++)
    {
        conf = gfc_list_nth(con->axisMap,i);
        if (!conf)continue;
        if (gfc_word_cmp(name,conf->name) == 0)return conf;
    }
    return NULL;
}

float gfc_controller_determine_a_axis_value(GFC_InputController *con,const char *name,int old)
{
    GFC_InputAxisConf *conf = NULL;
    int measure = 0;
    float value;
    Uint8 index = 0;
    if ((!con)||(!con->map))return 0;
    conf = gfc_controller_get_axis_conf(con->map,name);
    if (!conf)return 0;
    index = conf->index;
    if (index >= con->num_axis)return 0;
    if (!conf->range)return 0;
    
    if (!old)measure = con->axis[index];
    else measure = con->old_axis[index];
    if (!measure)return 0;
    
    switch (conf->style)
    {
        case GFC_IAS_Whole:
            if (measure <= conf->threshold)return 0;
            value = measure - conf->min;
            return value / conf->range;
        case GFC_IAS_Positive:
            if (measure <= conf->threshold)return 0;
            value = measure - conf->min;
            return value / conf->range;
        case GFC_IAS_Negative:
            if (measure >= conf->threshold)return 0;
            value = measure - conf->min;
            return value / conf->range;
        default:
            return 0;
    }
    return 0;
}

float gfc_controller_determine_axis_value(GFC_InputController *con,const char *name)
{
    return gfc_controller_determine_a_axis_value(con,name,0);
}

float gfc_controller_determine_old_axis_value(GFC_InputController *con,const char *name)
{
    return gfc_controller_determine_a_axis_value(con,name,1);
}


void gfc_controller_update(GFC_InputController *controller)
{
    int i;
    Uint8 button;
    if (!controller)return;
    if (!controller->controller)return;//nothing to do
    memcpy(controller->old_buttons,controller->buttons,sizeof(Uint8)*controller->num_buttons);// backup the old
    for (i = 0; i < controller->num_buttons;i++)
    {
        button = SDL_JoystickGetButton(controller->controller,i);
        controller->buttons[i] = button;
//        if (controller->buttons[i])slog("controller button %i is %i",i,controller->buttons[i]);
    }
    for (i = 0; i < controller->num_axis;i++)
    {
        controller->old_axis[i] = controller->axis[i];
        controller->axis[i] = SDL_JoystickGetAxis(controller->controller,i);
    }
}

void gfc_controller_map_free(GFC_InputControllerMap *controller)
{
    if (!controller)return;
    if (controller->buttonMap)
    {
        gfc_list_foreach(controller->buttonMap,(gfc_work_func*)free);
        gfc_list_delete(controller->buttonMap);
    }
    if (controller->axisMap)
    {
        gfc_list_foreach(controller->axisMap,(gfc_work_func*)free);
        gfc_list_delete(controller->axisMap);
    }
    free(controller);
}

void gfc_controller_free(GFC_InputController *controller)
{
    if (!controller)return;
    if (controller->buttons)free(controller->buttons);
    if (controller->old_buttons)free(controller->old_buttons);
    if (controller->axis)free(controller->axis);
    if (controller->old_axis)free(controller->old_axis);
    if (controller->controller)
    {
        SDL_JoystickClose(controller->controller);
    }
    free(controller);
}


GFC_InputController *gfc_controller_get(Uint8 index)
{
    return gfc_list_nth(gfc_input_manager.controllers,index);
}

GFC_InputController *gfc_controller_new()
{
    GFC_InputController *controller;
    controller = gfc_allocate_array(sizeof(GFC_InputController),1);
    return controller;
}

GFC_InputControllerMap *gfc_controller_map_new()
{
    GFC_InputControllerMap *controller;
    controller = gfc_allocate_array(sizeof(GFC_InputControllerMap),1);
    return controller;
}


GFC_InputAxisConf *gfc_controller_config_axis(SJson *json)
{
    const char *style = NULL;
    GFC_InputAxisConf *axis;
    if (!json)return NULL;
    axis = gfc_allocate_array(sizeof(GFC_InputAxisConf),1);
    if (!axis)return NULL;
    sj_object_get_uint8(json,"index",&axis->index);
    sj_object_word_value(json,"name",axis->name);
    sj_object_word_value(json,"label",axis->label);
    sj_object_get_int(json,"threshold",&axis->threshold);
    sj_object_get_int(json,"min",&axis->min);
    sj_object_get_int(json,"max",&axis->max);
    axis->range = axis->max - axis->min;
    style = sj_object_get_string(json,"style");
    if (style)
    {
        if (gfc_strlcmp(style,"whole") == 0)axis->style = GFC_IAS_Whole;
        else if (gfc_strlcmp(style,"positive") == 0)axis->style = GFC_IAS_Positive;
        else if (gfc_strlcmp(style,"negative") == 0)axis->style = GFC_IAS_Negative;
        else axis->style = GFC_IAS_MAX;
    }
    return axis;
}

GFC_InputButtonConf *gfc_controller_config_button(SJson *json)
{
    GFC_InputButtonConf *button;
    if (!json)return NULL;
    button = gfc_allocate_array(sizeof(GFC_InputButtonConf),1);
    if (!button)return NULL;
    sj_object_get_uint8(json,"index",&button->index);
    sj_object_word_value(json,"name",button->name);
    sj_object_word_value(json,"label",button->label);
    return button;
}

GFC_InputControllerMap *gfc_input_get_controller_map(const char *name)
{
    int i,c;
    GFC_InputControllerMap *map;
    if (!name)return NULL;
    c = gfc_list_count(gfc_input_manager.controllerMaps);
    for (i = 0; i < c; i++)
    {
        map = gfc_list_nth(gfc_input_manager.controllerMaps,i);
        if (!map)continue;
        if (gfc_strlcmp(map->name,name) == 0)return map;
    }
    return NULL;
}

GFC_InputController *gfc_input_controller_load(SJson *json,Uint8 index)
{
    GFC_InputController *controller;
    SDL_Joystick   *joystick;
    const char *map;
    if (!json)return NULL;
    controller = gfc_controller_new();
    if (!controller)
    {
        return NULL;
    }
    sj_object_line_value(json,"name",controller->name);
    map = sj_object_get_string(json,"useMap");
    controller->map = gfc_input_get_controller_map(map);
    joystick = SDL_JoystickOpen(index);
    if (!joystick)return controller;
    controller->num_buttons = SDL_JoystickNumButtons(joystick);
    if (controller->num_buttons)
    {
        controller->buttons = gfc_allocate_array(sizeof(Uint8),controller->num_buttons);
        controller->old_buttons = gfc_allocate_array(sizeof(Uint8),controller->num_buttons);
    }
    controller->num_axis = SDL_JoystickNumAxes(joystick);
    if (controller->num_axis)
    {
        controller->axis = gfc_allocate_array(sizeof(Sint16),controller->num_axis);
        controller->old_axis = gfc_allocate_array(sizeof(Sint16),controller->num_axis);
    }
    controller->controller = joystick;
    return controller;
}

GFC_InputControllerMap *gfc_input_controller_map_load(SJson *json)
{
    GFC_InputButtonConf *button;
    GFC_InputAxisConf *axis;
    GFC_InputControllerMap *controller = NULL;
    int i,c;
    SJson *array, *item;
    if (!json)return NULL;
    controller = gfc_controller_map_new();
    if (!controller)
    {
        return NULL;
    }
    sj_object_line_value(json,"name",controller->name);
    array = sj_object_get_value(json,"buttons");
    c = sj_array_count(array);
    if (c)
    {
        controller->buttonMap = gfc_list_new();
        for (i = 0; i < c; i++)
        {
            item = sj_array_nth(array,i);
            if (!item)continue;
            button = gfc_controller_config_button(item);
            if (button)gfc_list_append(controller->buttonMap,button);
        }
    }
    array = sj_object_get_value(json,"axes");
    c = sj_array_count(array);
    if (c)
    {
        controller->axisMap = gfc_list_new();
        for (i = 0; i < c; i++)
        {
            item = sj_array_nth(array,i);
            if (!item)continue;
            axis = gfc_controller_config_axis(item);
            if (button)gfc_list_append(controller->axisMap,axis);
        }
    }
    return controller;
}

//index position of the input in the list of axis map
int gfc_input_controller_get_axis_index(GFC_InputControllerMap *con, const char *axis)
{
    int i,c;
    GFC_InputAxisConf *axisConf;
    if (!con)return -1;
    if (!axis)return -1;
    c = gfc_list_count(con->axisMap);
    for (i = 0; i < c; i++)
    {
        axisConf = gfc_list_nth(con->axisMap,i);
        if (!axisConf)continue;
        if (gfc_strlcmp(axisConf->name,axis)==0)
        {
            return axisConf->index;
        }
    }
    return -1;
}

void gfc_input_command_clear_controls(const char *commandName)
{
    int i;
    GFC_Input *in;
    GFC_Command *command;
    command = gfc_command_get_by_name(commandName);
    if (!command)return;
    i = gfc_list_count(command->inputs);
    if (!i)return;
    for (i--;i >= 0;i--)
    {
        in = gfc_list_nth(command->inputs,i);
        if (!in)continue;
        if ((in->inputType == GFC_IT_Button)||(in->inputType == GFC_IT_Axis))
        {
            gfc_input_free(in);
            gfc_list_delete_nth(command->inputs,i);
        }
    }
}

void gfc_input_command_add_controller_input(const char *commandName,const char *inputName,Uint8 controllerId)
{
    GFC_InputButtonConf *button;
    GFC_InputAxisConf *axis;
    GFC_Input *input = NULL;
    GFC_Command *command;
    GFC_InputController *con;
    if (!inputName)return;
    if (gfc_input_command_get_controller_input(commandName,inputName,controllerId) != NULL)return;//already set
    con = gfc_controller_get(controllerId);
    if ((!con)||(!con->map))return;
    command = gfc_command_get_by_name(commandName);
    if (!command)
    {
        command = gfc_command_new();
        if (!command)return;
        command->trigger = GFC_ITT_Any;//default
        gfc_line_cpy(command->name,commandName);
    }
    button = gfc_controller_get_button_conf(con->map,inputName);
    if (button)
    {
        input = gfc_input_new();
        if (!input)return;
        gfc_word_cpy(input->name,inputName);
        input->inputType = GFC_IT_Button;
        input->controller = controllerId;
        gfc_list_append(command->inputs,input);
        return;
    }
    axis = gfc_controller_get_axis_conf(con->map,inputName);
    if (axis)
    {
        input = gfc_input_new();
        if (!input)return;
        gfc_word_cpy(input->name,inputName);
        input->inputType = GFC_IT_Axis;
        input->controller = controllerId;
        gfc_list_append(command->inputs,input);
        return;
    }
    slog("%s input not found for controller %i",inputName,controllerId);
}

GFC_Input *gfc_input_command_get_input(const char *commandName,const char *name)
{
    GFC_Input *input;
    GFC_Command *command;
    int i,c;
    if ((!commandName)||(!name))return NULL;
    command = gfc_command_get_by_name(commandName);
    if (!command)return NULL;
    c = gfc_list_count(command->inputs);
    for (i = 0; i < c; i++)
    {
        input = gfc_list_nth(command->inputs,i);
        if (!input)continue;
        if (gfc_word_cmp(input->name,name)==0)return input;
    }
    return NULL;
}

GFC_Input *gfc_input_command_get_key_input(const char *commandName,const char *name)
{
    GFC_Input *input;
    input = gfc_input_command_get_input(commandName,name);
    if (!input)return NULL;
    if (input->inputType == GFC_IT_Key)return input;
    return NULL;
}

GFC_Input *gfc_input_command_get_controller_input(const char *commandName,const char *name,Uint8 controllerId)
{
    GFC_Input *input;
    input = gfc_input_command_get_input(commandName,name);
    if (!input)return NULL;
    if (input->controller != controllerId)return NULL;
    if ((input->inputType == GFC_IT_Button)||(input->inputType == GFC_IT_Axis))return input;
    return NULL;
}

void gfc_input_command_add_key(const char *commandName,const char *key)
{
    int keyCode;
    GFC_Input *input;
    GFC_Command *command;
    if (!key)return;
    if (gfc_input_command_get_key_input(commandName,key) != NULL)return;//already set
    keyCode = gfc_input_key_to_keyCode(key);
    if (keyCode == -1)return;
    command = gfc_command_get_by_name(commandName);
    if (!command)
    {
        command = gfc_command_new();
        if (!command)return;
        command->trigger = GFC_ITT_Any;//default
        gfc_line_cpy(command->name,commandName);
    }
    input = gfc_input_new();
    if (!input)return;
    input->keyCode = keyCode;
    gfc_word_cpy(input->name,key);
    input->inputType = GFC_IT_Key;
    gfc_list_append(command->inputs,input);
}


void gfc_input_command_clear_keys(const char *commandName)
{
    int i;
    GFC_Input *in;
    GFC_Command *command;
    command = gfc_command_get_by_name(commandName);
    if (!command)return;
    i = gfc_list_count(command->inputs);
    if (!i)return;
    for (i--;i >= 0;i--)
    {
        in = gfc_list_nth(command->inputs,i);
        if (!in)continue;
        if (in->inputType == GFC_IT_Key)
        {
            gfc_input_free(in);
            gfc_list_delete_nth(command->inputs,i);
        }
    }
}

int gfc_input_controller_get_button_label_by_index(GFC_InputControllerMap *con, Uint8 index, GFC_TextWord output)
{
    int i,c;
    GFC_InputButtonConf *buttonConf;
    if (!con)return 0;
    c = gfc_list_count(con->buttonMap);
    for (i = 0; i < c; i++)
    {
        buttonConf = gfc_list_nth(con->buttonMap,i);
        if (!buttonConf)continue;
        if(buttonConf->index == index)
        {
            gfc_word_cpy(output,buttonConf->label);
            return 1;
        }
    }    
    return 0;
}

int gfc_input_controller_get_label_label_by_index(GFC_InputControllerMap *con, Uint8 index, GFC_TextWord output)
{
    int i,c;
    GFC_InputAxisConf *axisConf;
    if (!con)return 0;
    c = gfc_list_count(con->axisMap);
    for (i = 0; i < c; i++)
    {
        axisConf = gfc_list_nth(con->axisMap,i);
        if (!axisConf)continue;
        if(axisConf->index == index)
        {
            gfc_word_cpy(output,axisConf->label);
            return 1;
        }
    }    
    return 0;
}


int gfc_input_controller_get_button_index(GFC_InputControllerMap *con, const char *button)
{
    int i,c;
    GFC_InputButtonConf *buttonConf;
    if (!con)return -1;
    if (!button)return -1;
    c = gfc_list_count(con->buttonMap);
    for (i = 0; i < c; i++)
    {
        buttonConf = gfc_list_nth(con->buttonMap,i);
        if (!buttonConf)continue;
        if (gfc_strlcmp(buttonConf->name,button)==0)
        {
            return buttonConf->index;
        }
    }
    return -1;
}

int gfc_input_controller_get_count()
{
    return gfc_list_get_count(gfc_input_manager.controllers);
}

float gfc_input_controller_get_axis_state(Uint8 controllerId, const char *axis)
{
    GFC_InputController *con;
    con = gfc_controller_get(controllerId);
    if ((!con)||(!axis))return 0;
    return gfc_controller_determine_a_axis_value(con,axis,0) ;
}

Uint8 gfc_input_controller_button_state(Uint8 controllerId, const char *button)
{
    Uint8 index = 0;
    GFC_InputController *con;
    con = gfc_controller_get(controllerId);
    if ((!con)||(!button))return 0;
    index = gfc_input_controller_get_button_index(con->map, button);
    return gfc_input_controller_button_state_by_index(controllerId,index);
}

Uint8 gfc_input_controller_button_held(Uint8 controllerId, const char *button)
{
    Uint8 index = 0;
    GFC_InputController *con;
    con = gfc_controller_get(controllerId);
    index = gfc_input_controller_get_button_index(con->map,button);
    return gfc_input_controller_button_held_by_index(
        controllerId,
        index);
}

Uint8 gfc_input_controller_button_pressed(Uint8 controllerId, const char *button)
{
    int index;
    GFC_InputController *con;
    con = gfc_controller_get(controllerId);
    index = gfc_input_controller_get_button_index(con->map,button);
    return gfc_input_controller_button_pressed_by_index(
        controllerId,
        index);
}

Uint8 gfc_input_controller_button_released(Uint8 controllerId, const char *button)
{
    int index;
    GFC_InputController *con;
    con = gfc_controller_get(controllerId);
    index = gfc_input_controller_get_button_index(con->map,button);
    return gfc_input_controller_button_released_by_index(
        controllerId,
        index);
}

Uint8 gfc_input_controller_button_state_by_index(Uint8 controllerId, Uint32 button)
{
    GFC_InputController *controller;
    controller = gfc_list_get_nth(gfc_input_manager.controllers,controllerId);
    if (!controller)return 0;
    if (button >= controller->num_buttons)return 0;
    return controller->buttons[button];
}

Uint8 gfc_input_controller_old_button_state_by_index(Uint8 controllerId, Uint32 button)
{
    GFC_InputController *controller;
    controller = gfc_list_get_nth(gfc_input_manager.controllers,controllerId);
    if (!controller)return 0;
    if (button >= controller->num_buttons)return 0;
    return controller->old_buttons[button];
}

Uint8 gfc_input_controller_button_held_by_index(Uint8 controller, Uint32 button)
{
    if ((gfc_input_controller_old_button_state_by_index(controller, button))&&
        (gfc_input_controller_button_state_by_index(controller,button)))
        return 1;
    return 0;
}

Uint8 gfc_input_controller_button_pressed_by_index(Uint8 controller, Uint32 button)
{
    if ((!gfc_input_controller_old_button_state_by_index(controller, button))&&
        (gfc_input_controller_button_state_by_index(controller,button)))
        return 1;
    return 0;
}

Uint8 gfc_input_controller_button_released_by_index(Uint8 controller, Uint32 button)
{
    if ((gfc_input_controller_old_button_state_by_index(controller, button))&&
        (!gfc_input_controller_button_state_by_index(controller,button)))
        return 1;
    return 0;
}

//mouse section

Uint8 gfc_input_mouse_wheel_up()
{
    if (gfc_input_manager.mouse_wheel_y > 0)return 1;
    return 0;
}

Uint8 gfc_input_mouse_wheel_down()
{
    if (gfc_input_manager.mouse_wheel_y < 0)return 1;
    return 0;
}

Uint8 gfc_input_mouse_wheel_left()
{
    if (gfc_input_manager.mouse_wheel_x < 0)return 1;
    return 0;
}

Uint8 gfc_input_mouse_wheel_right()
{
    if (gfc_input_manager.mouse_wheel_x > 0)return 1;
    return 0;
}

void gfc_command_delete(GFC_Command *in)
{
    if (!in)return;
    if (in->inputs)
    {
        gfc_list_foreach(in->inputs,(gfc_work_func*)gfc_input_free);
        gfc_list_delete(in->inputs);
    }
    free(in);
}

GFC_Command *gfc_command_new()
{
    GFC_Command *in = NULL;
    in = (GFC_Command *)gfc_allocate_array(sizeof(GFC_Command),1);
    in->inputs = gfc_list_new();
    return in;
}

void gfc_input_close()
{
    if (gfc_input_manager.input_old_keys)
    {
        free(gfc_input_manager.input_old_keys);
    }
    if (gfc_input_manager.controllers)
    {
        gfc_list_foreach(gfc_input_manager.controllers,(gfc_work_func*)gfc_controller_free);
        gfc_list_delete(gfc_input_manager.controllers);
        gfc_input_manager.controllers = NULL;
    }
    if (gfc_input_manager.controllerMaps)
    {
        gfc_list_foreach(gfc_input_manager.controllerMaps,(gfc_work_func*)gfc_controller_map_free);
        gfc_list_delete(gfc_input_manager.controllerMaps);
        gfc_input_manager.controllers = NULL;
    }
    if (gfc_input_manager.commandList)
    {
        gfc_list_foreach(gfc_input_manager.commandList,(gfc_work_func*)gfc_command_delete);
        gfc_list_delete(gfc_input_manager.commandList);
        gfc_input_manager.commandList = NULL;
    }
    memset(&gfc_input_manager,0,sizeof(GFC_InputManager));
}

int gfc_input_determine_key_down(Uint32 kc)
{
    if (kc == GFC_EMK_Shift)
    {
        //deal with mod keys
        if (gfc_input_manager.input_keys[SDL_SCANCODE_LSHIFT]||gfc_input_manager.input_keys[SDL_SCANCODE_RSHIFT])
        {
            return 1;
        }
        return 0;
    }
    if (kc == GFC_EMK_Alt)
    {
        if (gfc_input_manager.input_keys[SDL_SCANCODE_LALT]||gfc_input_manager.input_keys[SDL_SCANCODE_RALT])
        {
            return 1;
        }
        return 0;
    }
    if (kc == GFC_EMK_Ctrl)
    {
        if (gfc_input_manager.input_keys[SDL_SCANCODE_LCTRL]||gfc_input_manager.input_keys[SDL_SCANCODE_RCTRL])
        {
            return 1;
        }
        return 0;
    }
    if (kc == GFC_EMK_Super)
    {
        if (gfc_input_manager.input_keys[SDL_SCANCODE_LGUI]||gfc_input_manager.input_keys[SDL_SCANCODE_RGUI])
        {
            return 1;
        }
        return 0;
    }
    if(gfc_input_manager.input_keys[kc])
    {
        return 1;
    }
    return 0;
}

int gfc_input_determine_down(GFC_Input *input)
{
    float value;
    if (!input)return 0;
    switch (input->inputType)
    {
        case GFC_IT_Key:
            return gfc_input_determine_key_down(input->keyCode);
        case GFC_IT_Button:
            return gfc_input_controller_button_state(input->controller, input->name);
        case GFC_IT_Axis:
            value = gfc_input_controller_get_axis_state(input->controller, input->name);
            if (value) return 1;
            return 0;
        default:
            return 0;
    }
    return 0;
}

void gfc_command_update(GFC_Command *command)
{
    int i,c;
    GFC_Input *input;
    if (!command)return;
    command->lastDownCount = command->downCount;
    command->downCount = 0;
    c = gfc_list_count(command->inputs);
    for (i = 0; i < c; i++)
    {
        input = gfc_list_nth(command->inputs,i);
        if (!input)continue;
        command->downCount += gfc_input_determine_down(input);
    }
    if ((!command->downCount) && (!command->lastDownCount))
    {
        command->state = GFC_IET_Idle;
        return;
    }
    switch (command->trigger)
    {
        case GFC_ITT_Any:
            if ((command->downCount) && (!command->lastDownCount))
            {
                command->state = GFC_IET_Press;
                command->pressTime = SDL_GetTicks();
                return;
            }
            if ((command->downCount) && (command->lastDownCount))
            {
                command->state = GFC_IET_Hold;
                return;
            }
            if ((!command->downCount) && (command->lastDownCount))
            {
                command->state = GFC_IET_Release;
                return;
            }
            break;
        case GFC_ITT_Combo:
            //TODO make this only work for keys together or controller inputs together
            if ((command->downCount == c) && (command->lastDownCount != c))
            {
                command->state = GFC_IET_Press;
                command->pressTime = SDL_GetTicks();
                return;
            }
            if ((command->downCount == c) && (command->lastDownCount == c))
            {
                command->state = GFC_IET_Hold;
                return;
            }
            if ((command->downCount != c) && (command->lastDownCount == c))
            {
                command->state = GFC_IET_Release;
                return;
            }
            break;
        default:
            break;
    }
}

GFC_Command *gfc_command_get_by_name(const char *name)
{
    Uint32 c,i;
    GFC_Command *in;
    if (!name)
    {
        return NULL;
    }
    c = gfc_list_get_count(gfc_input_manager.commandList);
    for (i = 0;i < c;i++)
    {
        in = gfc_list_get_nth(gfc_input_manager.commandList,i);
        if (!in)continue;
        if (gfc_line_cmp(in->name,name)==0)
        {
            return in;
        }
    }
    return NULL;
}

Uint8 gfc_input_command_pressed(const char *command)
{
    GFC_Command *in;
    in = gfc_command_get_by_name(command);
    if (!in)return 0;
    if (in->state == GFC_IET_Press)return 1;
    return 0;
}

Uint8 gfc_input_command_held(const char *command)
{
    GFC_Command *in;
    in = gfc_command_get_by_name(command);
    if ((in)&&(in->state == GFC_IET_Hold))return 1;
    return 0;
}

Uint8 gfc_input_command_released(const char *command)
{
    GFC_Command *in;
    in = gfc_command_get_by_name(command);
    if ((in)&&(in->state == GFC_IET_Release))return 1;
    return 0;
}

Uint8 gfc_input_command_down(const char *command)
{
    GFC_Command *in;
    in = gfc_command_get_by_name(command);
    if (in)
    {
        if((in->state == GFC_IET_Press)||(in->state == GFC_IET_Hold))return 1;
    }
    return 0;
}

GFC_InputEventType gfc_input_command_get_state(const char *command)
{
    GFC_Command *in;
    in = gfc_command_get_by_name(command);
    if (!in)return 0;
    return in->state;
}

int gfc_input_get_active_key(GFC_TextWord key)
{
    int i;
    for (i = 0;i < gfc_input_manager.input_key_count; i++)
    {
        if (gfc_input_manager.input_keys[i])
        {
            return gfc_input_keycode_to_label(i, key);
        }
    }
    return 0;
}

int gfc_input_get_active_controller_input(GFC_TextWord output,Uint8 controllerId)
{
    int i,c;
    GFC_InputController *con;
    GFC_InputButtonConf *button;
    GFC_InputButtonConf *axis;
    con = gfc_controller_get(controllerId);
    if ((!con)||(!con->map))return 0;
    
    c = gfc_list_count(con->map->buttonMap);
    for (i = 0;i < c;i++)
    {
        button = gfc_list_nth(con->map->buttonMap,i);
        if (!button)continue;
        if (gfc_input_controller_button_state_by_index(controllerId, button->index))
        {
            gfc_word_cpy(output,button->name);
            return 1;
        }
    }
    c = gfc_list_count(con->map->axisMap);
    for (i = 0;i < c;i++)
    {
        axis = gfc_list_nth(con->map->axisMap,i);
        if (!axis)continue;
        if (gfc_controller_determine_a_axis_value(con,axis->name,0))
        {
            gfc_word_cpy(output,axis->name);
            return 1;
        }
    }
    return 0;
}

void gfc_input_update()
{
    GFC_Command *in = NULL;
    GFC_InputController *controller;
    Uint32 c,i;
    SDL_Event event = {0};
    
    memcpy(gfc_input_manager.input_old_keys,gfc_input_manager.input_keys,sizeof(Uint8)*gfc_input_manager.input_key_count);
    gfc_input_manager.mouse_wheel_x_old = gfc_input_manager.mouse_wheel_x;
    gfc_input_manager.mouse_wheel_y_old = gfc_input_manager.mouse_wheel_y;
    gfc_input_manager.mouse_wheel_x = 0;
    gfc_input_manager.mouse_wheel_y = 0;

    SDL_PumpEvents();   // update SDL's internal event structures
    //grab all the input from SDL now
    c = gfc_list_get_count(gfc_input_manager.controllers);
    for (i = 0; i < c; i++)
    {
        controller = gfc_list_get_nth(gfc_input_manager.controllers,i);
        if (!controller)continue;
        gfc_controller_update(controller);
    }

    gfc_input_manager.input_keys = SDL_GetKeyboardState(&gfc_input_manager.input_key_count);

    c = gfc_list_get_count(gfc_input_manager.commandList);
    for (i = 0;i < c;i++)
    {
        in = gfc_list_get_nth(gfc_input_manager.commandList,i);
        if (!in)continue;
        gfc_command_update(in);
    }
    while(SDL_PollEvent(&event))
    {
        if (event.type == SDL_WINDOWEVENT)
        {
            if (event.window.event == SDL_WINDOWEVENT_CLOSE)
            {
                in = gfc_command_get_by_name("exit");
                if (in)
                {
                    in->state = GFC_IET_Press;
                }
            }
        }
        if (event.type == SDL_MOUSEWHEEL)
        {
            if(event.wheel.y > 0) // scroll up
            {
                gfc_input_manager.mouse_wheel_y = 1;
            }
            else if(event.wheel.y < 0) // scroll down
            {
                gfc_input_manager.mouse_wheel_y = -1;
            }

            if(event.wheel.x > 0) // scroll right
            {
                gfc_input_manager.mouse_wheel_x = 1;
            }
            else if(event.wheel.x < 0) // scroll left
            {
                gfc_input_manager.mouse_wheel_x = -1;
            }
        }
    }

}

GFC_InputModKey gfc_input_key_mod_check(const char * buffer)
{
    if (!buffer)return GFC_EMK_None;
    if (strcmp(buffer,"SHIFT")==0)return GFC_EMK_Shift;
    if (strcmp(buffer,"ALT")==0)return GFC_EMK_Alt;
    if (strcmp(buffer,"CTRL")==0)return GFC_EMK_Ctrl;
    if (strcmp(buffer,"SUPER")==0)return GFC_EMK_Super;
    return GFC_EMK_None;
}

int gfc_input_keycode_to_label(Uint32 keyCode, GFC_TextWord output)
{
    if ((keyCode >= SDL_SCANCODE_1) && (keyCode <= SDL_SCANCODE_9))
    {
        gfc_word_sprintf(output,"%i",keyCode - SDL_SCANCODE_1 + 1);
        return 1;
    }
    if ((keyCode >= SDL_SCANCODE_KP_1) && (keyCode <= SDL_SCANCODE_KP_9))
    {
        gfc_word_sprintf(output,"KP_%i",keyCode - SDL_SCANCODE_KP_1 + 1);
        return 1;
    }
    if (keyCode == SDL_SCANCODE_0)
    {
        gfc_word_cpy(output,"0");
        return 1;
    }
    if ((keyCode >= SDL_SCANCODE_A) && (keyCode <= SDL_SCANCODE_Z))
    {
        gfc_word_sprintf(output,"%c",keyCode - SDL_SCANCODE_A + 'a');
        return 1;
    }
    if ((keyCode >= SDL_SCANCODE_F1) && (keyCode <= SDL_SCANCODE_F12))
    {
        gfc_word_sprintf(output,"F%i",keyCode - SDL_SCANCODE_F1 + 1);
        return 1;
    }
    if (keyCode == SDL_SCANCODE_MINUS)
    {
        gfc_word_cpy(output,"-");
        return 1;
    }
    if (keyCode == SDL_SCANCODE_EQUALS)
    {
        gfc_word_cpy(output,"=");
        return 1;
    }
    if (keyCode == SDL_SCANCODE_ESCAPE)
    {
        gfc_word_cpy(output,"ESCAPE");
        return 1;
    }
    if (keyCode == SDL_SCANCODE_BACKSLASH)
    {
        gfc_word_cpy(output,"\\");
        return 1;
    }
    if (keyCode == SDL_SCANCODE_BACKSPACE)
    {
        gfc_word_cpy(output,"BACKSPACE");
        return 1;
    }
    if (keyCode == SDL_SCANCODE_SPACE)
    {
        gfc_word_cpy(output,"SPACE");
        return 1;
    }
    if (keyCode == SDL_SCANCODE_SLASH)
    {
        gfc_word_cpy(output,"/");
        return 1;
    }
    if (keyCode == SDL_SCANCODE_TAB)
    {
        gfc_word_cpy(output,"TAB");
        return 1;
    }
    if (keyCode == SDL_SCANCODE_RETURN)
    {
        gfc_word_cpy(output,"RETURN");
        return 1;
    }
    if (keyCode == SDL_SCANCODE_LEFTBRACKET)
    {
        gfc_word_cpy(output,"[");
        return 1;
    }
    if (keyCode == SDL_SCANCODE_RIGHTBRACKET)
    {
        gfc_word_cpy(output,"]");
        return 1;
    }
    if (keyCode == SDL_SCANCODE_SEMICOLON)
    {
        gfc_word_cpy(output,";");
        return 1;
    }
    if (keyCode == SDL_SCANCODE_GRAVE)
    {
        gfc_word_cpy(output,"`");
        return 1;
    }
    if (keyCode == SDL_SCANCODE_PERIOD)
    {
        gfc_word_cpy(output,".");
        return 1;
    }
    if (keyCode == SDL_SCANCODE_APOSTROPHE)
    {
        gfc_word_cpy(output,"'");
        return 1;
    }
    if (keyCode == SDL_SCANCODE_SEMICOLON)
    {
        gfc_word_cpy(output,";");
        return 1;
    }
    if (keyCode == SDL_SCANCODE_CAPSLOCK)
    {
        gfc_word_cpy(output,"CAPSLOCK");
        return 1;
    }
    if (keyCode == SDL_SCANCODE_PRINTSCREEN)
    {
        gfc_word_cpy(output,"PRINTSCREEN");
        return 1;
    }
    if (keyCode == SDL_SCANCODE_SCROLLLOCK)
    {
        gfc_word_cpy(output,"SCROLL");
        return 1;
    }
    if (keyCode == SDL_SCANCODE_INSERT)
    {
        gfc_word_cpy(output,"INSERT");
        return 1;
    }
    if (keyCode == SDL_SCANCODE_HOME)
    {
        gfc_word_cpy(output,"HOME");
        return 1;
    }
    if (keyCode == SDL_SCANCODE_PAGEUP)
    {
        gfc_word_cpy(output,"PGUP");
        return 1;
    }
    if (keyCode == SDL_SCANCODE_PAGEDOWN)
    {
        gfc_word_cpy(output,"PGDOWN");
        return 1;
    }
    if (keyCode == SDL_SCANCODE_END)
    {
        gfc_word_cpy(output,"END");
        return 1;
    }
    if (keyCode == SDL_SCANCODE_DELETE)
    {
        gfc_word_cpy(output,"DELETE");
        return 1;
    }
    if (keyCode == SDL_SCANCODE_LEFT)
    {
        gfc_word_cpy(output,"LEFT");
        return 1;
    }
    if (keyCode == SDL_SCANCODE_RIGHT)
    {
        gfc_word_cpy(output,"RIGHT");
        return 1;
    }
    if (keyCode == SDL_SCANCODE_UP)
    {
        gfc_word_cpy(output,"UP");
        return 1;
    }
    if (keyCode == SDL_SCANCODE_DOWN)
    {
        gfc_word_cpy(output,"DOWN");
        return 1;
    }
    if (keyCode == SDL_SCANCODE_NUMLOCKCLEAR)
    {
        gfc_word_cpy(output,"NUMLOCK");
        return 1;
    }
    if (keyCode == SDL_SCANCODE_KP_DIVIDE)
    {
        gfc_word_cpy(output,"KP_/");
        return 1;
    }
    if (keyCode == SDL_SCANCODE_KP_MULTIPLY)
    {
        gfc_word_cpy(output,"KP_*");
        return 1;
    }
    if (keyCode == SDL_SCANCODE_KP_MINUS)
    {
        gfc_word_cpy(output,"KP_-");
        return 1;
    }
    if (keyCode == SDL_SCANCODE_KP_COMMA)
    {
        gfc_word_cpy(output,"KP_,");
        return 1;
    }
    if (keyCode == SDL_SCANCODE_KP_PLUS)
    {
        gfc_word_cpy(output,"KP_+");
        return 1;
    }
    if (keyCode == SDL_SCANCODE_KP_ENTER)
    {
        gfc_word_cpy(output,"KP_ENTER");
        return 1;
    }
    if (keyCode == SDL_SCANCODE_KP_0)
    {
        gfc_word_cpy(output,"KP_0");
        return 1;
    }
    if (keyCode == SDL_SCANCODE_KP_PERIOD)
    {
        gfc_word_cpy(output,"KP_.");
        return 1;
    }
    if (keyCode == SDL_SCANCODE_KP_EQUALS)
    {
        gfc_word_cpy(output,"KP_=");
        return 1;
    }
    if (keyCode == SDL_SCANCODE_LCTRL)
    {
        gfc_word_cpy(output,"LCTRL");
        return 1;
    }
    if (keyCode == SDL_SCANCODE_RCTRL)
    {
        gfc_word_cpy(output,"RCTRL");
        return 1;
    }
    if (keyCode == SDL_SCANCODE_RSHIFT)
    {
        gfc_word_cpy(output,"RSHIFT");
        return 1;
    }
    if (keyCode == SDL_SCANCODE_LSHIFT)
    {
        gfc_word_cpy(output,"LSHIFT");
        return 1;
    }
    if (keyCode == SDL_SCANCODE_LALT)
    {
        gfc_word_cpy(output,"LALT");
        return 1;
    }
    if (keyCode == SDL_SCANCODE_RALT)
    {
        gfc_word_cpy(output,"RALT");
        return 1;
    }
    return 0;
}

SDL_Scancode gfc_input_key_to_keyCode(const char * buffer)
{
    int F = 0;
    SDL_Scancode kc = -1;
    if (strlen(buffer) == 1)
    {
        //single letter code
        if ((buffer[0] >= 'a')&&(buffer[0] <= 'z'))
        {
            kc = SDL_SCANCODE_A + buffer[0] - 'a';
        }
        else if (buffer[0] == '0')
        {
            kc = SDL_SCANCODE_0;
        }
        else if (buffer[0] == '1')
        {
            kc = SDL_SCANCODE_1;
        }
        else if (buffer[0] == '2')
        {
            kc = SDL_SCANCODE_2;
        }
        else if (buffer[0] == '3')
        {
            kc = SDL_SCANCODE_3;
        }
        else if (buffer[0] == '4')
        {
            kc = SDL_SCANCODE_4;
        }
        else if (buffer[0] == '5')
        {
            kc = SDL_SCANCODE_5;
        }
        else if (buffer[0] == '6')
        {
            kc = SDL_SCANCODE_6;
        }
        else if (buffer[0] == '7')
        {
            kc = SDL_SCANCODE_7;
        }
        else if (buffer[0] == '8')
        {
            kc = SDL_SCANCODE_8;
        }
        else if (buffer[0] == '9')
        {
            kc = SDL_SCANCODE_9;
        }
        else if (buffer[0] == '-')
        {
            kc = SDL_SCANCODE_MINUS;
        }
        else if (buffer[0] == '=')
        {
            kc = SDL_SCANCODE_EQUALS;
        }
        else if (buffer[0] == '[')
        {
            kc = SDL_SCANCODE_LEFTBRACKET;
        }
        else if (buffer[0] == ']')
        {
            kc = SDL_SCANCODE_RIGHTBRACKET;
        }
        else if (buffer[0] == '.')
        {
            kc = SDL_SCANCODE_PERIOD;
        }
        else if (buffer[0] == ',')
        {
            kc = SDL_SCANCODE_COMMA;
        }
        else if (buffer[0] == ';')
        {
            kc = SDL_SCANCODE_SEMICOLON;
        }
        else if (buffer[0] == '\\')
        {
            kc = SDL_SCANCODE_BACKSLASH;
        }
        else if (buffer[0] == '/')
        {
            kc = SDL_SCANCODE_SLASH;
        }
        else if (buffer[0] == '\'')
        {
            kc = SDL_SCANCODE_APOSTROPHE;
        }
        else if (buffer[0] == ';')
        {
            kc = SDL_SCANCODE_SEMICOLON;
        }
        else if (buffer[0] == '`')
        {
            kc = SDL_SCANCODE_GRAVE;
        }
        else if ((buffer[0] >= ' ')&&(buffer[0] <= '`'))
        {
            kc = SDL_SCANCODE_SPACE + buffer[0] - ' ';
        }
    }
    else
    {
        if (buffer[0] == 'F')
        {
            F = atoi(&buffer[1]);
            if (F <= 12)
            {
                kc = SDL_SCANCODE_F1 + F - 1; 
            }
            else if (F <= 24)
            {
                kc = SDL_SCANCODE_F13 + F - 1; 
            }
        }
        else if (gfc_strincmp(buffer,"KP_",3)==0)
        {
            F = atoi(&buffer[4]);
            if (gfc_strlcmp(buffer,"KP_ENTER") == 0)
            {
                kc = SDL_SCANCODE_KP_ENTER;                
            }
            else if (gfc_strlcmp(buffer,"KP_=") == 0)
            {
                kc = SDL_SCANCODE_KP_EQUALS;                
            }
            else if (gfc_strlcmp(buffer,"KP_+") == 0)
            {
                kc = SDL_SCANCODE_KP_PLUS;
            }
            else if (gfc_strlcmp(buffer,"KP_.") == 0)
            {
                kc = SDL_SCANCODE_KP_PERIOD;
            }
            else if (gfc_strlcmp(buffer,"KP_,") == 0)
            {
                kc = SDL_SCANCODE_KP_COMMA;
            }
            else if (gfc_strlcmp(buffer,"KP_-") == 0)
            {
                kc = SDL_SCANCODE_KP_MINUS;                
            }
            else if (gfc_strlcmp(buffer,"KP_*") == 0)
            {
                kc = SDL_SCANCODE_KP_MULTIPLY;                
            }
            else if (gfc_strlcmp(buffer,"KP_/") == 0)
            {
                kc = SDL_SCANCODE_KP_DIVIDE;                
            }
            else if (F == 0)
            {
                kc = SDL_SCANCODE_KP_0;
            }
            else if ((F > 1)&&(F <= 9))
            {
                kc = SDL_SCANCODE_KP_0 + F;
            }
        }
        else if (strcmp(buffer,"BACKSPACE") == 0)
        {
            kc = SDL_SCANCODE_BACKSPACE;
        }
        else if (strcmp(buffer,"SPACE") == 0)
        {
            kc = SDL_SCANCODE_SPACE;
        }
        else if (strcmp(buffer,"RIGHT") == 0)
        {
            kc = SDL_SCANCODE_RIGHT;
        }
        else if (strcmp(buffer,"LEFT") == 0)
        {
            kc = SDL_SCANCODE_LEFT;
        }
        else if (strcmp(buffer,"UP") == 0)
        {
            kc = SDL_SCANCODE_UP;
        }
        else if (strcmp(buffer,"DOWN") == 0)
        {
            kc = SDL_SCANCODE_DOWN;
        }
        else if (strcmp(buffer,"LALT") == 0)
        {
            kc = SDL_SCANCODE_LALT;
        }
        else if (strcmp(buffer,"RALT") == 0)
        {
            kc = SDL_SCANCODE_RALT;
        }
        else if (strcmp(buffer,"LSHIFT") == 0)
        {
            kc = SDL_SCANCODE_LSHIFT;
        }
        else if (strcmp(buffer,"RSHIFT") == 0)
        {
            kc = SDL_SCANCODE_RSHIFT;
        }
        else if (strcmp(buffer,"LCTRL") == 0)
        {
            kc = SDL_SCANCODE_LCTRL;
        }
        else if (strcmp(buffer,"RCTRL") == 0)
        {
            kc = SDL_SCANCODE_RCTRL;
        }
        else if (strcmp(buffer,"TAB") == 0)
        {
            kc = SDL_SCANCODE_TAB;
        }
        else if (strcmp(buffer,"RETURN") == 0)
        {
            kc = SDL_SCANCODE_RETURN;
        }
        else if (strcmp(buffer,"DELETE") == 0)
        {
            kc = SDL_SCANCODE_DELETE;
        }
        else if (strcmp(buffer,"ESCAPE") == 0)
        {
            kc = SDL_SCANCODE_ESCAPE;
        }
    }
    if (kc == -1)
    {
        slog("no input mapping available for %s",buffer);
    }
    return kc;
}



Uint8 gfc_input_key_pressed(const char *key)
{
    SDL_Scancode kc;
    kc = gfc_input_key_to_keyCode(key);
    if (kc == -1)return 0;
    if ((!gfc_input_manager.input_old_keys[kc])&&(gfc_input_manager.input_keys[kc]))return 1;
    return 0;
}

Uint8 gfc_input_key_released(const char *key)
{
    SDL_Scancode kc;
    kc = gfc_input_key_to_keyCode(key);
    if (kc == -1)return 0;
    if ((gfc_input_manager.input_old_keys[kc])&&(!gfc_input_manager.input_keys[kc]))return 1;
    return 0;
}

Uint8 gfc_input_key_held(const char *key)
{
    SDL_Scancode kc;
    kc = gfc_input_key_to_keyCode(key);
    if (kc == -1)return 0;
    if ((gfc_input_manager.input_old_keys[kc])&&(gfc_input_manager.input_keys[kc]))return 1;
    return 0;
}

Uint8 gfc_input_key_down(const char *key)
{
    SDL_Scancode kc;
    kc = gfc_input_key_to_keyCode(key);
    if (kc == -1)return 0;
    if (gfc_input_manager.input_keys[kc])
    {
        return 1;
    }
    return 0;
}

GFC_Input *gfc_input_new()
{
    GFC_Input *input;
    input = gfc_allocate_array(sizeof(GFC_Input),1);
    if (!input)return NULL;
    return input;
}

void gfc_input_free(GFC_Input *input)
{
    if (!input)return;
    free(input);
}

GFC_Input *gfc_input_parse(SJson *json)
{
    const char *iType;
    GFC_Input *input;
    if (!json)return NULL;
    iType = sj_object_get_string(json,"type");
    if (!iType)return NULL;
    input = gfc_input_new();
    if (!input)return NULL;
    sj_object_word_value(json,"name",input->name);
    if (gfc_strlcmp(iType,"key") == 0)
    {
        input->inputType = GFC_IT_Key;
        input->keyCode =  gfc_input_key_mod_check(input->name);
        if (input->keyCode == GFC_EMK_None)
        {
            input->keyCode = gfc_input_key_to_keyCode(input->name);
        }
        return input;
    }
    if (gfc_strlcmp(iType,"button") == 0)
    {
        input->inputType = GFC_IT_Button;
        sj_object_get_uint8(json,"controller",&input->controller);
        return input;
    }    
    if (gfc_strlcmp(iType,"axis") == 0)
    {
        input->inputType = GFC_IT_Axis;
        sj_object_get_uint8(json,"controller",&input->controller);
        return input;
    }    
    free(input);
    return NULL;
}

GFC_Command *gfc_input_parse_command_json(SJson *command)
{
    GFC_Input *input;
    SJson *value,*list;
    const char * buffer;
    GFC_Command *in;
    int count,i;
    if (!command)return NULL;
    buffer = sj_object_get_string(command,"command");
    if (!buffer)
    {
        slog("input command missing 'command' key");
        return NULL;
    }
    in = gfc_command_new();
    if (!in)return NULL;
    gfc_line_cpy(in->name,buffer);
    buffer = sj_object_get_string(command,"trigger");
    if (buffer)
    {
        if (gfc_strlcmp(buffer,"any") == 0)in->trigger = GFC_ITT_Any;
        else if (gfc_strlcmp(buffer,"combo") == 0)in->trigger = GFC_ITT_Combo;
    }
    list = sj_object_get_value(command,"inputs");
    count = sj_array_get_count(list);
    for (i = 0; i< count; i++)
    {
        value = sj_array_get_nth(list,i);
        if (!value)continue;
        input = gfc_input_parse(value);
        if (input)gfc_list_append(in->inputs,input);
    }
    return in;
}

void gfc_input_commands_load(SJson *commands)
{
    GFC_Command *in;
    SJson *value;
    int count,i;
    if (!commands)
    {
        return;
    }
    count = sj_array_get_count(commands);
    for (i = 0; i< count; i++)
    {
        value = sj_array_get_nth(commands,i);
        if (!value)continue;
        in = gfc_input_parse_command_json(value);
        if (!in)continue;
        gfc_list_append(gfc_input_manager.commandList,in);
    }
}

SJson *gfc_input_controller_button_save(GFC_InputButtonConf *button)
{
    SJson *json;
    if (!button)return NULL;
    json = sj_object_new();
    if (!json)return NULL;
    sj_object_insert(json,"name",sj_new_str(button->name));
    sj_object_insert(json,"index",sj_new_uint8(button->index));
    sj_object_insert(json,"label",sj_new_str(button->label));
    return json;
}

const char *gfc_input_style_to_str(GFC_InputAxisStyle style)
{
    if (style >= GFC_IAS_MAX)return NULL;
    return gfc_input_axis_style[style];
}

const char *gfc_input_type_to_str(GFC_InputType style)
{
    if (style >= GFC_IT_MAX)return NULL;
    return gfc_input_types[style];
}

const char *gfc_input_trigger_type_to_str(GFC_InputTriggerType style)
{
    if (style >= GFC_ITT_MAX)return NULL;
    return gfc_input_trigger_type[style];
}

SJson *gfc_input_controller_axis_save(GFC_InputAxisConf *axis)
{
    const char *style = NULL;
    SJson *json;
    if (!axis)return NULL;
    json = sj_object_new();
    if (!json)return NULL;
    sj_object_insert(json,"name",sj_new_str(axis->name));
    sj_object_insert(json,"index",sj_new_uint8(axis->index));
    sj_object_insert(json,"label",sj_new_str(axis->label));
    sj_object_insert(json,"threshold",sj_new_int(axis->threshold));
    sj_object_insert(json,"min",sj_new_int(axis->min));
    sj_object_insert(json,"max",sj_new_int(axis->max));
    style = gfc_input_style_to_str(axis->style);
    if (style)sj_object_insert(json,"style",sj_new_str(style));
    return json;
}

SJson *gfc_input_controller_save(GFC_InputController *con)
{
    SJson *json;
    if (!con)return NULL;
    json = sj_object_new();
    if (!json)return NULL;
    sj_object_insert(json,"name",sj_new_str(con->name));
    if (con->map)
    {
        sj_object_insert(json,"useMap",sj_new_str(con->map->name));
    }
    return json;
}

SJson *gfc_input_controller_save_map(GFC_InputControllerMap *con)
{
    int i,c;
    SJson *json,*list,*item;
    if (!con)return NULL;
    json = sj_object_new();
    if (!json)return NULL;
    c = gfc_list_count(con->buttonMap);
    if (c)
    {
        list = sj_array_new();
        if (list)
        {
            for (i = 0; i < c; i++)
            {
                item = gfc_input_controller_button_save(gfc_list_nth(con->buttonMap,i));
                if (item)sj_array_append(list,item);
            }
            sj_object_insert(json,"buttons",list);
        }
    }
    c = gfc_list_count(con->axisMap);
    if (c)
    {
        list = sj_array_new();
        if (list)
        {
            for (i = 0; i < c; i++)
            {
                item = gfc_input_controller_axis_save(gfc_list_nth(con->axisMap,i));
                if (item)sj_array_append(list,item);
            }
            sj_object_insert(json,"axes",list);
        }
    }
    return json;
}

SJson *gfc_input_command_input_save(GFC_Input *input)
{
    const char *str;
    SJson *json;
    if (!input)return NULL;
    json = sj_object_new();
    if (!json)return NULL;
    str = gfc_input_type_to_str(input->inputType);
    if (str)sj_object_insert(json,"type",sj_new_str(str));
    sj_object_insert(json,"name",sj_new_str(input->name));
    if ((input->inputType == GFC_IT_Button)||(input->inputType == GFC_IT_Axis))
    {
        sj_object_insert(json,"controller",sj_new_uint8(input->controller));
    }
    return json;
}

SJson *gfc_input_command_save(GFC_Command *command)
{
    int i,c;
    const char *style;
    GFC_Input *input;
    SJson *json,*list,*item;
    if (!command)return NULL;
    json = sj_object_new();
    if (!json)return NULL;
    sj_object_insert(json,"command",sj_new_str(command->name));
    style = gfc_input_trigger_type_to_str(command->trigger);
    if (style)sj_object_insert(json,"trigger",sj_new_str(style));
    c = gfc_list_count(command->inputs);
    if (c)
    {
        list = sj_array_new();
        if (list)
        {
            for (i = 0; i < c; i++)
            {
                input = gfc_list_nth(command->inputs,i);
                item = gfc_input_command_input_save(input);
                if (item)sj_array_append(list,item);
            }
            sj_object_insert(json,"inputs",list);
        }
    }
    return json;
}

SJson *gfc_input_config_save()
{
    int i,c;
    GFC_Command *command;
    GFC_InputControllerMap *controller;
    GFC_InputController *con;
    SJson *json,*array,*item;
    json = sj_object_new();
    if (!json)return NULL;
    c = gfc_list_count(gfc_input_manager.commandList);
    if (c)
    {
        array = sj_array_new();
        if (array)
        {
            for (i = 0;i < c; i++)
            {
                command = gfc_list_nth(gfc_input_manager.commandList,i);
                if (!command)continue;
                item = gfc_input_command_save(command);
                if (item)sj_array_append(array,item);
            }
            sj_object_insert(json,"commands",array);
        }
    }
    c = gfc_list_count(gfc_input_manager.controllers);
    if (c)
    {
        array = sj_array_new();
        if (array)
        {
            for (i = 0;i < c; i++)
            {
                con = gfc_list_nth(gfc_input_manager.controllers,i);
                if (!con)continue;
                item = gfc_input_controller_save(con);
                if (item)sj_array_append(array,item);
            }
            sj_object_insert(json,"controllers",array);
        }
    }
    c = gfc_list_count(gfc_input_manager.controllerMaps);
    if (c)
    {
        array = sj_array_new();
        if (array)
        {
            for (i = 0;i < c; i++)
            {
                controller = gfc_list_nth(gfc_input_manager.controllerMaps,i);
                if (!controller)continue;
                item = gfc_input_controller_save_map(controller);
                if (item)sj_array_append(array,item);
            }
            sj_object_insert(json,"controllerMaps",array);
        }
    }
    return json;
}

void gfc_input_save_config_to_file(const char *filepath)
{
    SJson *json;
    if (!filepath)return;
    json = gfc_input_config_save();
    if (!json)return;
    sj_save(json,filepath);
    sj_free(json);
    return;
}

int gfc_input_get_command_key_label(const char *command, GFC_TextWord word)
{
    int i,c;
    GFC_Input *input;
    GFC_Command *in;
    if (!command)return 0;
    in = gfc_command_get_by_name(command);
    if (!in)return 0;
    c = gfc_list_count(in->inputs);
    for (i = 0; i < c; i++)
    {
        input = gfc_list_nth(in->inputs,i);
        if (!input)continue;
        if (input->inputType == GFC_IT_Key)
        {
            if (gfc_input_keycode_to_label(input->keyCode, word))return 1;
            continue;
        }
    }
    return 0;
}

int gfc_input_get_command_controller_label(const char *command, GFC_TextWord word)
{
    int i,c;
    GFC_InputAxisConf *axis;
    GFC_InputButtonConf *button;
    GFC_Input *input;
    GFC_Command *in;
    GFC_InputController *con;
    if (!command)return 0;
    in = gfc_command_get_by_name(command);
    if (!in)return 0;
    c = gfc_list_count(in->inputs);
    for (i = 0; i < c; i++)
    {
        input = gfc_list_nth(in->inputs,i);
        if (!input)continue;
        if (input->inputType == GFC_IT_Axis)
        {
            con = gfc_controller_get(input->controller);
            if (!con)continue;
            axis = gfc_controller_get_axis_conf(con->map,input->name);
            if (!axis)continue;
            gfc_word_cpy(word,axis->label);
            return 1;
        }
        if (input->inputType == GFC_IT_Button)
        {
            con = gfc_controller_get(input->controller);
            if (!con)continue;
            button = gfc_controller_get_button_conf(con->map,input->name);
            if (!button)continue;
            gfc_word_cpy(word,button->label);
            return 1;
        }
    }
    return 0;
}


void gfc_input_controller_slog(Uint8 controllerId)
{
    int i;
    GFC_InputController *con;
    con = gfc_list_nth(gfc_input_manager.controllers,controllerId);
    if (!con)return;
    for (i = 0;i < con->num_buttons;i++)
    {
        if (con->buttons[i])
        {
            slog("controller %i button %i is down",controllerId,i);
        }
    }
}


/*eol@eof*/
