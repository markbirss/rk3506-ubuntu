/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-06-06     Cliff Chen   first implementation
 */

function GetTaskStackGrowDir()
{
    return -1;
}

/*********************************************************************
*
*       GetTaskPrioName
*
* Function description
*   Returns the display text of a task priority
*
* Parameters
*   Priority: task priority (integer)
*/
function GetTaskPrioName(Priority)
{

    var sName;

    if (Priority == 31)
    {
        sName = "Idle"
    }
    else if (Priority >= 30)
    {
        sName = "Low"
    }
    else if (Priority >= 20)
    {
        sName = "Normal";
    }
    else if (Priority >= 10)
    {
        sName = "High";
    }
    else if (Priority >= 0)
    {
        sName = "Highest";
    }
    else
    {
        return Priority.toString();
    }
    sName = sName + " (" + Priority + ")";
    return sName;
}

/*********************************************************************
*
*       GetTaskName
*
* Function description
*   Returns the display text of a task name
*
* Parameters
*   tcb:   task control block (type tskTCB)
*   Addr:  control block memory location
*/
function GetTaskName(tcb, Addr)
{

    var sTaskName;

    sTaskName = Debug.evaluate("(char*)(*(rt_thread_t)" + Addr + ").name");

    return sTaskName;
}

/*********************************************************************
*
*       GetTCB
*
* Function description
*   Returns the task control block of a task
*
* Parameters
*   Addr: control block memory location
*/
function GetTCB(Addr)
{
    var tcb;
    tcb = Debug.evaluate("*((rt_thread_t)" + Addr + ")");

    return tcb;
}

/*********************************************************************
*
*       GetTaskStackInfoStr
*
* Function description
*   Returns a display text of the format "<free space> / <stack size>"
*
* Parameters
*   tcb: task control block (type tskTCB)
*
* Notes
*   (1) pxEndOfStack is only available when RT-Thread was compiled with
*       configRECORD_STACK_HIGH_ADDRESS == 1
*/
function GetTaskStackInfoStr(tcb)
{
    var FreeSpace;
    var UsedSpace;
    var Size;
    //                                          GrowDir == -1     GrowDir == 1
    //
    // stack_addr               |  Low Addr   |  Stack Top     |  Stack Base    |
    //                          |             |                |                |
    // sp                       |             |  Stack Pointer |  Stack Pointer |
    //                          |             |                |                |
    // stack_addr + stack_size  |  High Addr  |  Stack Base    |  Stack Top     |
    //
    if (GetTaskStackGrowDir() == 1)   // stack grows in positive address direction
    {

        FreeSpace  =  tcb.stack_addr + tcb.stack_size - tcb.sp;
        UsedSpace  =  tcb.sp - tcb.stack_addr;
        Size       =  tcb.stack_size;
    }
    else        // stack grows in negative address direction
    {

        FreeSpace  =  tcb.sp - tcb.stack_addr;
        UsedSpace  =  tcb.stack_addr + tcb.stack_size - tcb.sp;
        Size       =  tcb.stack_size;
    }
    return FreeSpace + " / " + (Size == undefined ? "N/A" : Size);
}

/*********************************************************************
*
*       GetCurrTick
*
* Function description
*   Returns current tick of OS.
*
*/
function GetCurrTick()
{
    return Debug.evaluate("rt_tick");
}

/*********************************************************************
*
*       GetTaskTimeout
*
* Function description
*   Returns timeout stat of the task
*
* Parameters
*   tcb: task control block (type struct rt_thread)
*
*/
function GetTaskTimeout(tcb)
{
    var TaskTimeout;
    var CurrTick;
    var TimeoutState;

    CurrTick = GetCurrTick();
    TaskTimeout = tcb.thread_timer.timeout_tick;
    if (TaskTimeout >= CurrTick)
    {
        TimeoutState = "True";
    }
    else
    {
        TimeoutState = "False";
    }

    return TimeoutState;
}

/*********************************************************************
*
*       GetTaskState
*
* Function description
*   Returns stat of the task
*
* Parameters
*   tcb: task control block (type struct rt_thread)
*
*   RT_THREAD_INIT                  0x00                Initialized status
*   RT_THREAD_READY                 0x01                Ready status
*   RT_THREAD_SUSPEND               0x02                Suspend status
*   RT_THREAD_RUNNING               0x03                Running status
*   RT_THREAD_CLOSE                 0x04                Closed status
*/
function GetTaskState(tcb)
{
    var TaskState;
    var state;

    state = tcb.stat;
    switch (state)
    {
    case 0x00:
        TaskState = "Initialized";
        break;
    case 0x01:
        TaskState = "Ready";
        break;
    case 0x02:
        TaskState = "Suspend";
        break;
    case 0x03:
        TaskState = "Running";
        break;
    case 0x04:
        TaskState = "Closed";
        break;
    default:
        TaskState = "Unknown";
    }

    return TaskState;
}

/*********************************************************************
*
*       AddTask
*
* Function description
*   Adds a task to the task window
*
* Parameters
*   Addr:            memory location of the task's control block (TCB)
*/
function AddTask(Addr)
{
    var tcb;
    var sTaskName;
    var sPriority;
    var sRunCnt;
    var sRemainCnt;
    var sTimeout;
    var sStackInfo;
    var sState;

    tcb            = GetTCB(Addr);
    sTaskName      = GetTaskName(tcb, Addr);
    sStackInfo     = GetTaskStackInfoStr(tcb);
    sPriority      = GetTaskPrioName(tcb.current_priority);
    sTimeout       = GetTaskTimeout(tcb);
    sRunCnt        = tcb.init_tick - tcb.remaining_tick;
    sRemainCnt     = tcb.remaining_tick;
    sState         = GetTaskState(tcb);

    Threads.add2("Tasks", sTaskName, sRunCnt, sRemainCnt, sPriority, sState, sTimeout, sStackInfo, Addr);
}

/*********************************************************************
*
*       AddMutex
*
* Function description
*   Adds a mutex to the mutex window
*
* Parameters
*   Addr:            memory location of the mutex (rt_mutex_t)
*/
function AddMutex(Addr)
{
    var mutex;
    var sMutexName;
    var sOwner;
    var sValue;
    var sHold;

    mutex = Debug.evaluate("*(rt_mutex_t)" + Addr);
    sMutexName = Debug.evaluate("(char *)(*(rt_mutex_t)" + Addr + ").parent.parent.name");
    sOwner = mutex.owner;
    sOwner = Debug.evaluate("(char *)(*(rt_thread_t)" + sOwner + ").name");
    sValue = mutex.value;
    sHold = mutex.hold;

    Threads.add2("Mutexs", sMutexName, sOwner, sValue, sHold, Addr);
}

/*********************************************************************
*
*       AddList
*
* Function description
*   Adds all tasks of the task list to the task window
*
* Parameters
*   pList:            the list head (type rt_list_t)
*   MaxNums:          max number of tasks in the list
*   Type:             the list type (like: task, mutex ...)
*/
function AddList(pList, MaxNums, Type)
{
    var pHead;
    var curr;
    var count = 0;
    var List;

    List = Debug.evaluate("*(rt_list_t *)" + pList);
    curr = List.next;
    while (curr != pList)
    {
        pHead = curr - 12;
        if (Type == "Task")
        {
            AddTask(pHead);
        }
        else if (Type == "Mutex")
        {
            AddMutex(pHead);
        }
        curr = Debug.evaluate("*(rt_list_t *)" + curr);
        curr = curr.next;
        count++;
        if (count >= MaxNums)
        {
            break;
        }
    }
}

/*********************************************************************
*
*       API Functions
*
**********************************************************************
*/

/*********************************************************************
*
*       init
*
* Function description
*   Initializes the task window
*/
function init()
{

    Threads.clear();
    Threads.newqueue("Tasks");
    Threads.setColumns2("Tasks", "Name", "Run Ticks", "Remain Ticks", "Priority", "Status", "Timeout", "Stack Info", "ID");
    Threads.setSortByNumber("Priority");
    Threads.setSortByNumber("Timeout");
    Threads.setSortByNumber("Run Ticks");
    Threads.setColor("Status", "Ready", "Running", "Suspend");

    Threads.newqueue("Mutexs");
    Threads.setColumns2("Mutexs", "Name", "Owner", "Value", "Hold", "ID");
    Threads.setSortByNumber("Name");
}

/*********************************************************************
*
*       update
*
* Function description
*   Updates the task window
*
* RT_Object_Class_Thread = 1,                             The object is a thread
*
* RT_Object_Class_Semaphore = 2,                          The object is a semaphore
*
* RT_Object_Class_Mutex = 3,                              The object is a mutex
*
*/
function update()
{
    var pList;
    var Info;

    // clear all tables
    Threads.clear();

    // fill the task table
    if (Threads.shown("Tasks"))
    {
        Threads.newqueue("Tasks");
        Threads.setColumns2("Tasks", "Name", "Run Ticks", "Remain Ticks", "Priority", "Status", "Timeout", "Stack Info", "ID");

        Info = Debug.evaluate("_object_container[0]");
        //if (Info.type == RT_Object_Class_Thread) {
        pList = Debug.evaluate("&(_object_container[0].object_list)");
        AddList(pList, Info.object_size, "Task");
        //}
    }

    // fill the mutex table
    if (Threads.shown("Mutexs"))
    {
        Threads.newqueue("Mutexs");
        Threads.setColumns2("Mutexs", "Name", "Owner", "Value", "Hold", "ID");

        Info = Debug.evaluate("_object_container[2]");
        //if (Info.type == RT_Object_Class_Mutex) {
        pList = Debug.evaluate("&(_object_container[2].object_list)");
        AddList(pList, Info.object_size, "Mutex");
        //}
    }
}

/*********************************************************************
*
*       getregs
*
* Function description
*   Returns the register set of a task.
*   For ARM cores, this function is expected to return the values
*   of registers R0 to R15 and PSR.
*
* Parameters
*   hTask: integer number identifiying the task.
*   Identical to the last parameter supplied to method Threads.add.
*   For convenience, this should be the address of the TCB.
*
* Return Values
*   An array of unsigned integers containing the task’s register values.
*   The array must be sorted according to the logical indexes of the regs.
*   The logical register indexing scheme is defined by the ELF-DWARF ABI.
*
**********************************************************************
*/
function getregs(hTask)
{
    var i;
    var SP;
    var LR;
    var Addr;
    var tcb;
    var aRegs = new Array(17);

    tcb  =  GetTCB(hTask);
    SP   =  tcb.sp;
    Addr =  SP;

    /* the following registers are pushed by the RT-Thread-scheduler */
    //
    // R4...R11
    //
    for (i = 4; i < 12; i++)
    {
        aRegs[i] = TargetInterface.peekWord(Addr);
        Addr += 4;
    }
    //
    // EXEC_RET
    //
    LR = TargetInterface.peekWord(Addr);
    Addr += 4;
    //
    // S16...S31
    //
    if ((LR & 0x10) != 0x10)   // FP context has been saved?
    {
        Addr += 4 * 16; // skip S16..S31
    }
    /* the following registers are pushed by the ARM core */
    //
    // R0...R3
    //
    for (i = 0; i < 4; i++)
    {
        aRegs[i] = TargetInterface.peekWord(Addr);
        Addr += 4;
    }
    //
    // R12, LR, PC, PSR
    //
    aRegs[12] = TargetInterface.peekWord(Addr);
    Addr += 4;
    aRegs[14] = TargetInterface.peekWord(Addr);
    Addr += 4;
    aRegs[15] = TargetInterface.peekWord(Addr);
    Addr += 4;
    aRegs[16] = TargetInterface.peekWord(Addr);
    Addr += 4;
    //
    // S0..S15
    //
    if ((LR & 0x10) != 0x10)   // FP context has been saved?
    {
        Addr += 4 * 18; // skip S0...S15
    }
    if (aRegs[16] & (1 << 9)) // Stack has been 8-byte aligned
    {
        Addr += 4;
    }
    //
    // SP
    //
    aRegs[13] = Addr;

    return aRegs;
}

/*********************************************************************
*
*       getContextSwitchAddrs
*
*  Functions description
*    Returns an unsigned integer array containing the base addresses
*    of all functions that complete a task switch when executed.
*/
function getContextSwitchAddrs()
{

    var aAddrs;
    var Addr;

    Addr = Debug.evaluate("&rt_schedule");

    if (Addr != undefined)
    {
        aAddrs = new Array(1);
        aAddrs[0] = Addr;
        return aAddrs;
    }
    return [];
}

/*********************************************************************
*
*       getOSName()
*
*  Functions description:
*    Returns the name of the RTOS this script supplies support for
*/
function getOSName()
{
    return "RT-Thread";
}