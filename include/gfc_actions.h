#ifndef __GFC_ACTIONS_H__
#define __GFC_ACTIONS_H__

#include "simple_json.h"

#include "gfc_text.h"
#include "gfc_list.h"

/**
 * @purpose Actions and ActionLists are meant to help with animated things.  
 * Handling the update of animations based on named and configured animations
 */

typedef enum
{
    AT_NONE = 0,    /**<not intended to animate*/
    AT_LOOP,        /**<animation is meant to loop*/
    AT_PASS,        /**<animaiton is a single pass and meant to end*/
    AT_MAX          /**<how many types there are*/
}GFC_ActionType;

typedef enum
{
    ART_ERROR,      /**<something went wrong*/
    ART_NORMAL,     /**<still running fine*/
    ART_END,        /**<a Pass animation has completed*/
    ART_START,      /**<just started*/
    ART_LOOP,       /**<a Loop animation has completed a loop*/
    ART_MAX         /**<how many types there are*/
}GFC_ActionReturnType;

/**
 * @brief this structure describes an individual action
 */
typedef struct Action_S
{
    GFC_TextLine        name;       /**<searchable name criteria*/
    int                 startFrame; /**<starting frame of the animation*/
    int                 endFrame;   /**<ending frame of the animmation*/
    float               frameRate;  /**<how much to progress a frame per update*/
    GFC_ActionType      type;       /**<how the animation should be treated*/
}GFC_Action;

/**
 * @brief this structure describes the action list resource
 */
typedef struct
{
    GFC_TextLine    filename;   /**<name of the file this was loaded from*/
    Uint32          _refCount;  /**<how many things are referencing this resource*/
    GFC_List       *actions;    /**<list of animation actions*/
}GFC_ActionList;

/**
 * @brief init the action subsystem
 */
void gfc_action_init(Uint32 maxActionLists);

/**
 * @brief allocate a blank action list
 * @note free it with 
 */
GFC_ActionList *gfc_action_list_new();

/**
 * @brief free an action list that is no longer needed
 * @note this only frees the memory when the ref count reaches zero
 * @param list Action list that will be freed
 */
void gfc_action_list_free(GFC_ActionList *actionList);

/**
 * @brief append an action to the provided action list
 * @param list the action list to add to 
 * @param action the action to add
 * @note this is a no-op if nulls are provided
 */
void gfc_action_list_append(GFC_ActionList *list,GFC_Action *action);

/**
 * @brief parse out an action list from json
 * @param actionList json contain a list of actions
 * @return the parsed list
 */
GFC_ActionList *gfc_action_list_parse(SJson *actionListJson);

/**
 * @brief parse out an aciton list from a json file.  
 * @note The action list must be contained in the "actionList" key
 * @param filename to load
 * @return NULL on error or not found, the action list otherwise
 */
GFC_ActionList *gfc_action_list_load(const char *filename);

/**
 * @brief encode an action list into json
 * @param actionList list of Actions to encode
 * @return NULL on error or SJSon
 */
SJson *gfc_action_list_to_json(GFC_ActionList *actionList);

/**
 * @brief given a list of Actions, search for the name
 * @param list list containing actions
 * @param name the search criteria
 * @return NULL on error or not found, the Action otherwise
 */
GFC_Action *gfc_action_list_get_action_by_name(GFC_ActionList *list,const char *name);

/**
 * @brief given a list of Actions, search for the name, and set the frame to the start frame if provided
 * @param list list containing actions
 * @param name the search criteria
 * @param frame [output] set to the starting frame of the action if provided.
 * @return NULL on error or not found, the Action otherwise
 */
GFC_Action *gfc_action_list_get_action_frame(GFC_ActionList *al, const char *name,float *frame);

/**
 * @brief call if you insert an animation frame and need to keep the actions in line
 * @param list the action list to modify
 * @param index the index of the frame that was inserted
 */
void gfc_action_list_frame_inserted(GFC_ActionList *list,Uint32 index);

/**
 * @brief call if you delete an animation frame and need to keep the actions in line
 * @param list the action list to modify
 * @param index the index of the frame that was deleted
 */
void gfc_action_list_frame_deleted(GFC_ActionList *list,Uint32 index);

/**
 * @brief allocate a new action
 * @return NULL on error a new initialized action
 */
GFC_Action *gfc_action_new();

/**
 * @brief free a previousaly allocated action
 * @param action the action to free
 */
void gfc_action_free(GFC_Action *action);

/**
 * @brief search an action list by name
 * @param al a list of Action
 * @param name the search criteria
 * @return NULL on error or not found, the action otherwise
 */
GFC_Action *gfc_action_list_get_action(GFC_ActionList *al, const char *name);

/**
 * @brief search an action list by its index
 * @param al a list of Action
 * @param index the search criteria
 * @return NULL on error or not found, the action otherwise
 */
GFC_Action *gfc_action_list_get_action_by_index(GFC_ActionList *list,Uint32 index);

/**
 * @brief get the next action in the action list from the provided action
 * @param list the action list to search
 * @param action from here.  IF NULL this returns the first item in the list
 * @return NULL on error or out of range.
 */
GFC_Action *gfc_action_list_get_next_action(GFC_ActionList *list,GFC_Action *action);

/**
 * @brief get the total number of frames provided in an action list
 * @param list the list to check
 * @return 0 on error, the number of frames in total from the action list otherwise
 */
Uint32 gfc_action_list_get_framecount(GFC_ActionList *list);

/**
 * @brief given an action get the next frame from the current frame
 * @param action the action to base the animation on
 * @param frame (input and output) given this starting frame, this frame will be set to the next frame
 * @return if not an ART_ERROR, it will let you know its return status.  
 */
GFC_ActionReturnType gfc_action_next_frame(GFC_Action *action,float *frame);

/**
 * @brief given a frame, what is the next whole number frame that will come afterwards
 * @param action the action to query
 * @param frame the current frame
 * @return which whole number frame would be next
 */
Uint32 gfc_action_next_frame_after(GFC_Action *action,float frame);

/**
 * @brief given an action, check how many frames of animation it has
 * @note this is not adjusted for frame rate
 * @param action the action to query
 * @return the difference between the action's start and end frames
 */
Uint32 gfc_action_get_framecount(GFC_Action *action);

/**
 * @brief given the action, get the number of frames of animation (accounting for frame rate)
 * @note: this DOES take into consideration frame Rate.  SO its more like the number of calls to next_frame
 * before the animation is completed
 * @param action the action in question
 * @return the number of frames (or calls to next_frame), -1 on error
 */
int gfc_action_get_animation_frames(GFC_Action *action);

/**
 * @brief given the action and current frame, get the number of frames left
 * @note: this DOES take into consideration frame Rate.  SO its more like the number of calls to next_frame
 * before the animation is completed
 * @param action the action in question
 * @param frame the current frame
 * @return the number of frames (or calls to next_frame)before the current action is completed
 */
int gfc_action_get_frames_remaining(GFC_Action *action,float frame);

/**
 * @brief returns the percentage of completion of the current action
 * @note: this is for synching timing events
 * @param action the action in question
 * @param frame the current frame
 */
float gfc_action_get_percent_complete(GFC_Action *action,float frame);

/**
 * @brief get the number of frames into the action we are based on the current frame
 * @param action the action in question
 * @param frame the current rendering frame
 * @return -1 on error or no frame rate, the number of frames into the action otherwise
 * @note:this DOES take into consideration frame Rate.  SO its more like the number of calls to next_frame since the action began
 */
int gfc_action_get_action_frame(GFC_Action *action,float frame);

/**
 * @brief get an action from an actor by the frame number
 * @note if there are any overlapping frames, this returns the first in the list
 * @param list the action list to query
 * @param frame the frame to search fo
 * @return NULL on error or no results, the action otherwise
 */
GFC_Action *gfc_action_list_get_action_by_frame(GFC_ActionList *list,Uint32 frame);

/**
 * @brief parse a text name into its actionType
 * @param text the text to parse (should be "pass", "loop" or "none"
 * @return the actiontype
 */
GFC_ActionType gfc_action_type_from_text(const char *text);

/**
 * @brief given an actiontype, get the text associated with it
 * @param type the action type
 * @return the name of the action typee
 */
const char *gfc_action_type_to_text(GFC_ActionType type);


#endif
