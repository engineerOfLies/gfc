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
    //TODO make input callbacks a seperate list that can be cleared or set
}GFC_InputManager;

static GFC_InputManager gfc_input_manager = {0};

void gfc_input_close();
GFC_InputController *gfc_input_controller_load(SJson *json,Uint8 index);
void gfc_input_commands_load(SJson *commands);


void gfc_input_init(char *configFile)
{
    SJson *json;
    SJson *array,*item;
    GFC_InputController *controller;
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
    array = sj_object_get_value(json,"controllers");
    c = sj_array_count(array);
    if ((SDL_NumJoysticks() > 0) && (c > 0))
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

GFC_InputAxisConf *gfc_controller_get_axis_conf(GFC_InputController *con,const char *name)
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
    if ((!con)||(!con->axisMap))return 0;
    conf = gfc_controller_get_axis_conf(con,name);
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
        case GFC_IAS_Negative:

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

void gfc_controller_free(GFC_InputController *controller)
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

GFC_InputAxisConf *gfc_controller_config_axis(SJson *json)
{
    const char *style = NULL;
    GFC_InputAxisConf *axis;
    if (!json)return NULL;
    axis = gfc_allocate_array(sizeof(GFC_InputAxisConf),1);
    if (!axis)return NULL;
    sj_object_get_uint8(json,"index",&axis->index);
    sj_object_word_value(json,"name",axis->name);
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
    return button;
}


GFC_InputController *gfc_input_controller_load(SJson *json,Uint8 index)
{
    GFC_InputButtonConf *button;
    GFC_InputAxisConf *axis;
    GFC_InputController *controller = NULL;
    SDL_Joystick   *joystick;
    int i,c;
    SJson *array, *item;
    if (!json)return NULL;
    joystick = SDL_JoystickOpen(index);
    if (!joystick)return NULL;
    controller = gfc_controller_new();
    if (!controller)
    {
        SDL_JoystickClose(joystick);
        return NULL;
    }
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
int gfc_input_controller_get_axis_index(GFC_InputController *con, const char *axis)
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


int gfc_input_controller_get_button_index(GFC_InputController *con, const char *button)
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
    index = gfc_input_controller_get_button_index(con, button);
    return gfc_input_controller_button_state_by_index(controllerId,index);
}

Uint8 gfc_input_controller_button_held(Uint8 controllerId, const char *button)
{
    Uint8 index = 0;
    GFC_InputController *con;
    con = gfc_controller_get(controllerId);
    index = gfc_input_controller_get_button_index(con,button);
    return gfc_input_controller_button_held_by_index(
        controllerId,
        index);
}

Uint8 gfc_input_controller_button_pressed(Uint8 controllerId, const char *button)
{
    int index;
    GFC_InputController *con;
    con = gfc_controller_get(controllerId);
    index = gfc_input_controller_get_button_index(con,button);
    return gfc_input_controller_button_pressed_by_index(
        controllerId,
        index);
}

Uint8 gfc_input_controller_button_released(Uint8 controllerId, const char *button)
{
    int index;
    GFC_InputController *con;
    con = gfc_controller_get(controllerId);
    index = gfc_input_controller_get_button_index(con,button);
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
        gfc_list_foreach(in->inputs,(gfc_work_func*)free);
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
    if (gfc_input_manager.commandList)
    {
        gfc_list_foreach(gfc_input_manager.commandList,(gfc_work_func*)gfc_command_delete);
        gfc_list_delete(gfc_input_manager.commandList);
        gfc_input_manager.commandList = NULL;
    }
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

SDL_Scancode gfc_input_key_to_scancode(const char * buffer)
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
        else if (strcmp(buffer,"BACKSPACE") == 0)
        {
            kc = SDL_SCANCODE_BACKSPACE;
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
    kc = gfc_input_key_to_scancode(key);
    if (kc == -1)return 0;
    if ((!gfc_input_manager.input_old_keys[kc])&&(gfc_input_manager.input_keys[kc]))return 1;
    return 0;
}

Uint8 gfc_input_key_released(const char *key)
{
    SDL_Scancode kc;
    kc = gfc_input_key_to_scancode(key);
    if (kc == -1)return 0;
    if ((gfc_input_manager.input_old_keys[kc])&&(!gfc_input_manager.input_keys[kc]))return 1;
    return 0;
}

Uint8 gfc_input_key_held(const char *key)
{
    SDL_Scancode kc;
    kc = gfc_input_key_to_scancode(key);
    if (kc == -1)return 0;
    if ((gfc_input_manager.input_old_keys[kc])&&(gfc_input_manager.input_keys[kc]))return 1;
    return 0;
}

Uint8 gfc_input_key_down(const char *key)
{
    SDL_Scancode kc;
    kc = gfc_input_key_to_scancode(key);
    if (kc == -1)return 0;
    if (gfc_input_manager.input_keys[kc])
    {
        return 1;
    }
    return 0;
}

GFC_Input *gfc_input_parse(SJson *json)
{
    const char *iType;
    GFC_Input *input;
    if (!json)return NULL;
    iType = sj_object_get_string(json,"type");
    if (!iType)return NULL;
    input = gfc_allocate_array(sizeof(GFC_Input),1);
    if (!input)return NULL;
    sj_object_word_value(json,"name",input->name);
    if (gfc_strlcmp(iType,"key") == 0)
    {
        input->inputType = GFC_IT_Key;
        input->keyCode =  gfc_input_key_mod_check(input->name);
        if (input->keyCode == GFC_EMK_None)
        {
            input->keyCode = gfc_input_key_to_scancode(input->name);
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
    slog("parsed %i commands",gfc_list_count(gfc_input_manager.commandList));
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
