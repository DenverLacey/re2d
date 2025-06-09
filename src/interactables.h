#ifndef INTERACTABLES_H_
#define INTERACTABLES_H_

#include <raylib.h>

typedef enum {
    Interactable_Kind_AMMO,
    Interactable_Kind_DOCUMENT,
    Interactable_Kind_WEAPON,
    Interactable_Kind_KEY,
    Interactable_Kind_COUNT
} Interactable_Kind;

// WARNING: Ammo kinds must be alphabetic order. Parts of the code rely on this fact.
typedef enum {
    Ammo_Kind_HANDGUN,
    Ammo_KIND_COUNT
} Ammo_Kind;

// WARNING: Weapon kinds must be alphabetic order. Parts of the code rely on this fact.
typedef enum {
    Weapon_Kind_HANDGUN,
    Weapon_Kind_COUNT
} Weapon_Kind;

// WARNING: Key kinds must be alphabetic order. Parts of the code rely on this fact.
typedef enum {
    Key_Kind_CLUBS,
    Key_Kind_DIAMONDS,
    Key_Kind_HEARTS,
    Key_Kind_SPADES,
    Key_Kind_COUNT
} Key_Kind;

typedef struct {
    Interactable_Kind kind;
    int amount;
    union {
        int info_index;
        int specific_kind;
    };
} Interactable;

typedef struct {
    const char *title;
    const char *text;
} Interactable_Info_Document;

#define INTERACTABLE_INFO_DOCUMENT_COUNT 1
extern const Interactable_Info_Document DOCUMENT_INFOS[INTERACTABLE_INFO_DOCUMENT_COUNT];

#endif
