#ifndef VIEW_H_
#define VIEW_H_

#define DEFINE_VIEW_FOR_TYPE_WITH_NAME(type, name) typedef struct { int begin, end; type *data; } View_ ## name
#define DEFINE_VIEW_FOR_TYPE(type) DEFINE_VIEW_FOR_TYPE_WITH_NAME(type, type)

#ifdef bool
    #undef bool
    DEFINE_VIEW_FOR_TYPE_WITH_NAME(_Bool, bool);
    #define bool _Bool
#else
    DEFINE_VIEW_FOR_TYPE_WITH_NAME(_Bool, bool);
#endif

DEFINE_VIEW_FOR_TYPE(char);
DEFINE_VIEW_FOR_TYPE(int);
DEFINE_VIEW_FOR_TYPE(float);
DEFINE_VIEW_FOR_TYPE_WITH_NAME(char *, String);

#endif
