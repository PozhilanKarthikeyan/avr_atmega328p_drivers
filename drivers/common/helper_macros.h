#ifndef HELPER_MACROS
#define HELPER_MACROS

#define SET_BIT(REG,POSITION)     ((REG) |= 1<<(POSITION))
#define CLEAR_BIT(REG,POSITION)   ((REG) &= ~(1<<(POSITION)))
#define READ_BIT(REG,POSITION)    (((REG)>>(POSITION))&0X01)

#endif