#ifndef INTERACTABLES_H_
#define INTERACTABLES_H_

#include "raylib.h"

typedef enum {
    Interactable_Kind_AMMO,
    Interactable_Kind_DOCUMENT,
    Interactable_Kind_WEAPON,
    Interactable_Kind_COUNT
} Interactable_Kind;

typedef struct {
    Interactable_Kind kind;
    short info_index;
    short amount;
} Interactable;

typedef struct {
    const char *title;
    const char *text;
} Interactable_Info_Document;

#define INTERACTABLE_INFO_DOCUMENT_COUNT 1
extern const Interactable_Info_Document DOCUMENT_INFOS[INTERACTABLE_INFO_DOCUMENT_COUNT];

#endif
