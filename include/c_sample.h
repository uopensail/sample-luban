#ifndef SAMPLE_LUBAN_C_API_H
#define SAMPLE_LUBAN_C_API_H

#ifdef __cplusplus
extern "C"
{
#endif

    void *sample_luban_new_pool_getter(const char **pool_files, int size);
    void sample_luban_update_pool(void *pool_getter_ptr, int index, char *pool_file);
    void sample_luban_delete_pool_getter(void *pool_getter_ptr);

    void *new_sample_luban_toolkit(char *plugin_file, char* luban_file);
    void delete_sample_luban_toolkit(void *sample_luban_toolkit_ptr);

    void *sample_luban_new_user_rows(void *sample_luban_toolkit_ptr, void *pool_getter_ptr,  char *user_feature_json, int user_feature_len);
    void sample_luban_delete_user_rows(void *rows_ptr);

 
#ifdef __cplusplus
} /* end extern "C"*/
#endif

#endif // SAMPLE_LUBAN_C_API_H