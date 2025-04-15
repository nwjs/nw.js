#ifdef BLINK_HOOK_MAP
#define V(type, sym, fn) BLINK_HOOK_MAP(type, sym, fn)
#else
#define V(type, sym, fn)
#endif
V(VoidPtr4Fn, "g_web_worker_start_thread", g_web_worker_start_thread_fn)
#undef V

#ifdef PLATFORM_HOOK_MAP
#define V(type, sym, fn) PLATFORM_HOOK_MAP(type, sym, fn)
#else
#define V(type, sym, fn)
#endif
V(VoidPtr2Fn, "g_web_worker_thread_new", g_web_worker_thread_new_fn)
#undef V

#ifdef NW_HOOK_INIT
#define V(type, sym, fn) NW_HOOK_INIT(type, sym, fn)
#else
#define V(type, sym, fn) NW_HOOK_MAP(type, sym, fn)
#endif
V(CallTickCallbackFn, "g_call_tick_callback", g_call_tick_callback_fn)
V(SetupNWNodeFn, "g_setup_nwnode", g_setup_nwnode_fn)
V(IsNodeInitializedFn, "g_is_node_initialized", g_is_node_initialized_fn)
V(SetNWTickCallbackFn, "g_set_nw_tick_callback", g_set_nw_tick_callback_fn)
V(StartNWInstanceFn, "g_start_nw_instance", g_start_nw_instance_fn)
V(GetNodeContextFn, "g_get_node_context", g_get_node_context_fn)
V(SetNodeContextFn, "g_set_node_context", g_set_node_context_fn)
V(GetNodeEnvFn, "g_get_node_env", g_get_node_env_fn)
V(GetCurrentEnvironmentFn, "g_get_current_env", g_get_current_env_fn)
V(EmitExitFn, "g_emit_exit", g_emit_exit_fn)
V(RunAtExitFn, "g_run_at_exit", g_run_at_exit_fn)
V(HostImportModuleFn, "g_host_import_module", g_host_import_module_fn)
V(HostGetImportMetaFn, "g_host_get_import_meta", g_host_get_import_meta_fn)
#undef V

#ifdef NODE_HOOK_INIT
#define V(type, sym, fn) NODE_HOOK_INIT(type, sym, fn)
#else
#define V(type, sym, fn) NW_HOOK_MAP(type, sym, fn)
#endif
V(VoidVoidFn, "g_stop_nw_instance", g_stop_nw_instance_fn)
V(VoidIntHookFn, "g_msg_pump_ctor", g_msg_pump_ctor_fn)
V(VoidHookFn, "g_msg_pump_dtor", g_msg_pump_dtor_fn)
V(VoidHookFn, "g_msg_pump_sched_work", g_msg_pump_sched_work_fn)
V(VoidHookFn, "g_msg_pump_nest_leave", g_msg_pump_nest_leave_fn)
V(VoidHookFn, "g_msg_pump_need_work", g_msg_pump_need_work_fn)
V(VoidHookFn, "g_msg_pump_did_work", g_msg_pump_did_work_fn)
V(VoidHookFn, "g_msg_pump_pre_loop", g_msg_pump_pre_loop_fn)
V(VoidHookFn, "g_msg_pump_nest_enter", g_msg_pump_nest_enter_fn)
V(VoidIntHookFn, "g_msg_pump_delay_work", g_msg_pump_delay_work_fn)
V(VoidHookFn, "g_msg_pump_clean_ctx", g_msg_pump_clean_ctx_fn)
V(GetPointerFn, "g_uv_default_loop", g_uv_default_loop_fn)
V(NodeStartFn, "g_node_start", g_node_start_fn)
V(UVRunFn, "g_uv_run", g_uv_run_fn)
V(SetUVRunFn, "g_set_uv_run", g_set_uv_run_fn)
V(VoidIntFn, "g_uv_init_nw", g_uv_init_nw_fn)
V(VoidHookFn, "g_promise_reject_callback", g_promise_reject_callback_fn)

#if defined(OS_MAC)
V(VoidHookFn, "g_msg_pump_dtor_osx", g_msg_pump_dtor_osx_fn)
V(VoidHookFn, "g_uv_sem_post", g_uv_sem_post_fn)
V(VoidHookFn, "g_uv_sem_wait", g_uv_sem_wait_fn)
V(VoidPtr4IntFn, "g_msg_pump_ctor_osx", g_msg_pump_ctor_osx_fn)
V(IntVoidFn, "g_nw_uvrun_nowait", g_nw_uvrun_nowait_fn)
V(IntVoidFn, "g_uv_runloop_once", g_uv_runloop_once_fn)
V(IntVoidFn, "g_uv_backend_timeout", g_uv_backend_timeout_fn)
V(IntVoidFn, "g_uv_backend_fd", g_uv_backend_fd_fn)
V(BoolVoidFn, "g_nw_enter_dom", g_nw_enter_dom_fn)
V(VoidBoolFn, "g_nw_leave_dom", g_nw_leave_dom_fn)
#endif

#undef V
