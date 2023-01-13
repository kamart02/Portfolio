#include "string.h"
#include "error.h"

String create_string(void) {
    String str;
    str = safe_malloc(sizeof(struct string));
    str->chars = safe_malloc(sizeof(char) * 24); 
    str->size = 0;
    str->max_size = 24;
    str->chars[0] = '\0';

    return str;
}

void string_insert(String str, char letter) {
    if (str->size == str->max_size - 1) {
        str->max_size *= 2;
        str->chars = safe_realloc(str->chars, str->max_size * sizeof(char));
        
    }
    str->chars[str->size] = letter;
    str->size++;
    str->chars[str->size] = '\0';

    return;
}

void free_string(String str) {
    if(str != NULL){
        if(str->chars != NULL){
            free(str->chars);
        }
        free(str);
    }
    
    return;
}