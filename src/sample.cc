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

void*  sample_luban_new_toolkit(char *plugin_file) {
    auto toolkit = new sample_luban::SampleToolkit(std::string(plugin_file));
    return (void*) toolkit;
}
void sample_luban_delete_toolkit(void*  toolkit_ptr) {
    auto toolkit = (sample_luban::SampleToolkit*)toolkit_ptr;
    if (toolkit != nullptr) {
        delete toolkit;
    }
}
char*  sample_luban_process_item(void*toolkit_ptr, void *pool_getter_ptr, char* item_id, int item_id_len) {
    auto pool = (sample_luban::PoolGetter* )pool_getter_ptr;
    if (pool == nullptr) {
        return nullptr;
    }
    auto item_feature = pool->get(std::string(item_id, item_id_len));
    if (item_feature == nullptr) {
        return nullptr;
    }
    auto toolkit = (sample_luban::SampleToolkit*)toolkit_ptr;
    auto processed_feat = toolkit->process_item_featrue(item_feature);
    std::string feature_json = processed_feat->stringnify();
    return strdup(feature_json.c_str());
}

char* sample_luban_process_user(void*toolkit_ptr, void *pool_ptr, char *user_feature_json, int user_feature_len) {
    auto pool = (sample_luban::PoolGetter* )pool_ptr;
    auto user_feature = std::make_shared<luban::Features>(std::string_view{user_feature_json, size_t(user_feature_len)});
    auto toolkit = (sample_luban::SampleToolkit*)toolkit_ptr;
    auto processed_feat = toolkit->process_user_feature(pool, user_feature);
    std::string feature_json = processed_feat->stringnify();
    return strdup(feature_json.c_str());
}

char*  sample_luban_process_sample(void*toolkit_ptr, void *pool_ptr, char *user_feature_json, int user_feature_len,char* item_id, int item_id_len){
    auto pool = (sample_luban::PoolGetter* )pool_ptr;
    auto user_feature = std::make_shared<luban::Features>(std::string_view{user_feature_json, size_t(user_feature_len)});
    auto toolkit = (sample_luban::SampleToolkit*)toolkit_ptr;
    auto out = toolkit->process_sample(pool,user_feature, std::string(item_id, item_id_len));
    if (out == nullptr) {
        return nullptr;
    }
    return strdup(out->stringnify().c_str());
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
    const std::string& pool_file,
    const std::string &luban_config_file, 
    const std::string& process_plugin_file_path) {
    std::vector<std::string> files;
    files.push_back(pool_file);
    m_pool_getter = std::make_shared<PoolGetter>(files);
    m_luban_kit = std::make_shared<luban::Toolkit>(luban_config_file);
    m_sample_tool_kit = std::make_shared<SampleToolkit>(process_plugin_file_path);

}
std::shared_ptr<luban::Rows> SampleLubanToolKit::process_sample(const std::string& user_feature_json, const std::string& item_id) {
    auto user_feature = std::make_shared<luban::Features>(user_feature_json);
    auto sample = m_sample_tool_kit->process_sample(m_pool_getter.get(),user_feature, std::string(item_id));
    auto processed_sample =  m_luban_kit->process(sample);
    return processed_sample;
}
} // sample_luban