//
// Created by chris on 9/29/16.
//

#include "PlayerInner.h"
#include "util/XLog.h"

#define TAG "PLAYER_INNER"


PlayerInner::PlayerInner()
{
    // TODO

}

#if 0
// 线程，本身的一个大的引擎线程
EL_STATIC void *el_demux_file_thread(void *arg)
{
	el_player_engine_evt_e evt;
	int64_t                tm;

	EL_DBG_FUNC_ENTER;

	// read the packets from media file and buffer them to a queue
	while(1)
	{
        el_ipc_wait(el_msg_q_demux_file, &evt, &tm);

        el_player_engine_state_machine_t *state_machine = &g_media_file_obj.demux_file_state_machine;
        int64_t FUNC_START_BeginTime = el_get_sys_time_ms();

        printf("[D1] DemuxFileThread(0x%x), EVT = [%s] arrived, Current State = [%s]\n",
               (unsigned int)pthread_self(),
               g_event_names[evt],
               g_state_names[state_machine->state]);

        //        FUNC_DEBUG_START()

        // 结束线程
        if(evt == EVT_EXIT_THREAD)
        {
            break;
        }

        // 如果是SEEK方法，则先发一个SEEK_PAUSE
        if(evt == EVT_SEEK)
        {
            el_demux_file_state_machine_process_event(EVT_SEEK_PAUSE);
        }

        // 处理常规流程
        el_demux_file_state_machine_process_event(evt);

        printf("[D2] DemuxFileThread(0x%x), EVT [%s] processed in %lld ms, OldState = [%s], NewState = [%s]\n\n",
               (unsigned int)pthread_self(),
               g_event_names[evt],
               el_get_sys_time_ms() - FUNC_START_BeginTime,
               g_state_names[state_machine->old_state],
               g_state_names[state_machine->state]);
//        FUNC_DEBUG_END(&g_media_file_obj.demux_file_state_machine, msg)
	}

	EL_DBG_FUNC_EXIT;
	return ((void *)0);
}

#endif

/**
 * player central engine init.
 */
void PlayerInner::player_engine_init()
{
    // Register all formats and codecs
    av_register_all();
    avformat_network_init();

    //
}






