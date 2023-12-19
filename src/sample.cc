#include "sample.h"
#include "c_sample.h"
#include "lua_plugin.h"
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif
 
 
void* sample_luban_new_pool_getter(const char** pool_files, int size){
    std::vector<std::string> files;
    for (int i = 0; i < size; i++) {
        files.push_back(std::string(pool_files[i]));
    }
    auto pool_getter = new sample_luban::PoolGetter(files);
    return (void*)pool_getter;
}
void *sample_luban_get_item_feature_json(void *pool_getter_ptr, char* item_id) {
    auto pool_getter = (sample_luban::PoolGetter*)pool_getter_ptr;
    auto item_feature = pool_getter->get(std::string(item_id));
    auto json_str= item_feature->stringnify();
    return strdup(json_str.c_str());
}
void sample_luban_update_pool(void * pool_getter_ptr,int index , char*pool_file) {
    auto pool_getter = (sample_luban::PoolGetter*)pool_getter_ptr;
    pool_getter->update_pool(index, pool_file);
}

void sample_luban_delete_pool_getter(void * pool_getter_ptr) {
    auto pool_getter = (sample_luban::PoolGetter*)pool_getter_ptr;
    if (pool_getter != nullptr) {
        delete pool_getter;
    }
}
 

void *sample_luban_new_user_rows(void *sample_luban_toolkit_ptr, void *pool_getter_ptr,  char *user_feature_json, int user_feature_len) {
    auto pool = (sample_luban::PoolGetter* )pool_getter_ptr;
    auto user_feature = std::make_shared<luban::Features>(std::string_view{user_feature_json, size_t(user_feature_len)});
    auto sample_luban_toolkit = (sample_luban::SampleLubanToolKit*)sample_luban_toolkit_ptr;
    auto rows = sample_luban_toolkit->process_user(pool,user_feature);
    luban::Rows* rows_ptr = rows.get();
    luban::Rows* clone_rows = new luban::Rows();
    clone_rows->m_rows = rows_ptr->m_rows;
    return (void*)clone_rows;
}

void sample_luban_delete_user_rows(void *rows_ptr) {
    auto feature = (luban::Rows*)rows_ptr;
    if (feature != nullptr) {
        delete feature;
    }
}

void *new_sample_luban_toolkit(char *plugin_file, char* luban_file) {
    auto toolkit = new sample_luban::SampleLubanToolKit( std::string(plugin_file),std::string(luban_file));
    return (void*) toolkit;
}
void delete_sample_luban_toolkit(void *sample_luban_toolkit_ptr) {
    auto toolkit = (sample_luban::SampleLubanToolKit*)sample_luban_toolkit_ptr;
    if (toolkit != nullptr) {
        delete toolkit;
    }
}

#ifdef __cplusplus
} /* end extern "C"*/
#endif

namespace sample_luban
{
std::string get_file_name(const std::string& filepath) {
    // 查找最后一个路径分隔符
    std::size_t lastSlashPos = filepath.find_last_of("/\\");
    
    // 查找最后一个点号
    std::size_t lastDotPos = filepath.find_last_of('.');
    
    // 如果点号在最后一个路径分隔符之前被找到了
    if (lastDotPos != std::string::npos && (lastSlashPos == std::string::npos || lastDotPos > lastSlashPos)) {
        return filepath.substr(lastSlashPos + 1, lastDotPos - lastSlashPos - 1);
    }
    
    // 如果没有点号，或者点号在最后一个路径分隔符之前
    if (lastSlashPos != std::string::npos) {
        // +1 为了跳过路径分隔符
        return filepath.substr(lastSlashPos + 1);
    }
    
    return filepath;
}

SampleToolkit::SampleToolkit(std::string process_plugin_file_path) {
    auto plugin_file = std::string(process_plugin_file_path);
    auto plugin_name = get_file_name(plugin_file);
    m_plugin_name = plugin_name;
    m_plugin = std::make_shared<LuaPluginBridge>(m_plugin_name, plugin_file);
 }
luban::SharedFeaturesPtr  SampleToolkit::process_sample(PoolGetter* pool, luban::SharedFeaturesPtr user_feature, std::string_view item_id) {
    
    auto processed_user =  m_plugin->process_user(user_feature, pool);
    auto item_feature = pool->get(std::string(item_id));
    if (item_feature == nullptr || processed_user == nullptr) {
            return nullptr;
    }
    auto processed_item = m_plugin->process_item(item_feature);
    processed_item->merge(processed_user);
    return processed_item;
}

luban::SharedFeaturesPtr SampleToolkit::process_user_feature(PoolGetter* pool, luban::SharedFeaturesPtr user_feature) {
     return m_plugin->process_user(user_feature, pool);
}
luban::SharedFeaturesPtr SampleToolkit::process_item_featrue(luban::SharedFeaturesPtr item_feature) {  
     return m_plugin->process_item(item_feature);
}



SampleLubanToolKit::SampleLubanToolKit( 
    const std::string& process_plugin_file_path,
    const std::string& luban_config_file) {
    m_luban_kit = std::make_shared<luban::Toolkit>(luban_config_file);
    m_sample_tool_kit = std::make_shared<SampleToolkit>(process_plugin_file_path);

}
std::shared_ptr<luban::Rows> SampleLubanToolKit::process_sample(PoolGetter* pool_getter, luban::SharedFeaturesPtr user_feature, const std::string& item_id) {
    auto sample = m_sample_tool_kit->process_sample(pool_getter ,user_feature, std::string(item_id));
    if (sample== nullptr) {
            return nullptr;
    }
    auto processed_sample =  m_luban_kit->process(sample);
    return processed_sample;
}

std::shared_ptr<luban::Rows> SampleLubanToolKit::process_user(PoolGetter* pool_getter, luban::SharedFeaturesPtr user_feature) {
    auto feature = m_sample_tool_kit->process_user_feature(pool_getter, user_feature);
    return m_luban_kit->process_user(feature);
}

} // sample_luban
