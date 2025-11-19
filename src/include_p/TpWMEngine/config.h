/* liucy has been here，but nothing to see and nothing left ^_^!*/

/*
** Copyright (c) 2007-2021 By Alexander.King.
**
** Permission is hereby granted, free of charge, to any person obtaining a
** copy of this software and/or associated documentation files (the
** "Materials"), to deal in the Materials without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Materials, and to
** permit persons to whom the Materials are furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be included
** in all copies or substantial portions of the Materials.
**
** THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
*/
#ifndef CONFIG_H
#define CONFIG_H

#include "pixdef.h"

#ifdef __cplusplus
extern "C"{
#endif

/**
 * Open for read/write a config file. 
 * If the file is not exist, it will be created when <config_close>.
 * @param <path> The config file path. If (null), no file will be created/modified.
 * @return The config handler.
 */

extern DECLSPEC IPiConfig * STDCALL config_open(const char *path);

/**
 * Load config from one file. All exist sections and key-value pairs will be deleted and the 
 * specified file's will be loaded.
 * @param <cfg> The config handler.
 * @param <path> Path of file to be loaded.
 * @return true if load success or false if any errors.
 */
extern DECLSPEC tpBool STDCALL config_load(IPiConfig *cfg, const char *path);

/**
 * Save config sections/key-value paires to one file.
 * @param <cfg> The config handler.
 * @param <path> Path of file to be saved.
 * @return true if save success or false if any errors.
 */
extern DECLSPEC tpBool STDCALL config_save(IPiConfig *cfg, const char *path);

/**
 * Close config handler and write any changes to file. 
 * If the file is not exist, it will be created here.
 * @param <cfg> The config handler.
 * @return (none).
 */
extern DECLSPEC void STDCALL config_close(IPiConfig *cfg);

/**
 * Get config head comment.
 * @param <cfg> The config handler.
 * @return Head comment of config or null if no head comment.
 */
extern DECLSPEC const char * STDCALL config_get_comment(IPiConfig *cfg);

/**
 * Set config head comment.
 * @param <cfg> The config handler.
 * @param <comment> The comment string.
 * @return (none).
 */
extern DECLSPEC void STDCALL config_set_comment(IPiConfig *cfg, const char *comment);

/**
 * Get number of total sections in config.
 * @param <cfg> The config handler.
 * @return The number of sections. If the config file does not define any sections (only has 
 *             key-value pair), then 1 will be returned (default section).
 */
extern DECLSPEC int STDCALL config_get_nbr_sections(IPiConfig *cfg);

/**
 * Get all sections defined in config.
 * @param <cfg> The config handler.
 * @return The section's name array. If the config file does not define any sections (only has 
 *             key-value pair), then an empty string will be returned.
 */
extern DECLSPEC const char ** STDCALL config_get_sections(IPiConfig *cfg);

/**
 * Check if the config contains such a section.
 * @param <cfg> The config handler.
 * @param <section> The section name to be check.
 * @return Whether specified section is exist in this config.
 */
extern DECLSPEC tpBool STDCALL config_has_section(IPiConfig *cfg, const char *section);

/**
 * Get section comment.
 * @param <cfg> The config handler.
 * @param <section> The section name which to be get comment.
 * @return Comments of section or null if no comment.
 */
extern DECLSPEC const char *STDCALL config_get_section_comment(IPiConfig *cfg, const char *section);

/**
 * Set section comment. If section doest not exist, it will be created.
 * @param <cfg> The config handler.
 * @param <section> The section name which to be set comment.
 * @param <comment> The comment string.
 * @return (none).
 */
extern DECLSPEC void STDCALL config_set_section_comment(IPiConfig *cfg, const char *section, const char *comment);

/**
 * Get number of total keys in config-section.
 * @param <cfg> The config handler.
 * @param <section> The section name which to be get key count.
 * @return The number of keys in specified section or -1 if no such section.
 */
extern DECLSPEC int STDCALL config_get_nbr_keys(IPiConfig *cfg, const char *section);

/**
 * Get all keys in config-section.
 * @param <cfg> The config handler.
 * @param <section> The section name which to be get keys.
 * @return The keys array or null if no such section.
 */
extern DECLSPEC const char ** STDCALL config_get_keys(IPiConfig *cfg, const char *section);

/**
 * Check if the config-section contains the key.
 * @param <cfg> The config handler.
 * @param <section> The section name in which to check key.
 * @param <key> The key to be check.
 * @return Whether the key is exist in this config-section.
 */
extern DECLSPEC tpBool STDCALL config_has_key(IPiConfig *cfg, const char *section, const char *key);

/**
 * Delete a section in config. All keys belong to this section will be deleted at the same time.
 * @param <cfg> The config handler.
 * @param <section> The section to be deleted.
 * @return true if the section exist and has been deleted else false.
 */
extern DECLSPEC tpBool STDCALL config_delete_section(IPiConfig *cfg, const char *section);

/**
 * Delete the key in config-section.
 * @param <cfg> The config handler.
 * @param <section> The section in which to delete key.
 * @param <key> The key to be deleted.
 * @return true if the key exist and has been deleted else false.
 */
extern DECLSPEC tpBool STDCALL config_delete_key(IPiConfig *cfg, const char *section, const char *key);

/**
 * Delete value combined with the key, the key is still exist but it's value is empty.
 * Therefor if you go to get it's value later, the default value is always returned.
 * @param <cfg> The config handler.
 * @param <section> The section in which to delete key value.
 * @param <key> The key to be delete it's value.
 * @return true if the key exist and value has been deleted else false.
 */
extern DECLSPEC tpBool STDCALL config_delete_value(IPiConfig *cfg, const char *section, const char *key);

/**
 * Get key comment.
 * @param <cfg> The config handler.
 * @param <section> The section name in which to get comment.
 * @param <key> The key to be get its comment.
 * @return Comments of key or null if no comment or section/key is not exist.
 */
extern DECLSPEC const char * STDCALL config_get_key_comment(IPiConfig *cfg, const char *section, const char *key);

/**
 * Get a string type value in config.
 * @param <cfg> The config handler.
 * @param <section> The section in which to get key value.
 * @param <key> The key to be get its value.
 * @param <default_value> If the section/key is not exist, this default value will be returned.
 * @return The string type value combined with the key.
 */
extern DECLSPEC const STDCALL char *config_get_value_string(IPiConfig *cfg, const char *section, const char *key, const char *default_value);

/**
 * Get an integer value in config.
 * @param <cfg> The config handler.
 * @param <section> The section in which to get key value.
 * @param <key> The key to be get its value.
 * @param <default_value> If the section/key is not exist or the value is not an integer, 
 *                                     this default value will be returned.
 * @return The integer value combined with the key.
 */
extern DECLSPEC int STDCALL config_get_value_int(IPiConfig *cfg, const char *section, const char *key, int default_value);

/**
 * Get a floating point value in config.
 * @param <cfg> The config handler.
 * @param <section> The section in which to get key value.
 * @param <key> The key to be get its value.
 * @param <default_value> If the section/key is not exist or the value is not a floating 
 *                                     number, this default value will be returned.
 * @return The float value combined with the key.
 */
extern DECLSPEC float STDCALL config_get_value_float(IPiConfig *cfg, const char *section, const char *key, float default_value);

/**
 * Get a boolean value in config.
 * @param <cfg> The config handler.
 * @param <section> The section in which to get key value.
 * @param <key> The key to be get its value.
 * @param <default_value> If the section/key is not exist or the value string is not a 
 *                                     canonical boolean expression, this default value will be returned.
 * @return The boolean value combined with the key.
 */
extern DECLSPEC tpBool STDCALL config_get_value_bool(IPiConfig *cfg, const char *section, const char *key, tpBool default_value);

/**
 * Set key comment. If the section/key doest not exist, they will be created.
 * @param <cfg> The config handler.
 * @param <section> The section name in which to set comment.
 * @param <key> The key to be set its comment.
 * @param <comment> The comment string.
 * @return (none).
 */
extern DECLSPEC void STDCALL config_set_key_comment(IPiConfig *cfg, const char *section, const char *key, const char *comment);

/**
 * Set a string type key value. If the section/key doest not exist, they will be created.
 * @param <cfg> The config handler.
 * @param <section> The section in which to set key value.
 * @param <key> The key to be set its value.
 * @value The value to be set.
 * @return (none).
 */
extern DECLSPEC void STDCALL config_set_value_string(IPiConfig *cfg, const char *section, const char *key, const char *value);

/**
 * Set an integer key value. If the section/key doest not exist, they will be created.
 * @param <cfg> The config handler.
 * @param <section> The section in which to set key value.
 * @param <key> The key to be set its value.
 * @value The value to be set.
 * @return (none).
 */
extern DECLSPEC void STDCALL config_set_value_int(IPiConfig *cfg, const char *section, const char *key, int value);

/**
 * Set a floating point key value. If the section/key doest not exist, they will be created.
 * @param <cfg> The config handler.
 * @param <section> The section in which to set key value.
 * @param <key> The key to be set its value.
 * @value The value to be set.
 * @return (none).
 */
extern DECLSPEC void STDCALL config_set_value_float(IPiConfig *cfg, const char *section, const char *key, float value);

/**
 * Set a boolean key value. If the section/key doest not exist, they will be created.
 * @param <cfg> The config handler.
 * @param <section> The section in which to set key value.
 * @param <key> The key to be set its value.
 * @value The value to be set.
 * @return (none).
 */
extern DECLSPEC void STDCALL config_set_value_bool(IPiConfig *cfg, const char *section, const char *key, tpBool value);

#ifdef __cplusplus
}
#endif

#endif
