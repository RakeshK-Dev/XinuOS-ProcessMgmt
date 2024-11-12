/*  main.c  */

#include <xinu.h>
#include <stdarg.h>
//#define DEBUG

process test1();
process test2();
process test3();
process test4();
process spawn(uint32 n);

pid32 pid1, pid2, pid3, pid4, pid5, pid6, pid7, pid8, pid9, pid10, pid11, pid12, pid13;

void sync_printf(char *fmt, ...)
{
    intmask mask = disable();
    void *arg = __builtin_apply_args();
    __builtin_apply((void*)kprintf, arg, 100);
    restore(mask);
}

process child1_test2()
{
        sync_printf("child1_test2 : pid5 %d\n", pid5);
        sleep(30);
        return OK;
}

process child2_test2()
{
        sync_printf("child2_test2 : pid6 %d\n", pid6);
        sleep(30);
        return OK;
}

process child3_test2()
{
        sync_printf("child3_test2 : pid7 %d\n", pid7);
        sleep(30);
        return OK;
}

process child1_test3()
{
        sync_printf("child1_test3 : pid8 %d\n", pid8);
        sleep(30);
        return OK;
}

process child2_test3()
{
        sync_printf("child2_test3 : pid9 %d\n", pid9);
        sleep(30);
        return OK;
}

process child3_test3()
{
        sync_printf("child3_test3 : pid10 %d\n", pid10);
        sleep(30);
        return OK;
}

process child1_test4()
{
        sync_printf("child1_test4 : pid11 %d\n", pid11);
        sleep(30);
        return OK;
}

process child2_test4()
{
        sync_printf("child2_test4 : pid12 %d\n", pid12);
        sleep(30);
        return OK;
}

process child3_test4()
{
        sync_printf("child3_test4 : pid13 %d\n", pid13);
        sleep(30);
        return OK;
}

process test1()
{
    sync_printf("test1 : pid1 %d\n", pid1);

    pid2 = create(test2, 8192, 50, "test2", 1, CONSOLE);
    proctab[pid2].user_process = TRUE;
    resume(pid2);

    pid3 = create(test3, 8192, 50, "test3", 1, CONSOLE);
    proctab[pid3].user_process = TRUE;
    resume(pid3);

    pid4 = create(test4, 8192, 50, "test4", 1, CONSOLE);
    proctab[pid4].user_process = TRUE;
    resume(pid4);

    sleep(20);

    return OK;
}

process test2()
{
    sync_printf("test2 : pid2 %d\n", pid2);

    pid5 = create(child1_test2, 8192, 50, "child1_test2", 1, CONSOLE);
    proctab[pid5].user_process = TRUE;
    resume(pid5);

    pid6 = create(child2_test2, 8192, 50, "child2_test2", 1, CONSOLE);
    proctab[pid6].user_process = TRUE;
    resume(pid6);

        pid7 = create(child3_test2, 8192, 50, "child3_test2", 1, CONSOLE);
    proctab[pid7].user_process = TRUE;
    resume(pid7);

    sleep(20);
    return OK;
}

process test3()
{
    sync_printf("test3 : pid3 %d\n", pid3);
    pid8 = create(child1_test3, 8192, 50, "child1_test3", 1, CONSOLE);
    proctab[pid8].user_process = TRUE;
    resume(pid8);

    pid9 = create(child2_test3, 8192, 50, "child2_test3", 1, CONSOLE);
    proctab[pid9].user_process = TRUE;
    resume(pid9);

    pid10 = create(child3_test3, 8192, 50, "child3_test3", 1, CONSOLE);
    proctab[pid10].user_process = TRUE;
    resume(pid10);
    sleep(20);
    return OK;
}

process test4()
{
    sync_printf("test4 : pid4 %d\n", pid4);
    pid11 = create(child1_test4, 8192, 50, "child1_test4", 1, CONSOLE);
    proctab[pid11].user_process = TRUE;
    resume(pid11);

    pid12 = create(child2_test4, 8192, 50, "child2_test4", 1, CONSOLE);
    proctab[pid12].user_process = TRUE;
    resume(pid12);

    pid13 = create(child3_test4, 8192, 50, "child3_test4", 1, CONSOLE);
    proctab[pid13].user_process = TRUE;
    resume(pid13);
    sleep(20);
    return OK;
}

void print_active_processes()
{
    struct procent *prptr;
    uint32 i = 0, j = 0;
    intmask mask = disable();

    sync_printf("Start\n");

    for (i = 0; i < NPROC; i++) {
        prptr = &proctab[i];

        if ((prptr->prstate == PR_FREE) || (proctab[i].user_process == FALSE)) {
            continue;
        }
        if(proctab[i].user_process){
        sync_printf(" P%d:: ", i);

        for (j = 0; j < NPROC; j++) {
            prptr = &proctab[j];

            if ((prptr->prstate != PR_FREE) && ((prptr->prparent) == i)) {
                sync_printf(" %d  ", j);
            }
        }

        sync_printf("\n");}
    }

    sync_printf("End\n\n");

    restore(mask);
}

process main(void)
{
	
    sync_printf("\nCascading Termination\n");

    pid1 = create(test1, 8192, 50, "test1", 1, CONSOLE);
    proctab[pid1].user_process = TRUE;
    resume(pid1);

    sync_printf("\nProcess List before killing\n");
    print_active_processes();

    sync_printf("\nKill process %d  with no child \n", 14);
    if (proctab[14].prstate != PR_FREE) {
        kill(14);
    } else {
        sync_printf("Process %d does not exist or is already terminated.\n", 14);
    }
    print_active_processes();

    sync_printf("\nKill process %d with few child\n", 8);
    if (proctab[8].prstate != PR_FREE) {
        kill(8);
    } else {
        sync_printf("Process %d does not exist or is already terminated.\n", 8);
    }
    print_active_processes();

    sync_printf("\nKill parent process %d \n", 5);
    if (proctab[5].prstate != PR_FREE) {
        kill(5);
    } else {
        sync_printf("Process %d does not exist or is already terminated.\n", 5);
    }
    print_active_processes();

    sync_printf("\n\n");

    return OK;
}
