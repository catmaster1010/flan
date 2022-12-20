#ifndef  lock_h
#define  lock_h
typedef struct {
    int lock;
} spinlock_t;
int spinlock_test_aquire(spinlock_t*  lock);
void spinlock_aquire(spinlock_t* lock);
void spinlock_release(spinlock_t* lock);
#endif