/*
    Some much needed header for common constants
*/

//Got these from some random site: Hope they are right
//https://www.keil.com/pack/doc/cmsis/Core_A/html/group__CMSIS__CPSR__M.html#details

#define CPSR_M_ABT   0x17U
#define CPSR_M_FIQ   0x11U
#define CPSR_M_HYP   0x1AU
#define CPSR_M_IRQ   0x12U
#define CPSR_M_MON   0x16U
#define CPSR_M_SVC   0x13U
#define CPSR_M_SYS   0x1FU
#define CPSR_M_UND   0x1BU
#define CPSR_M_USR   0x10U

#define CPSR_N_FLAG 0x80000000
#define CPSR_Z_FLAG 0x40000000
#define CPSR_C_FLAG 0x20000000
#define CPSR_V_FLAG 0x10000000
#define CPSR_Q_FLAG 0x08000000
#define CPSR_IT_FLAG 0x06000000
#define CPSR_J_FLAG 0x01000000
#define CPSR_GE_FLAG 0x000f0000
#define CPSR_IT_FLAG 0x0000fc00
#define CPSR_E_FLAG 0x00000200
#define CPSR_A_FLAG 0x00000100
#define CPSR_I_FLAG 0x00000080
#define CPSR_F_FLAG 0x00000040
#define CPSR_T_FLAG 0x00000020
#define CPSR_M_FLAG 0x0000001f