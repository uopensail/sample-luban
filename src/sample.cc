#include "sample.h"
#include "c_sample.h"
#include "lua_plugin.h"

#include <vector>
#include <fstream>
#include <thread>
#include <json.hpp>
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
void* sample_luban_rows_to_json(void* ptr) {
    auto rows = (luban::Rows*)ptr;
    auto j = rows->to_json();
    auto js = j->dump();
    return strdup(js.c_str());
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

std::string join_paths(const std::string& path1, const std::string& path2) {
    // 检查是否需要添加路径分隔符
    if (!path1.empty() && path1.back() != '/' && path1.back() != '\\') {
        return path1 + "/" + path2;
    } else {
        return path1 + path2;
    }
}

SamplePreProcessor::SamplePreProcessor(std::string process_plugin_file_path) {
    auto plugin_file = std::string(process_plugin_file_path);
    auto plugin_name = get_file_name(plugin_file);
    m_plugin_name = plugin_name;
    m_plugin = std::make_shared<LuaPluginBridge>(m_plugin_name, plugin_file);
}
std::tuple<std::string, luban::SharedFeaturesPtr>  SamplePreProcessor::process_sample(PoolGetter* pool, luban::SharedFeaturesPtr user_feature, std::string_view item_id,const std::string& label) {
    
    auto item_feature = pool->get(std::string(item_id));
    return this->process_sample(pool, user_feature,item_feature, label);
}

std::tuple<std::string, luban::SharedFeaturesPtr>  SamplePreProcessor::process_sample(PoolGetter* pool, luban::SharedFeaturesPtr user_feature, luban::SharedFeaturesPtr item_feature,const std::string& label) {
    
    auto processed_user =  m_plugin->process_user(user_feature, pool);
    if (item_feature == nullptr || processed_user == nullptr) {
            return std::make_tuple(label,nullptr);;
    }
    auto processed_item = m_plugin->process_item(item_feature);
    processed_item->merge(processed_user);
    auto process_label = m_plugin->process_label(processed_item, label);
    return std::make_tuple(process_label, processed_item);
}

luban::SharedFeaturesPtr SamplePreProcessor::process_user_feature(PoolGetter* pool, luban::SharedFeaturesPtr user_feature) {
     return m_plugin->process_user(user_feature, pool);
}
luban::SharedFeaturesPtr SamplePreProcessor::process_item_featrue(luban::SharedFeaturesPtr item_feature) {  
     return m_plugin->process_item(item_feature);
}



SampleLubanToolKit::SampleLubanToolKit( 
    const std::string& process_plugin_file_path,
    const std::string& luban_config_file) {
    m_luban_kit = std::make_shared<luban::Toolkit>(luban_config_file);
    m_sample_tool_kit = std::make_shared<SamplePreProcessor>(process_plugin_file_path);

}
std::tuple<std::string, std::shared_ptr<luban::Rows> > SampleLubanToolKit::process_sample(PoolGetter* pool_getter, luban::SharedFeaturesPtr user_feature, const std::string& item_id, const std::string& label) {
    auto label_sample = m_sample_tool_kit->process_sample(pool_getter ,user_feature, item_id,label);
    std::string process_label = std::get<0>(label_sample);
    luban::SharedFeaturesPtr sample =  std::get<1>(label_sample);
    if (sample== nullptr) {
            return std::make_tuple(process_label,nullptr);
    }
    auto processed_sample_rows =  m_luban_kit->process(sample);
    return std::make_tuple(process_label, processed_sample_rows);
}
 
std::tuple<std::string, std::shared_ptr<luban::Rows> > SampleLubanToolKit::process_sample(PoolGetter* pool_getter, luban::SharedFeaturesPtr user_feature, luban::SharedFeaturesPtr item_feature, const std::string& label) {
    auto label_sample = m_sample_tool_kit->process_sample(pool_getter ,user_feature, item_feature,label);
    std::string process_label = std::get<0>(label_sample);
    luban::SharedFeaturesPtr sample =  std::get<1>(label_sample);
    if (sample== nullptr) {
            return std::make_tuple(process_label,nullptr);
    }
    auto processed_sample_rows =  m_luban_kit->process(sample);
    return std::make_tuple(process_label, processed_sample_rows);
}

std::tuple<std::string,luban::SharedFeaturesPtr> SampleLubanToolKit::sample_feature(PoolGetter* pool_getter, luban::SharedFeaturesPtr user_feature, const std::string& item_id, const std::string& label) {
    auto label_sample = m_sample_tool_kit->process_sample(pool_getter ,user_feature, std::string(item_id),label);
    std::string process_label = std::get<0>(label_sample);
    luban::SharedFeaturesPtr sample =  std::get<1>(label_sample);
    if (sample== nullptr) {
            return std::make_tuple(process_label,nullptr);
    }
    return std::make_tuple(process_label,sample);
}
std::shared_ptr<luban::Rows> SampleLubanToolKit::process_user(PoolGetter* pool_getter, luban::SharedFeaturesPtr user_feature) {
    auto feature = m_sample_tool_kit->process_user_feature(pool_getter, user_feature);
    return m_luban_kit->process_user(feature);
}


void process_sample_one_file(std::shared_ptr<PoolGetter> pool, std::shared_ptr<SampleLubanToolKit> toolkit,
     std::string input_file, std::string output_file) {
    std::ifstream reader(input_file, std::ios::in);
    if (!reader) {
        std::cerr << "read input_file file: " << input_file << " error" << std::endl;
        exit(-1);
    }
    std::string line;
    std::ofstream writer(output_file); // 打开文件用于写入，如果文件不存在则创建

    while (std::getline(reader, line)) {
        auto ss = split(line,'\t');
        if (ss.size() != 3) {
            continue;
        }
        auto user_feature_json = ss[0];
        auto item_id = ss[1];
        auto label = ss[2];
        auto user_features = std::make_shared<luban::Features>(user_feature_json);
        if (user_features == nullptr) {
            continue;
        }
        auto label_rows = toolkit->process_sample(pool.get(), user_features,item_id, label);
        std::string process_label = std::get<0>(label_rows);
        std::shared_ptr<luban::Rows> rows =  std::get<1>(label_rows);
        if (rows == nullptr) {
            continue;
        }
        auto js = rows->to_json();
        writer << process_label << "\t" <<  js->dump() <<std::endl;
    }
    reader.close();      
    writer.close();
}
void process_sample_files_work(std::shared_ptr<PoolGetter> pool,const std::string& process_plugin_file_path,
    const std::string& luban_config_file, std::vector<std::string> files, std::string output_dir) {
    auto toolkit = std::make_shared<SampleLubanToolKit>(process_plugin_file_path, luban_config_file);
    for (int i=0;i<files.size();i++) {
        auto file = files[i];
        auto file_name = get_file_name(file);
        auto output_file = join_paths(output_dir, file_name);
        process_sample_one_file(pool, toolkit, file, output_file);
    }
}

SampleToolKit::SampleToolKit(const std::vector<std::string> & pool_files,
    const std::string& process_plugin_file_path,
    const std::string& luban_config_file):m_process_plugin_file_path(process_plugin_file_path), m_luban_config_file(luban_config_file){
    m_pool = std::make_shared<PoolGetter>(pool_files);
}

void SampleToolKit::process_sample_files(const std::vector<std::string>& files, int thread_num, std::string output_dir) {
    if (files.size() <=0) {
        return;
    }
    if (thread_num <=0) {
        thread_num =1;
    }
    int step = files.size() / thread_num;
    if (step <=0) {
        step = 1;
    }
    std::vector<std::thread> workers;
     
    int i=0;
    for (;i<files.size();i+= step) {
        std::vector<std::string> work_files;
        for (int j=i;j<i+step && j<files.size();j++) {
            work_files.push_back(files[j]);
        }
  
        workers.push_back(std::thread(process_sample_files_work, m_pool, 
            m_process_plugin_file_path, m_luban_config_file,work_files, output_dir));
    }
    if (i < files.size()) {
        std::vector<std::string> work_files;
        for (int j=i;  j<files.size();j++) {
            work_files.push_back(files[j]);
        }
        workers.push_back(std::thread(process_sample_files_work, m_pool, 
            m_process_plugin_file_path, m_luban_config_file,work_files, output_dir));
    }
    for(std::thread &t : workers) {
        t.join();
    }
}
} // sample_luban
