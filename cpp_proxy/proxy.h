#pragma once

#ifdef __cplusplus
extern "C" {
#endif

    typedef void (*notify_result_callback)(int result, char *res_description);
    int initialize_service(notify_result_callback result_callback);

    void start_calculation(int task_value);

    void deinitialize_service();

#ifdef __cplusplus
}; // end of extern "C"
#endif
