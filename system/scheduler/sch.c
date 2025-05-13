
#include"sch.h"

/** Case A:new task happens before current
 *                            --------------------------------
 *                            |            Current           |
 *                            --------------------------------
 * —————————————————————————————————————————————————————————————————————→clock
 * ---------------------
 * |        NEW        |
 * ---------------------
 */

/** Case B:new task starts before and finishes during current
 *                            --------------------------------
 *                            |            Current           |
 *                            --------------------------------
 * —————————————————————————————————————————————————————————————————————→clock
 *               ---------------------
 *               |        NEW        |
 *               ---------------------
 */

/** Case C:new task starts before and finishes after current
 *                            --------------------------------
 *                            |            Current           |
 *                            --------------------------------
 * —————————————————————————————————————————————————————————————————————→clock
 *               ---------------------------------------------------
 *               |                      NEW                        |
 *               ---------------------------------------------------
 */

/** Case D:new task starts after and finishes before current
 *                            --------------------------------
 *                            |            Current           |
 *                            --------------------------------
 * —————————————————————————————————————————————————————————————————————→clock
 *                                 -----------------
 *                                 |      NEW      |
 *                                 -----------------
 */

 /** Case E:new task starts during and finishes after current
 *                            --------------------------------
 *                            |            Current           |
 *                            --------------------------------
 * —————————————————————————————————————————————————————————————————————→clock
 *                                     -------------------------------
 *                                     |             NEW              |
 *                                     -------------------------------
 */

  /** Case F:new task starts and finishes after current
 *                            --------------------------------
 *                            |            Current           |
 *                            --------------------------------
 * —————————————————————————————————————————————————————————————————————————————————————————→clock
 *                                                                   ---------------------
 *                                                                   |         NEW       |
 *                                                                   ---------------------
 */
 enum
 {
    SCH_START_BEFORE_END_BEFORE = 0x01, //Case A
    SCH_START_BEFORE_END_DURING = 0x02, //Case B
    SCH_START_BEFORE_END_AFTER  = 0x03,  //Case C
    SCH_START_DURING_END_DURING = 0x03, //Case D
    SCH_START_DURING_END_AFTER  = 0x04,  //Case E
    SCH_START_AFTER_END_AFTER   = 0x05   //Case F
 };

 static int sch_task1_conflict_with_task2(sch_node_t* task1,sch_node_t* task2)
 {
    ASSERT(task1==NULL||task2==NULL);
    _u32 task1S = task1->timestamp - task1->startLatency;
    _u32 task1E = task1->timestamp + task1->duration + task1->stopLatency;
    _u32 task2S = task2->timestamp - task2->startLatency;
    _u32 task2E = task2->timestamp + task2->duration + task2->stopLatency;

    if (tick1_exceed_tick2(task2S,task1S))
    {
        if(tick1_exceed_tick2(task2S,task1E))
        {
            return SCH_START_BEFORE_END_BEFORE; //case A
        }
        else 
        {
            if(tick1_exceed_tick2(task2E,task1E))
            {
                return SCH_START_BEFORE_END_DURING;//case B
            }
            else
            {
                return SCH_START_BEFORE_END_AFTER;
            }
        }
    }
    else
    {
        if(tick1_exceed_tick2(task2E,task1E))
        {
            return SCH_START_DURING_END_DURING;
        }
        else 
        {
            if(tick1_exceed_tick2(task2S,task1E))
            {
                return SCH_START_DURING_END_AFTER;
            }
            else
            {
                return SCH_START_AFTER_END_AFTER;
            }
        }
    }
 }

 