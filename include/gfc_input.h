#ifndef __GFC_INPUT_H__
#define __GFC_INPUT_H__

#include <SDL.h>
#include "gfc_text.h"
#include "gfc_list.h"

typedef enum
{
    GFC_EMK_None = 0,
    GFC_EMK_Shift = 1000,
    GFC_EMK_Alt,
    GFC_EMK_Ctrl,
    GFC_EMK_Super
}GFC_InputModKey;

typedef enum
{
    GFC_IET_Idle    = 0,
    GFC_IET_Press   = 1,
    GFC_IET_Hold    = 2,
    GFC_IET_Release = 3
}GFC_InputEventType;

typedef enum
{
    GFC_IAS_Whole,
    GFC_IAS_Positive,
    GFC_IAS_Negative,
    GFC_IAS_MAX
}GFC_InputAxisStyle;

typedef struct
{
    Uint8               index;      /**<map to the SDL_Joystick axis*/
    GFC_TextWord        name;       /**<name from config*/    
}GFC_InputButtonConf;

typedef struct
{
    Uint8               index;      /**<map to the SDL_Joystick axis*/
    GFC_TextWord        name;       /**<name from config*/
    int                 threshold;  /**<anything less than this is ignored as input*/
    GFC_InputAxisStyle  style;      /**<how this input is treated*/
    int                 min;        /**<minimum possible value - can be negative for some axes*/
    int                 max;        /**<maximum possible value - can be negative for some axes*/
    float               range;      /**<max - min*/
}GFC_InputAxisConf;

typedef struct
{
    Uint32          num_buttons;    /**<how many ACTUAL buttons are on the controller*/
    Uint8          *buttons;        /**<array of current button states (on or off)*/
    Uint8          *old_buttons;    /**<last frame's button states*/
    GFC_List       *buttonMap;      /**<how to read each button*/
    Uint32          num_axis;       /**<how many axes there are for the controller*/
    Sint16         *axis;           /**<measured values of each axis*/
    Sint16         *old_axis;       /**<last frame's axis states*/
    GFC_List       *axisMap;        /**<how to read each axis*/
    SDL_Joystick   *controller;     /**<handle for the hardware*/
}GFC_InputController;


typedef enum
{
    GFC_ITT_None,
    GFC_ITT_Any,
    GFC_ITT_Combo,
    GFC_ITT_MAX
}GFC_InputTriggerType;

typedef enum
{
    GFC_IT_Key,
    GFC_IT_Button,
    GFC_IT_Axis,
    GFC_IT_MouseMotion,
    GFC_IT_MouseButton,
    GFC_IT_MouseWheel,
    GFC_IT_MAX
}GFC_InputType;

/**
 * @brief config for a single input
 */
typedef struct
{
    GFC_InputType   inputType;
    GFC_TextWord    name;
    Uint8           controller;
    Uint32          keyCode;
}GFC_Input;
/**
 * @brief Inputs abstract user input collection.  They can be setup to trigger callbacks and/or polled for current state
 */
typedef struct
{
    GFC_TextLine            name;           /**<the name of this command*/
    GFC_InputTriggerType    trigger;        /**<what it takes to trigger this input, combo or any*/
    GFC_List               *inputs;         /**<inputs that are part of the input*/
    int                     downCount;      /**<how many of the inputs are down*/
    int                     lastDownCount;  /**<previous frame's status*/
    Uint32                  pressTime;      /**<clock ticks when button was pressed*/
    GFC_InputEventType      state;          /**<updated each frame*/
}GFC_Command;

/**
 * @brief initializes the config system
 * @param configFile if not NULL it will load the file for user input configuration
 */
void gfc_input_init(char *configFile);

/**
 * @brief called as often as you want your input data update.  Every Frame, or faster if you like
 * @note must be called or input will be stale
 */
void gfc_input_update();

/**
 * @brief check if a command was pressed/held/release/down this frame
 * @param command the name of the command to check
 * @returns true if pressed, false otherwise
 */
Uint8 gfc_input_command_pressed(const char *command);
Uint8 gfc_input_command_held(const char *command);
Uint8 gfc_input_command_released(const char *command);
Uint8 gfc_input_command_down(const char *command);

/**
 * @brief get the current state of a given command
 */
GFC_InputEventType gfc_input_command_get_state(const char *command);

/**
 * @brief report if the key provided has been pressed this frame
 * @param key the name of the key to check
 * @returns true if pressed, false otherwise
 * 
 */
Uint8 gfc_input_key_pressed(const char *key);
Uint8 gfc_input_key_released(const char *key);
Uint8 gfc_input_key_held(const char *key);
Uint8 gfc_input_key_down(const char *key);

/**
 * @brief check if the mouse wheel is moving in the indicated direction
 * @return 0 if it is not, 1 if it is
 */
Uint8 gfc_input_mouse_wheel_up();
Uint8 gfc_input_mouse_wheel_down();
Uint8 gfc_input_mouse_wheel_left();
Uint8 gfc_input_mouse_wheel_right();

/**
 * @brief check the state of a controller's button
 * @param controller the controller index to check
 * @param button, the SDL_GameControllerButton to check
 * @return if the event is true for this update frame
 */

Uint8 gfc_input_controller_button_state_by_index(Uint8 controller, Uint32 button);
Uint8 gfc_input_controller_button_held_by_index(Uint8 controller, Uint32 button);
Uint8 gfc_input_controller_button_pressed_by_index(Uint8 controller, Uint32 button);
Uint8 gfc_input_controller_button_released_by_index(Uint8 controller, Uint32 button);

/**
 * @brief get the state of a controller's button by its configured name
 * @param controllerId the index of the controller to check
 * @param button the name of the button
 */
Uint8 gfc_input_controller_button_state(Uint8 controllerId, const char *button);
Uint8 gfc_input_controller_button_held(Uint8 controllerId, const char *button);
Uint8 gfc_input_controller_button_pressed(Uint8 controllerId, const char *button);
Uint8 gfc_input_controller_button_released(Uint8 controllerId, const char *button);

/**
 * @brief get the button index given the "name" of the button in the config file
 * @return -1 on not found or error, or the index of the button otherwise
 */
int gfc_input_controller_get_button_index(GFC_InputController *con, const char *button);


/**
 * @brief check the state of a named axis of a controller
 * @param controllerId the id of the controller to poll
 * @param axis the name of the axis to ask about
 * @return 0 on error or if the axis is not engaged, a value <= 1.0 otherwise
 */
float gfc_input_controller_get_axis_state(Uint8 controllerId, const char *axis);

/**
 * @brief get the number of controllers that are setup
 */
int gfc_input_controller_get_count();

/**
 * @brief slog the state of any buttons axis that are active
 * @param controllerId the id of the controller to slog
 */
void gfc_input_controller_slog(Uint8 controllerId);

#endif
