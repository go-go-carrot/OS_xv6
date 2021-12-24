
struct spinlock{
 uint locked;
 char *name;
 struct cpu *cpu;
 uint pcs[10];
};


