#ifndef MAUNGA_HEADER
#define MAUNGA_HEADER

#define INPUT  1
#define OUTPUT 2

#define CELL int
#define DROP vm.data[vm.sp] = 0; vm.sp--; if (vm.sp < 0) vm.ip = IMAGE_SIZE;
#define TOS  vm.data[vm.sp]
#define NOS  vm.data[vm.sp-1]

#define TORS vm.address[vm.rsp]
#define NORS vm.address[vm.rsp-1]

#define VMOP vm.image[vm.ip]

#define IMAGE_SIZE   5000000
#define STACK_DEPTH      100
#define ADDRESSES       1024

typedef struct {
  int sp;
  int rsp;
  int ip;
  int data[STACK_DEPTH];
  int address[ADDRESSES];
  int ports[1024];
  int image[IMAGE_SIZE];
  int padding;
  char filename[2048];
} VM_STATE;


enum vm_opcode {VM_NOP, VM_LIT, VM_DUP, VM_DROP, VM_SWAP, VM_PUSH, VM_POP,
                VM_CALL, VM_JUMP, VM_RETURN, VM_GT_JUMP, VM_LT_JUMP,
                VM_NE_JUMP,VM_EQ_JUMP, VM_FETCH, VM_STORE, VM_ADD,
                VM_SUB, VM_MUL, VM_DIVMOD, VM_AND, VM_OR, VM_XOR, VM_SHL,
                VM_SHR, VM_ZERO_EXIT, VM_INC, VM_DEC, VM_IN, VM_OUT,
                VM_WAIT };

#define NUM_OPS VM_WAIT
#endif
