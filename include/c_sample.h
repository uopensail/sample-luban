#ifndef SAMPLE_LUBAN_C_API_H
#define SAMPLE_LUBAN_C_API_H

#ifdef __cplusplus
extern "C"
{
#endif

    void *sample_luban_new_pool_getter(const char **pool_files, int size);
    void sample_luban_update_pool(void *pool_getter_ptr, int index, char *pool_file);
    void sample_luban_delete_pool_getter(void *pool_getter_ptr);

    void *sample_luban_new_toolkit(char *plugin_file);
    void sample_luban_delete_toolkit(void *toolkit_ptr);

    void *sample_luban_new_features(char *feature_json);
    void sample_luban_delete_features(void *feature_ptr);

    char *sample_luban_process_item(void *toolkit_ptr, void *pool_getter_ptr, char *item_id, int item_id_len);
    char *sample_luban_process_user(void *toolkit_ptr, void *pool_getter_ptr, char *user_feature_json, int user_feature_len);
    char *sample_luban_process_sample(void *toolkit_ptr, void *pool_getter_ptr, char *user_feature_json, int user_feature_len, char *item_id, int item_id_len);

#ifdef __cplusplus
} /* end extern "C"*/
#endif

#endif // SAMPLE_LUBAN_C_API_H