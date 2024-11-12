/* fork.c - fork, newpid */

#include <xinu.h>
//#define DEBUG
pid32 newpid(void);

#ifdef DEBUG
local   int newpid();

/*------------------------------------------------------------------------
 *  newpid  -  Obtain a new (free) process ID
 *------------------------------------------------------------------------
 */
pid32   newpid(void)
{
        uint32  i;                      /* Iterate through all processes*/
        static  pid32 nextpid = 1;      /* Position in table to try or  */
                                        /*   one beyond end of table    */

        /* Check all NPROC slots */

        for (i = 0; i < NPROC; i++) {
                nextpid %= NPROC;       /* Wrap around to beginning */
                if (proctab[nextpid].prstate == PR_FREE) {
                        return nextpid++;
                } else {
                        nextpid++;
                }
        }
        return (pid32) SYSERR;
}
#endif
/*------------------------------------------------------------------------
 *  fork  -  fork a process to start running a function on x86
 *------------------------------------------------------------------------
 */
pid32   fork()
        //void                *funcaddr,      /* Address of the function      */
        //uint32      ssize,          /* Stack size in bytes          */
        //pri16               priority,       /* Process priority > 0         */
        //char                *name,          /* Name (for debugging)         */
        //uint32      nargs,          /* Number of args that follow   */
{
    uint32  ssize;          /* Stack size in bytes          */
    pri16           priority;       /* Process priority > 0         */
    uint32          savsp;//, *pushsp;
    intmask         mask;           /* Interrupt mask               */
    pid32           pid;            /* Stores new process id        */
    struct  procent *prptr;         /* Pointer to proc. table entry */
    int32           i;
//  uint32          *a;             /* Points to list of args       */
    uint32          *saddr;         /* Stack address                */
    struct procent *parentptr;   /* Pointer to parent in proc. table entry */

    parentptr = &proctab[currpid];
    ssize = parentptr->prstklen;
    priority = parentptr->prprio;
    mask = disable();
    if (ssize < MINSTK)
            ssize = MINSTK;
    ssize = (uint32) roundmb(ssize);
    if ( (priority < 1) || ((pid=newpid()) == SYSERR) ||
            ((saddr = (uint32 *)getstk(ssize)) == (uint32 *)SYSERR) ) {
            restore(mask);
            return SYSERR;
    }

    prcount++;
    prptr = &proctab[pid];

    /* Initialize process table entry for new process */
    prptr->prstate = PR_SUSP;       /* Initial state is suspended   */
    prptr->prprio = priority;
    prptr->prstkbase = (char *)saddr;
    prptr->prstklen = ssize;
    prptr->prname[PNMLEN-1] = NULLCH;
    for (i=0 ; i<PNMLEN-1 && (prptr->prname[i]=parentptr->prname[i])!=NULLCH; i++)
            ;
    prptr->prsem = -1;
    prptr->prparent = currpid; //(pid32)getpid();
    prptr->prhasmsg = FALSE;
    //prptr->user_process = FALSE;

    /* Set up stdin, stdout, and stderr descriptors for the shell   */
    prptr->prdesc[0] = CONSOLE;
    prptr->prdesc[1] = CONSOLE;
    prptr->prdesc[2] = CONSOLE;

    /* Initialize stack as if the process was called                */

    *saddr = STACKMAGIC;
    savsp = (uint32)saddr;

    uint32 *sp, *bp;
    asm("movl %%esp, %0\n" :"=r"(sp));  /*  ESP         */
    asm("movl %%ebp, %0\n" :"=r"(bp));  /*  EBP         */

    uint32 *parent_curr_addr, *child_curr_addr, *child_addr;
    uint32 offset;

    child_curr_addr = (uint32 *)prptr->prstkbase;
    parent_curr_addr = (uint32 *)parentptr->prstkbase;
    offset = (uint32)parent_curr_addr - (uint32)child_curr_addr;

    /* Push arguments */
    while (parent_curr_addr >= bp)
    {
        *child_curr_addr = *parent_curr_addr;
        child_curr_addr--;
        parent_curr_addr--;
#ifdef DEBUG
        kprintf("*child_curr_addr = %X, child_curr_addr = %X, *parent_curr_addr = %X, parent_curr_addr = %X \n",*child_curr_addr, child_curr_addr, *parent_curr_addr, parent_curr_addr);
#endif
    }
    //a = (uint32 *)(&nargs + 1);   /* Start of args                */
    //a += nargs -1;                        /* Last argument                */
    //for ( ; nargs > 0 ; nargs--)  /* Machine dependent; copy args */
    //      *--saddr = *a--;        /* onto created process's stack */
    //*--saddr = (long)INITRET;     /* Push on return address       */
    parent_curr_addr++;
    child_curr_addr++;
    child_addr = child_curr_addr;
    /* The following entries on the stack must match what ctxsw     */
    /*   expects a saved process state to contain: ret address,     */
    /*   ebp, interrupt mask, flags, registers, and an old SP       */
    while(child_addr < (uint32 *)prptr->prstkbase)
    {
        *child_addr = *child_addr - (uint32)offset;
        child_addr = (uint32 *)*child_addr;
#ifdef DEBUG
        kprintf("*child_addr = %X, child_addr = %X, offset = %X\n",*child_addr, child_addr, offset);
#endif
    }

#ifdef DEBUG
    stacktrace(currpid);
#endif

    //*--saddr = (long)funcaddr;    /* Make the stack look like it's*/
                                    /*   half-way through a call to */
                                    /*   ctxsw that "returns" to the*/
                                    /*   new process                */
    //*--saddr = savsp;             /* This will be register ebp    */
                                    /*   for process exit           */
    //savsp = (uint32) saddr;               /* Start of frame for ctxsw     */
    *--child_curr_addr = 0x00000200;                /* New process runs with        */
                                    /*   interrupts enabled         */

    /* Basically, the following emulates an x86 "pushal" instruction*/

    *--child_curr_addr = NPROC;                     /* %eax */
    *--child_curr_addr = 0;                 /* %ecx */
    *--child_curr_addr = 0;                 /* %edx */
    *--child_curr_addr = 0;                 /* %ebx */
    *--child_curr_addr = 0;                 /* %esp; value filled in below  */
    //pushsp = child_curr_addr;                     /* Remember this location       */
    *--child_curr_addr = savsp;             /* %ebp (while finishing ctxsw) */
    *--child_curr_addr = 0;                 /* %esi */
    *--child_curr_addr = 0;                 /* %edi */

    prptr->prstkptr = (char *)child_curr_addr;
    prptr->prstate = PR_READY;
    //pushsp = (unsigned long) (prptr->prstkptr = (char *)child_curr_addr);
    restore(mask);
    insert(pid, readylist, prptr->prprio);
    return pid;
}


