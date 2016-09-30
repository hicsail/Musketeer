    rt.schedule_callback(DeferredList(all_done), shutdown_wrapper, rt)
    ext.finish()
