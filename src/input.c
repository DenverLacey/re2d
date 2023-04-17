#include "input.h"

void input_set_flags(Input *input, Input_Flags flags) {
    input->flags |= flags;
}

void input_set_flags_if(Input *input, bool condition, Input_Flags flags) {
    if (condition) {
        input_set_flags(input, flags);
    } else {
        input->flags &= ~flags;
    }
}

bool input_is_flags_set(Input *input, Input_Flags flags) {
    return (input->flags & flags) == flags;
}
