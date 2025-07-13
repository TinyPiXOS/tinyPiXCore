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
#include "tpConfig.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

typedef struct{
    int32_t length;
    char **array;
}PiSplitString;

bool isEmpty(const char *str)
{   
    return (str == NULL) || (strlen(str) == 0);
}

static inline char *fgetline(FILE *fp)
{
    char buf[128];
    char c;
    char *line = NULL;
    int32_t line_len = 0, buf_len = 0;

    while(!feof(fp))
    {
        memset(buf, 0, sizeof(buf));
        fgets(buf, sizeof(buf), fp);
        buf_len = strlen(buf);
        line = (char *)realloc(line, line_len+buf_len+1);
        strcpy(line+line_len, buf);
        line_len += buf_len;
        
        /**
         * fgets() reads '\n' and set '\0' for last character.
         * So if line length < buf length, "c" should be '\0'. If line length == buf length, "c" should be '\n'.
         * Else "c" should be one character, then we should read more.
         */
        c = buf[sizeof(buf)-2];
        if(c == '\0' || c == '\n')
        {
            break;
        }
    }
    return line;
}

static inline char *trim(char *str)
{   
    if(str != NULL)
    {
        int32_t len = (int32_t)strlen(str);
        char *start = str;
        char *end = str + len;
        while(start < end && *start <= ' ')
        {
            start++;
        }
        while(end > start && *(end-1) <= ' ')
        {
            end--;
        }
        if(end - start != len)
        {
            strncpy(str, start, end-start);
            str[end-start] = '\0';
        }
    }
    return str;
}

static inline PiSplitString *split_create(const char *str, const char *delimiter)
{
    if(!isEmpty(str) && !isEmpty(delimiter))
    {
        int32_t str_len = (int32_t)strlen(str);
        int32_t delimiter_len = (int32_t)strlen(delimiter);
        PiSplitString *strings = (PiSplitString *)malloc(sizeof(PiSplitString));
        char *start, *end;
        strings->length = 0;
        strings->array = NULL;
        start = (char *)str;
        do
        {   
            end = strstr(start, delimiter);
            if(end == NULL)
            {
                end = (char *)str + str_len;
            }
            strings->length++;
            strings->array = (char **)realloc(strings->array, strings->length*sizeof(char *));
            strings->array[strings->length-1] = (char *)malloc(end-start+1);
            memcpy(strings->array[strings->length-1], start, end-start);
            strings->array[strings->length-1][end-start] = '\0';

            start = end + delimiter_len;
        }
        while(start < str + str_len);

        return strings;
    }

    return NULL;
}

static inline void split_free(PiSplitString *strings)
{
    if(strings != NULL)
    {
        int32_t i;
        for(i=0; i<strings->length; i++)
        {
            if(strings->array[i] != NULL)
            {
                free(strings->array[i]);
            }
        }
        if(strings->array != NULL)
        {
            free(strings->array);
        }
        free(strings);
    }
}

#if defined (WIN32)
#define strkscmp stricmp
#else
#define strkscmp strcasecmp
#endif

static const char COMMENT_LEADER[] = {'#', '*'};

static inline int32_t cfg_atoi(const char *str, int32_t def_value)
{
    int32_t value = def_value;

    if(!isEmpty(str))
    {
        PiSplitString *strings = split_create(str, "*");
        if(strings != NULL)
        {
            if(strings->length > 0)
            {
                int32_t i;
                char *endptr = NULL;
                for(i=0, value=1; i<strings->length; i++, endptr=NULL)
                {
                    if(isEmpty(trim(strings->array[i])))
                    {
                        value = def_value;
                        break;
                    }
                    value *= (int32_t)strtol(strings->array[i], &endptr, 0);
                #ifdef WIN32    
                    if(endptr != NULL && endptr-strings->array[i] < (int32_t)strlen(strings->array[i]))
                #else
                    if(endptr == strings->array[i] || errno == ERANGE)
                #endif
                    {
                        value = def_value;
                        break;
                    }
                }
            }
			split_free(strings);
        }
    }
    return value;
}

static inline float cfg_atof(char *str)
{
    tpString floatStr(str);
    return floatStr.toDouble();
}

static inline bool cfg_atob(char *str, bool def_value)
{
    if(isEmpty(str))
    {
        return def_value;
    }
    if(strkscmp(str, "true") == 0 || strkscmp(str, "yes") == 0 || strkscmp(str, "1") == 0)
    {
        return true;
    }
    else if(strkscmp(str, "false") == 0 || strkscmp(str, "no") == 0 || strkscmp(str, "0") == 0)
    {
        return false;
    }

    return def_value;
}

typedef struct{
    char *name;
    char *value;
    char *comment;
}KEY;

typedef struct{
    char *name;
    int32_t nbr_keys;
    KEY **keys;
    char *comment;
    
    char **key_names;
}SECTION;

typedef struct{
    char *path;
    int32_t nbr_sections;
    SECTION **sections;
    char *comment;

    char **section_names;
    bool modified;
}CFG;

static inline KEY *cfg_new_key(const char *name)
{
    KEY *k = (KEY *)malloc(sizeof(KEY));
    k->name = strdup(name);
    k->value = NULL;
    k->comment = NULL;
    return k;
}

static inline SECTION *cfg_new_section(const char *name)
{
    SECTION *s = (SECTION *)malloc(sizeof(SECTION));
    s->name = strdup(name);
    s->nbr_keys = 0;
    s->keys = NULL;
    s->comment = NULL;
    s->key_names = NULL;
    return s;
}

static inline CFG *cfg_new(const char *path)
{
    CFG *c = (CFG *)malloc(sizeof(CFG));
    
    if(path != NULL)    {
        c->path = strdup(path);
    }
    else    {
        c->path = NULL;
    }

    c->nbr_sections = 0;
    c->sections = NULL;
    c->comment = NULL;
    c->section_names = NULL;
    c->modified = false;
    
    return c;
}

static inline void cfg_free_key(KEY *k)
{
    if(k != NULL){
        if(k->name != NULL){
            free(k->name);
        }

        if(k->value != NULL){
            free(k->value);
        }

        if(k->comment != NULL){
            free(k->comment);
        }

        free(k);
    }
}

static inline void cfg_free_section(SECTION *s)
{
    if(s != NULL){
        int32_t i;
        if(s->name != NULL){
            free(s->name);
        }
        for(i=0; i<s->nbr_keys; i++){  
            cfg_free_key(s->keys[i]);
            if(s->key_names[i] != NULL){
                free(s->key_names[i]);
            }
        }

        if(s->keys != NULL){
            free(s->keys);
        }

        if(s->comment != NULL){
            free(s->comment);
        }

        if(s->key_names != NULL){
            free(s->key_names);
        }

        free(s);
    }
}

static inline void cfg_free(CFG *c)
{
    if(c != NULL){
        int32_t i;
        if(c->path != NULL){
            free(c->path);
        }

        for(i=0; i<c->nbr_sections; i++){
            cfg_free_section(c->sections[i]);
            if(c->section_names[i] != NULL){
                free(c->section_names[i]);
            }
        }

        if(c->sections != NULL){
            free(c->sections);
        }

        if(c->comment != NULL){
            free(c->comment);
        }

        if(c->section_names != NULL){
            free(c->section_names);
        }

        free(c);
    }
}

static inline KEY *cfg_get_key(SECTION *s, const char *key_name)
{
    int32_t i;
    for(i=0; i<s->nbr_keys; i++){
        if(strkscmp(key_name, s->keys[i]->name) == 0){
            return s->keys[i];
        }
    }
    return NULL;
}

static inline int32_t cfg_get_key_index(SECTION *s, const char *key_name)
{
    int32_t i;
    for(i=0; i<s->nbr_keys; i++){
        if(strkscmp(key_name, s->keys[i]->name) == 0){
            return i;
        }
    }
    return -1;
}

static inline SECTION *cfg_get_section(CFG *c, const char *section_name)
{
    int32_t i;
    for(i=0; i<c->nbr_sections; i++){
        if(strkscmp(section_name, c->sections[i]->name) == 0){
            return c->sections[i];
        }
    }
    return NULL;
}

static int32_t cfg_get_section_index(CFG *c, const char *section_name)
{
    int32_t i;
    for(i=0; i<c->nbr_sections; i++){
        if(strkscmp(section_name, c->sections[i]->name) == 0){
            return i;
        }
    }
    return -1;
}

static inline void cfg_add_key(SECTION *s, KEY *k)
{
    s->nbr_keys++;
    s->keys = (KEY **)realloc(s->keys, s->nbr_keys * sizeof(KEY *));
    s->keys[s->nbr_keys-1] = k;
    s->key_names = (char **)realloc(s->key_names, s->nbr_keys * sizeof(char *));
    s->key_names[s->nbr_keys-1] = strdup(k->name);
}

static inline void cfg_add_section(CFG *c, SECTION *s)
{
    c->nbr_sections++;
    c->sections = (SECTION **)realloc(c->sections, c->nbr_sections * sizeof(SECTION *));
    c->section_names = (char **)realloc(c->section_names, c->nbr_sections * sizeof(char *));
    if(!isEmpty(s->name)){
        c->sections[c->nbr_sections-1] = s;
        c->section_names[c->nbr_sections-1] = strdup(s->name);
    }
    else{   
        int32_t i;
        for(i=c->nbr_sections-1; i>0; i--){
            c->sections[i] = c->sections[i-1];
            c->section_names[i] = c->section_names[i-1];
        }
        c->sections[0] = s;
        c->section_names[0] = strdup(s->name);
    }
}

static bool cfg_remove_key(SECTION *s, const char *key_name)
{
    int32_t key_index = cfg_get_key_index(s, key_name);
    if(key_index >= 0 && key_index < s->nbr_keys){
        int32_t i;
        cfg_free_key(s->keys[key_index]);
        free(s->key_names[key_index]);
        
        for(i=key_index+1; i<s->nbr_keys; i++){
            s->keys[i-1] = s->keys[i];
            s->key_names[i-1] = s->key_names[i];
        }
        
        s->nbr_keys--;
        if(s->nbr_keys > 0){
            s->keys = (KEY **)realloc(s->keys, s->nbr_keys*(sizeof(KEY *)));
            s->key_names = (char **)realloc(s->key_names, s->nbr_keys*(sizeof(char *)));
        }
        else{
            free(s->keys);
            s->keys = NULL;
            free(s->key_names);
            s->key_names = NULL;
        }

        return true;
    }
    return false;
}

static inline 	bool cfg_remove_section(CFG *c, const char *section_name)
{
    int32_t section_index = cfg_get_section_index(c, section_name);    
    if(section_index >= 0 && section_index < c->nbr_sections){
        int32_t i;
        cfg_free_section(c->sections[section_index]);
        free(c->section_names[section_index]);
        
        for(i=section_index+1; i<c->nbr_sections; i++){
            c->sections[i-1] = c->sections[i];
            c->section_names[i-1] = c->section_names[i];
        }
        
        c->nbr_sections--;

        if(c->nbr_sections > 0){
            c->sections = (SECTION **)realloc(c->sections, c->nbr_sections*(sizeof(SECTION *)));
            c->section_names = (char **)realloc(c->section_names, c->nbr_sections*(sizeof(char *)));
        }
        else{
            free(c->sections);
            c->sections = NULL;
            free(c->section_names);
            c->section_names = NULL;
        }
        return true;
    }
    return false;
}

static inline void cfg_merge_key(KEY *dst, KEY *src)
{
    if(dst->value!= NULL){
        free(dst->value);
        dst->value= NULL;
    }

    if(src->value!= NULL){
        dst->value= strdup(src->value);
    }

    if(dst->comment != NULL){
        free(dst->comment);
        dst->comment = NULL;
    }

    if(src->comment != NULL){
        dst->comment = strdup(src->comment);
    }
}

static inline void cfg_merge_section(SECTION *dst, SECTION *src)
{
    int32_t i;
    KEY *k;
    for(i=0; i<src->nbr_keys; i++){
        k = cfg_get_key(dst, src->key_names[i]);
        if(k != NULL){
            cfg_merge_key(k, src->keys[i]);
        }
        else{
            k = cfg_new_key(src->key_names[i]);

            if(src->keys[i]->value != NULL){
                k->value = strdup(src->keys[i]->value);
            }

            if(src->keys[i]->comment != NULL){
                k->comment = strdup(src->keys[i]->comment);
            }

            cfg_add_key(dst, k);
        }
    }
}


typedef enum
{
    TYPE_EMPTYLINE,
    TYPE_COMMENT,
    TYPE_SECTION,
    TYPE_KEY,
    TYPE_UNKNOW,
} CFG_CONTENT_TYPE;

static inline CFG_CONTENT_TYPE cfg_get_content_type(const char *line)
{
    int32_t i;

    if(line[0] == '\0'){
        return TYPE_EMPTYLINE;
    }

    for(i=0; i<sizeof(COMMENT_LEADER); i++){
        if(line[0] == COMMENT_LEADER[i]){
            return TYPE_COMMENT;
        }
    }

    if(line[0] == '[' && line[strlen(line)-1] == ']'){
        return TYPE_SECTION;
    }

    if(strchr(line, '=') != NULL && line[0] != '='){
        return TYPE_KEY;
    }

    return TYPE_UNKNOW;
}

static inline char *cfg_read_comment(FILE *fp)
{
    char *comment = NULL;
    CFG_CONTENT_TYPE type;
    int32_t offset = ftell(fp);
    char *line = NULL;
    int32_t cmtLen = 0, lineLen = 0;
    
    while((line = trim(fgetline(fp))) != NULL)
    {
        type = cfg_get_content_type(line);
        if(type == TYPE_COMMENT){
            lineLen = (int32_t)strlen(line);
            comment = (char *)realloc(comment, cmtLen + lineLen + 1);
            memset(comment+cmtLen, 0, lineLen+1);
            strcpy(comment+cmtLen, line+1);
            comment[cmtLen+lineLen-1] = '\n';
            free(line);
            offset = ftell(fp);
            cmtLen += lineLen;
        }
        else if(type == TYPE_EMPTYLINE){
            if(isEmpty(comment)){
                //Ignore leader empty line.
            }
            else{
                cmtLen++;
                comment = (char *)realloc(comment, cmtLen+1);
                comment[cmtLen-1] = '\n';
                comment[cmtLen] = '\0';
            }
            free(line);
            offset = ftell(fp);
        }
        else{
            free(line);
            // Stop read comment and go back one line.
            fseek(fp, offset, SEEK_SET); 
            break; 
        }
    }

    if(comment != NULL && cmtLen > 0 && comment[cmtLen-1] == '\n'){
        comment[cmtLen-1] = '\0';
    }
    return comment;
}

static KEY *cfg_read_key(FILE *fp)
{
    int32_t offset = ftell(fp);
    char *comment = cfg_read_comment(fp);
    CFG_CONTENT_TYPE type;
    char *line = NULL;
    
    while((line = trim(fgetline(fp))) != NULL){
        type = cfg_get_content_type(line);
        if(type == TYPE_KEY){
            KEY *k;
            char *key_name;
            char *p = strchr(line, '=');
            *p = '\0';
            key_name = trim(line);
            k = cfg_new_key(key_name);
            k->value = trim(strdup(p+1));
            k->comment = comment;
            free(line);
            return k;
        }
        else if(type == TYPE_SECTION){
            free(line);
            if(comment != NULL){
                free(comment);
            }
            fseek(fp, offset, SEEK_SET);
            return NULL;
        }
        else{
            free(line);
            offset = ftell(fp);
            continue;
        }
    }
    
    if(comment != NULL){
        free(comment);
    }
    return NULL;
}

static char *cfg_read_section_name(FILE *fp)
{
    CFG_CONTENT_TYPE type;
    int32_t offset = ftell(fp);
    char *line = NULL;
    
    while((line = trim(fgetline(fp))) != NULL){
        type = cfg_get_content_type(line);
        if(type == TYPE_SECTION){
            int32_t len = (int32_t)strlen(line);
            char *name = (char *)malloc(len-2+1);
            memset(name, 0, len-2+1);
            strncpy(name, line+1, len-2);
            free(line);
            return name;
        }
        else if(type == TYPE_KEY){
            free(line);
            fseek(fp, offset, SEEK_SET);
            return strdup(""); // Regard as default empty section name.
        }
        else{
            free(line);
            offset = ftell(fp);
            continue;
        }
    }
    return NULL;
}

static inline SECTION *cfg_read_section(FILE *fp)
{
    SECTION *s = NULL;
    KEY *k = NULL, *existKey;
    char *comment = cfg_read_comment(fp);
    char *section_name = cfg_read_section_name(fp);
    if(section_name != NULL){
        s = cfg_new_section(section_name);
        free(section_name);
        s->comment = comment;
        
        while((k = cfg_read_key(fp)) != NULL){
            existKey = cfg_get_key(s, k->name);
            if(existKey != NULL){
                cfg_merge_key(existKey, k);
                cfg_free_key(k);
            }
            else{
                cfg_add_key(s, k);
            }
        }
        return s;
    }
    else if(comment != NULL){
        free(comment);
    }
    return NULL;
}

static inline void cfg_read(CFG *c, FILE *fp)
{
    SECTION *s = NULL, *existSection;
    char *p = NULL;
    
    c->comment = cfg_read_comment(fp);
    // Separate head comment from first section comment.
    if(c->comment != NULL && (p = strrchr(c->comment, '\n')) != NULL){
        s = cfg_read_section(fp);
        if(s != NULL){
            if(s->comment != NULL){
                //Should not occur.
                free(s->comment);
            }
            s->comment = strdup(p+1);
            *p = '\0';
        }
        cfg_add_section(c, s);
    }
    
    while((s = cfg_read_section(fp)) != NULL){
        existSection = cfg_get_section(c, s->name);
        if(existSection != NULL){
            cfg_merge_section(existSection, s);
            cfg_free_section(s);
        }
        else{
            cfg_add_section(c, s);
        }
    }
}

static inline void cfg_write_comment(char *comment, FILE *fp)
{
    PiSplitString *strings = split_create(comment, "\n");
    if(strings != NULL){
        int32_t i;
        for(i=0; i<strings->length; i++){
            if(!isEmpty(strings->array[i])){
                fputc(COMMENT_LEADER[0], fp);
                if(strings->array[i] != NULL){
                    fputs(strings->array[i], fp);
                }
            }
            fputc('\n', fp);
        }
		split_free(strings);
    }
}

static inline void cfg_write_key(KEY *k, FILE *fp)
{
    cfg_write_comment(k->comment, fp);

    fputs(k->name, fp);
    fputs("\t= ", fp);
    if(k->value != NULL){
        fputs(k->value, fp);
    }
    fputc('\n', fp);
}

static void cfg_write_section(SECTION *s, FILE *fp)
{
    int32_t i;
    cfg_write_comment(s->comment, fp);

    if(!isEmpty(s->name)){
        fputc('[', fp);
        fputs(s->name, fp);
        fputc(']', fp);
        fputc('\n', fp);
    }

    for(i=0; i<s->nbr_keys; i++){
        cfg_write_key(s->keys[i], fp);
    }
}

static inline void cfg_write(CFG *c, FILE *fp)
{
    int32_t i;
    if(c->comment != NULL){
        cfg_write_comment(c->comment, fp);
        fputc('\n', fp); // An empty line to separate config head comment from sections.
    }

    for(i=0; i<c->nbr_sections; i++){
        cfg_write_section(c->sections[i], fp);
        fputc('\n', fp); // An empty line to separate sections.
    }
}

/**
 * Open for read/write a config file. 
 * If the file is not exist, it will be created when <config_close>.
 * @param <path> The config file path. If (NULL), no file will be created/modified.
 * @return The config handler.
 */
static inline CFG *config_open(const char *path)
{
    CFG *c = cfg_new(path);
    FILE *fp;

    if(c->path != NULL){
        fp = fopen(c->path, "r");
        if(fp != NULL){
            cfg_read(c, fp);
            fclose(fp);
        }
    }
    return (CFG *)c;
}

/**
 * Load config from one file. All exist sections and key-value pairs will be deleted and the 
 * specified file's will be loaded.
 * @param <cfg> The config handler.
 * @param <path> Path of file to be loaded.
 * @return true if load success or false if any errors.
 */
static inline bool config_load(CFG *cfg, const char *path)
{
    FILE *fp;

    if(path != NULL){
        fp = fopen(path, "r");
        if(fp != NULL){
            CFG *c = (CFG *)cfg;
            int32_t i;

            for(i=0; i<c->nbr_sections; i++){
                cfg_free_section(c->sections[i]);
                free(c->section_names[i]);
            }

            if(c->sections != NULL){
                free(c->sections);
                c->sections = NULL;
            }

            if(c->comment != NULL){
                free(c->comment);
                c->comment = NULL;
            }

            if(c->section_names != NULL){
                free(c->section_names);
                c->section_names = NULL;
            }

            c->nbr_sections = 0;
            c->modified = true;
            
            cfg_read(c, fp);
            fclose(fp);
            return true;
        }
    }
    return false;
}

/**
 * Save config sections/key-value paires to one file.
 * @param <cfg> The config handler.
 * @param <path> Path of file to be saved.
 * @return true if save success or false if any errors.
 */
static inline bool config_save(CFG *cfg, const char *path)
{
    CFG *c = (CFG *)cfg;

    if(path != NULL){
        FILE *fp = fopen(path, "w+");
        if(fp != NULL){
            cfg_write(c, fp);
            fclose(fp);
            return true;
        }
    }
    return false;
}

/**
 * Close config handler and write any changes to file. 
 * If the file is not exist, it will be created here.
 * @param <cfg> The config handler.
 * @return (none).
 */
static inline void config_close(CFG *cfg)
{
    CFG *c = (CFG *)cfg;

    if(c->modified && c->path != NULL){
        FILE *fp = fopen(c->path, "w+");
        if(fp != NULL){
            cfg_write(c, fp);
            fclose(fp);
        }
    }
    cfg_free(c);
}


/**
 * Get config head comment.
 * @param <cfg> The config handler.
 * @return Head comment of config or NULL if no head comment.
 */
static inline const char *config_get_comment(CFG *cfg)
{
    CFG *c = (CFG *)cfg;
    return c->comment;
}

/**
 * Set config head comment.
 * @param <cfg> The config handler.
 * @param <comment> The comment string.
 * @return (none).
 */
static inline void config_set_comment(CFG *cfg, const char *comment)
{
    CFG *c = (CFG *)cfg;

    if(c->comment != NULL){
        free(c->comment);
        c->comment = NULL;
    }

    if(comment != NULL){
        c->comment = strdup(comment);
    }

    c->modified = true;
}

/**
 * Get number of total sections in config.
 * @param <cfg> The config handler.
 * @return The number of sections. If the config file does not define any sections (only has 
 *             key-value pair), then 1 will be returned (default section).
 */
static inline int32_t config_get_nbr_sections(CFG *cfg)
{
    CFG *c = (CFG *)cfg;
    return c->nbr_sections;
}

/**
 * Get all sections defined in config.
 * @param <cfg> The config handler.
 * @return The section's name array. If the config file does not define any sections (only has 
 *             key-value pair), then an empty string will be returned.
 */
static inline const char **config_get_sections(CFG *cfg)
{
    CFG *c = (CFG *)cfg;
    return (const char **)c->section_names;
}

/**
 * Check if the config contains such a section.
 * @param <cfg> The config handler.
 * @param <section> The section name to be check.
 * @return Whether specified section is exist in this config.
 */
static inline bool config_has_section(CFG *cfg, const char *section)
{
    CFG *c = (CFG *)cfg;
    return cfg_get_section(c, section) != NULL;
}

/**
 * Get section comment.
 * @param <cfg> The config handler.
 * @param <section> The section name which to be get comment.
 * @return Comments of section or NULL if no comment.
 */
static inline const char *config_get_section_comment(CFG *cfg, const char *section)
{
    CFG *c = (CFG *)cfg;
    SECTION *s = cfg_get_section(c, section);
    if(s != NULL){
        return s->comment;
    }
    return NULL;
}

/**
 * Set section comment. If section doest not exist, it will be created.
 * @param <cfg> The config handler.
 * @param <section> The section name which to be set comment.
 * @param <comment> The comment string.
 * @return (none).
 */
static inline void config_set_section_comment(CFG *cfg, const char *section, const char *comment)
{
    CFG *c = (CFG *)cfg;
    SECTION *s = cfg_get_section(c, section);
    if(s != NULL){
        if(s->comment != NULL){
            free(s->comment);
            s->comment = NULL;
        }
        if(comment != NULL){
            s->comment = strdup(comment);
        }
    } 
    else
    {
        s = cfg_new_section(section);
        if(comment != NULL){
            s->comment = strdup(comment);
        }
        cfg_add_section(c, s);
    }
    c->modified = true;
}


/**
 * Get number of total keys in config-section.
 * @param <cfg> The config handler.
 * @param <section> The section name which to be get key count.
 * @return The number of keys in specified section or -1 if no such section.
 */
static inline int32_t config_get_nbr_keys(CFG *cfg, const char *section)
{
    CFG *c = (CFG *)cfg;
    SECTION *s = cfg_get_section(c, section);
    if(s != NULL){
        return s->nbr_keys;
    }
    return -1;
}

/**
 * Get all keys in config-section.
 * @param <cfg> The config handler.
 * @param <section> The section name which to be get keys.
 * @return The keys array or NULL if no such section.
 */
static inline const char **config_get_keys(CFG *cfg, const char *section)
{
    CFG *c = (CFG *)cfg;
    SECTION *s = cfg_get_section(c, section);
    if(s != NULL){
        return (const char **)s->key_names;
    }
    return NULL;
}

/**
 * Check if the config-section contains the key.
 * @param <cfg> The config handler.
 * @param <section> The section name in which to check key.
 * @param <key> The key to be check.
 * @return Whether the key is exist in this config-section.
 */
static inline bool config_has_key(CFG *cfg, const char *section, const char *key)
{
    CFG *c = (CFG *)cfg;
    SECTION *s = cfg_get_section(c, section);
    if(s != NULL){
        return cfg_get_key(s, key) != NULL;
    }
    return false;
}

/**
 * Delete a section in config. All keys belong to this section will be deleted at the same time.
 * @param <cfg> The config handler.
 * @param <section> The section to be deleted.
 * @return true if the section exist and has been deleted else false.
 */
static inline bool config_delete_section(CFG *cfg, const char *section)
{
    CFG *c = (CFG *)cfg;
    bool removed = cfg_remove_section(c, section);
    if(removed){
        c->modified = true;
    }
    return removed;
}

/**
 * Delete the key in config-section.
 * @param <cfg> The config handler.
 * @param <section> The section in which to delete key.
 * @param <key> The key to be deleted.
 * @return true if the key exist and has been deleted else false.
 */
static inline bool config_delete_key(CFG *cfg, const char *section, const char *key)
{
    CFG *c = (CFG *)cfg;
    SECTION *s = cfg_get_section(c, section);

    if(s != NULL){
        bool removed = cfg_remove_key(s, key);
        if(removed){
            c->modified = true;
        }
        return removed;
    }
    return false;
}

/**
 * Delete value combined with the key, the key is still exist but it's value is empty.
 * Therefor if you go to get it's value later, the default value is always returned.
 * @param <cfg> The config handler.
 * @param <section> The section in which to delete key value.
 * @param <key> The key to be delete it's value.
 * @return true if the key exist and value has been deleted else false.
 */
static inline bool config_delete_value(CFG *cfg, const char *section, const char *key)
{
    CFG *c = (CFG *)cfg;
    SECTION *s = cfg_get_section(c, section);

    if(s != NULL){
        KEY *k = cfg_get_key(s, key);
        if(k != NULL){
            if(k->value != NULL){
                free(k->value);
                k->value = NULL;
            }
            c->modified = true;
            return true;
        }
    }
    return false;
}

/**
 * Get key comment.
 * @param <cfg> The config handler.
 * @param <section> The section name in which to get comment.
 * @param <key> The key to be get its comment.
 * @return Comments of key or NULL if no comment or section/key is not exist.
 */
static inline const char *config_get_key_comment(CFG *cfg, const char *section, const char *key)
{
    CFG *c = (CFG *)cfg;
    SECTION *s = cfg_get_section(c, section);

    if(s != NULL){
        KEY *k = cfg_get_key(s, key);
        if(k != NULL){
            return k->comment;
        }
    }
    return NULL;
}

/**
 * Get a string type value in config.
 * @param <cfg> The config handler.
 * @param <section> The section in which to get key value.
 * @param <key> The key to be get its value.
 * @param <default_value> If the section/key is not exist, this default value will be returned.
 * @return The string type value combined with the key.
 */
static inline const char *config_get_value_string(CFG *cfg, const char *section, const char *key, const char *default_value)
{
    CFG *c = (CFG *)cfg;
    SECTION *s = cfg_get_section(c, section);

    if(s != NULL){
        KEY *k = cfg_get_key(s, key);
        if(k != NULL && k->value != NULL){
            return k->value;
        }
    }
    return default_value;
}

/**
 * Get an integer value in config.
 * @param <cfg> The config handler.
 * @param <section> The section in which to get key value.
 * @param <key> The key to be get its value.
 * @param <default_value> If the section/key is not exist or the value is not an integer, 
 *                                     this default value will be returned.
 * @return The integer value combined with the key.
 */
static inline int32_t config_get_value_int(CFG *cfg, const char *section, const char *key, int32_t default_value)
{
    CFG *c = (CFG *)cfg;
    SECTION *s = cfg_get_section(c, section);

    if(s != NULL){
        KEY *k = cfg_get_key(s, key);
        if(k != NULL && k->value != NULL){
            return cfg_atoi(k->value, default_value);
        }
    }
    return default_value;
}

/**
 * Get a floating point value in config.
 * @param <cfg> The config handler.
 * @param <section> The section in which to get key value.
 * @param <key> The key to be get its value.
 * @param <default_value> If the section/key is not exist or the value is not a floating 
 *                                     number, this default value will be returned.
 * @return The float value combined with the key.
 */
static inline float config_get_value_float(CFG *cfg, const char *section, const char *key, float default_value)
{
    CFG *c = (CFG *)cfg;
    SECTION *s = cfg_get_section(c, section);

    if(s != NULL){
        KEY *k = cfg_get_key(s, key);
        if(k != NULL && k->value != NULL){
            return cfg_atof(k->value);
        }
    }
    return default_value;
}

/**
 * Get a boolean value in config.
 * @param <cfg> The config handler.
 * @param <section> The section in which to get key value.
 * @param <key> The key to be get its value.
 * @param <default_value> If the section/key is not exist or the value string is not a 
 *                                     canonical boolean expression, this default value will be returned.
 * @return The boolean value combined with the key.
 */
static inline bool config_get_value_bool(CFG *cfg, const char *section, const char *key, bool default_value)
{
    CFG *c = (CFG *)cfg;
    SECTION *s = cfg_get_section(c, section);

    if(s != NULL){
        KEY *k = cfg_get_key(s, key);
        if(k != NULL && k->value != NULL){
            return cfg_atob(k->value, default_value);
        }
    }
    return default_value;
}

/**
 * Set key comment. If the section/key doest not exist, they will be created.
 * @param <cfg> The config handler.
 * @param <section> The section name in which to set comment.
 * @param <key> The key to be set its comment.
 * @param <comment> The comment string.
 * @return (none).
 */
static inline void config_set_key_comment(CFG *cfg, const char *section, const char *key, const char *comment)
{
    CFG *c = (CFG *)cfg;
    SECTION *s;
    KEY *k;

    if(isEmpty(key)){
        return;
    }

    s = cfg_get_section(c, section);
    if(s == NULL){
        s = cfg_new_section(section);
        cfg_add_section(c, s);
    }
    
    k = cfg_get_key(s, key);

    if(k == NULL){
        k = cfg_new_key(key);
        cfg_add_key(s, k);
    }
    
    if(k->comment != NULL){
        free(k->comment);
        k->comment = NULL;
    }

    if(comment != NULL){
        k->comment = strdup(comment);
    }
    c->modified = true;
}

/**
 * Set a string type key value. If the section/key doest not exist, they will be created.
 * @param <cfg> The config handler.
 * @param <section> The section in which to set key value.
 * @param <key> The key to be set its value.
 * @value The value to be set.
 * @return (none).
 */
static inline void config_set_value_string(CFG *cfg, const char *section, const char *key, const char *value)
{
    CFG *c = (CFG *)cfg;
    SECTION *s;
    KEY *k;

    if(isEmpty(key)){
        return;
    }

    s = cfg_get_section(c, section);

    if(s == NULL){
        s = cfg_new_section(section);
        cfg_add_section(c, s);
    }
    
    k = cfg_get_key(s, key);

    if(k == NULL){
        k = cfg_new_key(key);
        cfg_add_key(s, k);
    }
    
    if(k->value != NULL){
        free(k->value);
        k->value = NULL;
    }

    if(value != NULL){
        k->value = strdup(value);
    }

    c->modified = true;
}

/**
 * Set an integer key value. If the section/key doest not exist, they will be created.
 * @param <cfg> The config handler.
 * @param <section> The section in which to set key value.
 * @param <key> The key to be set its value.
 * @value The value to be set.
 * @return (none).
 */
static inline void config_set_value_int(CFG *cfg, const char *section, const char *key, int32_t value)
{
    CFG *c = (CFG *)cfg;
    SECTION *s;
    KEY *k;

    if(isEmpty(key)){
        return;
    }

    s = cfg_get_section(c, section);

    if(s == NULL){
        s = cfg_new_section(section);
        cfg_add_section(c, s);
    }
    
    k = cfg_get_key(s, key);

    if(k == NULL){
        k = cfg_new_key(key);
        cfg_add_key(s, k);
    }
    
    if(k->value != NULL){
        free(k->value);
        k->value = NULL;
    }

    k->value = (char *)malloc(32);
    memset(k->value, 0, 32);
    sprintf(k->value, "%d", value);
    c->modified = true;
}

/**
 * Set a floating point key value. If the section/key doest not exist, they will be created.
 * @param <cfg> The config handler.
 * @param <section> The section in which to set key value.
 * @param <key> The key to be set its value.
 * @value The value to be set.
 * @return (none).
 */
static inline void config_set_value_float(CFG *cfg, const char *section, const char *key, float value)
{
    CFG *c = (CFG *)cfg;
    SECTION *s;
    KEY *k;

    if(isEmpty(key)){
        return;
    }
    
    s = cfg_get_section(c, section);

    if(s == NULL){
        s = cfg_new_section(section);
        cfg_add_section(c, s);
    }
    
    k = cfg_get_key(s, key);

    if(k == NULL){
        k = cfg_new_key(key);
        cfg_add_key(s, k);
    }
    
    if(k->value != NULL){
        free(k->value);
        k->value = NULL;
    }

    k->value = (char *)malloc(32);
    memset(k->value, 0, 32);
    sprintf(k->value, "%f", value);
    c->modified = true;
}

/**
 * Set a boolean key value. If the section/key doest not exist, they will be created.
 * @param <cfg> The config handler.
 * @param <section> The section in which to set key value.
 * @param <key> The key to be set its value.
 * @value The value to be set.
 * @return (none).
 */
static inline void config_set_value_bool(CFG *cfg, const char *section, const char *key, bool value)
{
    config_set_value_string(cfg, section, key, value ? "true" : "false");
}

tpConfig::tpConfig():
	tpConfigSet(nullptr)
{

}

tpConfig::~tpConfig()
{
	CFG *set = (CFG*)this->tpConfigSet;
	
	if(set){
		config_close(set);
		//delete set;
	}
}

bool tpConfig::readFrom(const char *cfgFile)
{
	CFG *set = (CFG*)this->tpConfigSet;
	
	if(set){
		config_close(set);	
		delete set;
	}
	
	return ((this->tpConfigSet = config_open(cfgFile)) != nullptr);
}

bool tpConfig::saveAs(const char *cfgFile)
{
	CFG *set = (CFG*)this->tpConfigSet;
	bool ret = false;
	
	if(set){
		ret = config_save(set, cfgFile);
	}
	
	return ret;
}

bool tpConfig::save()
{
	CFG *set = (CFG*)this->tpConfigSet;
	bool ret = false;
	
	if(set){
		ret = config_save(set, set->path);
	}
	
	return ret;
}

const char *tpConfig::getComment()
{
	CFG *set = (CFG*)this->tpConfigSet;
	const char *comment = nullptr;
	
	if(set){
		comment = config_get_comment(set);
	}
	
	return comment;
}

void tpConfig::setComment(const char *comment)
{
	CFG *set = (CFG*)this->tpConfigSet;
	
	if(set){
		config_set_comment(set, comment);
	}
}

int32_t tpConfig::getNbrSections()
{
	CFG *set = (CFG*)this->tpConfigSet;
	int32_t numbers = 0;
	
	if(set){
		numbers = config_get_nbr_sections(set);
	}
	
	return numbers;
}

const char **tpConfig::getAllSections()
{
	CFG *set = (CFG*)this->tpConfigSet;
	const char **sections = nullptr;
	
	if(set){
		sections = config_get_sections(set);
	}
	
	return sections;
}

bool tpConfig::hasSections(const char *section)
{
	CFG *set = (CFG*)this->tpConfigSet;
	bool ret = false;
	
	if(set){
		ret = config_has_section(set, section);
	}
	
	return ret;
}

const char *tpConfig::getSectionComment(const char *section)
{
	CFG *set = (CFG*)this->tpConfigSet;
	const char *comment = nullptr;
	
	if(set){
		comment = config_get_section_comment(set, section);
	}
	
	return comment;
}

void tpConfig::setSectionComment(const char *section, const char *comment)
{
	CFG *set = (CFG*)this->tpConfigSet;
	
	if(set){
		config_set_section_comment(set, section, comment);
	}
}

int32_t tpConfig::getNbrKeys(const char *section)
{
	CFG *set = (CFG*)this->tpConfigSet;
	int32_t numbers = 0;
	
	if(set){
		numbers = config_get_nbr_keys(set, section);
	}
	
	return numbers;
}

const char **tpConfig::getKeys(const char *section)
{
	CFG *set = (CFG*)this->tpConfigSet;
	const char **keys = nullptr;
	
	if(set){
		keys = config_get_keys(set, section);
	}
	
	return keys;
}

bool tpConfig::hasKeys(const char *section, const char *key)
{
	CFG *set = (CFG*)this->tpConfigSet;
	bool ret = false;
	
	if(set){
		ret = config_has_key(set, section, key);
	}
	
	return ret;
}

bool tpConfig::deleteSeciton(const char *section)
{
	CFG *set = (CFG*)this->tpConfigSet;
	bool ret = false;
	
	if(set){
		ret = config_delete_section(set, section);
	}
	
	return ret;
}

bool tpConfig::deleteKey(const char *section, const char *key)
{
	CFG *set = (CFG*)this->tpConfigSet;
	bool ret = false;
	
	if(set){
		ret = config_delete_key(set, section, key);
	}
	
	return ret;
}

bool tpConfig::deleteVaule(const char *section, const char *key)
{
	CFG *set = (CFG*)this->tpConfigSet;
	bool ret = false;
	
	if(set){
		ret = config_delete_value(set, section, key);
	}
	
	return ret;
}

const char *tpConfig::getKeyComment(const char *section, const char *key)
{
	CFG *set = (CFG*)this->tpConfigSet;
	const char *comment = nullptr;
	
	if(set){
		comment = config_get_key_comment(set, section, key);
	}
	
	return comment;
}

void tpConfig::setKeyComment(const char *section, const char *key, const char *comment)
{
	CFG *set = (CFG*)this->tpConfigSet;
	
	if(set){
		config_set_key_comment(set, section, key, comment);
	}
}

const char *tpConfig::keyValueAsString(const char *section, const char *key)
{
	CFG *set = (CFG*)this->tpConfigSet;
	const char *value = nullptr;
	
	if(set){
		value = config_get_value_string(set, section, key, nullptr);
	}
	
	return value;
}

int32_t tpConfig::keyValueAsInteger(const char *section, const char *key)
{
	CFG *set = (CFG*)this->tpConfigSet;
	int32_t value = -1;
	
	if(set){
		value = config_get_value_int(set, section, key, -1);
	}
	
	return value;
}

float tpConfig::keyValueAsFloat(const char *section, const char *key)
{
	CFG *set = (CFG*)this->tpConfigSet;
	float value = 0.0;
	
	if(set){
		value = config_get_value_float(set, section, key, 0.0);
	}
	
	return value;
}

bool tpConfig::keyValueAsBool(const char *section, const char *key)
{
	CFG *set = (CFG*)this->tpConfigSet;
	bool value = false;
	
	if(set){
		value = config_get_value_bool(set, section, key, false);
	}
	
	return value;
}

void tpConfig::setKeyValueAsString(const char *section, const char *key, const char *value)
{
	CFG *set = (CFG*)this->tpConfigSet;
	
	if(set){
		config_set_value_string(set, section, key, value);
	}
}

void tpConfig::setKeyValueAsInteger(const char *section, const char *key, int32_t value)
{
	CFG *set = (CFG*)this->tpConfigSet;
	
	if(set){
		config_set_value_int(set, section, key, value);
	}
}

void tpConfig::setKeyValueAsFloat(const char *section, const char *key, float value)
{
	CFG *set = (CFG*)this->tpConfigSet;
	
	if(set){
		config_set_value_float(set, section, key, value);
	}
}

void tpConfig::setKeyAsBool(const char *section, const char *key, bool value)
{
	CFG *set = (CFG*)this->tpConfigSet;
	
	if(set){
		config_set_value_bool(set, section, key, value);
	}
}
