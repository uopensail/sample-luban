

#include "lua_plugin.h"
#include "feature.h"
#include "pool.h"

#include <sol/sol.hpp>
#include <variant>
#include <memory>
#include <vector>


namespace sample_luban{

luban::Features* pool_get( Pool& pool, const std::string& key) {
    auto ret_ptr =  pool.get(key);
    if (ret_ptr != nullptr) {
        return ret_ptr.get();
    }
    return nullptr;
}

luban::Parameter* features_get( luban::Features& features, const std::string& key) {
    auto ret_ptr =  features.operator[](key);
    if (ret_ptr != nullptr) {
        return ret_ptr.get();
    }
    return nullptr;
}
template <typename T>
void features_template_set(luban::Features& features, const std::string& key, const T&value) {
        luban::SharedParameter p = std::make_shared<luban::Parameter>();
        *p = value;
        features.set(key, p);
}
void features_set_fvalue(luban::Features& features, const std::string& key, luban::SharedParameter fv) {
    features.set(key, fv);
}


// Extract the value from the variant and return a sol::object
sol::object get_fvalue(lua_State* L, luban::Parameter*fv) {

    if (std::holds_alternative<int64_t>(*fv)) {
        return sol::make_object(L, std::get<int64_t>(*fv));
    } else if (std::holds_alternative<float>(*fv)) {
        return sol::make_object(L, std::get<float>(*fv));
    } else if (std::holds_alternative<std::string>(*fv)) {
        return sol::make_object(L, std::get<std::string>(*fv));
    }else if (std::holds_alternative<std::vector<int64_t>>(*fv)) {
        return sol::make_object(L,  &std::get<std::vector<int64_t>>(*fv));
    }else if (std::holds_alternative<std::vector<float>>(*fv)) {
        return sol::make_object(L, &std::get<std::vector<float>>(*fv));
    }else if (std::holds_alternative<std::vector<std::string>>(*fv)) {
        return sol::make_object(L, &std::get<std::vector<std::string>>(*fv));
    }else {
        return sol::make_object(L,nullptr);
    }
}

void bind_lua(sol::state& lua) {
     
    lua.set_function("IntValue",[](int64_t vv){return std::make_shared<luban::Parameter>(vv);});
    lua.set_function("FloatValue",[](float vv){return std::make_shared<luban::Parameter>(vv);});
    lua.set_function("StrValue",[](const std::string& vv){return vv;});
    lua.set_function("IntsValue",[](std::vector<int64_t> vv){return vv;});
    lua.set_function("FloatsValue",[](std::vector<float> vv){return vv;});
    lua.set_function("StrsValue",[](std::vector<std::string> vv){return vv;});
    lua.new_usertype<luban::Features>("Features",
        "new", sol::factories(
            []() { return std::make_shared<luban::Features>(); }
        ),
        "stringnify",&luban::Features::stringnify,
        sol::meta_function::index,[&]( luban::Features& features, const std::string& key){
            auto fv = features_get(features,key);
            if (fv == nullptr) {
                return sol::make_object(lua,nullptr);
            }
            return get_fvalue(lua,fv);
        },
        sol::meta_function::new_index, sol::overload(
            &features_template_set<std::string>,
            &features_template_set<std::vector<std::string>>,
            &features_template_set<std::vector<int64_t>>,
            &features_template_set<std::vector<float> >,
            &features_set_fvalue
        )
    );

    lua.new_usertype<Pool>("Pool",
        sol::meta_function::index, &Pool::get
    );

    lua.new_usertype<PoolGetter>("PoolGetter",
        sol::meta_function::index, &PoolGetter::get
    );
    
}
 
LuaPluginBridge::LuaPluginBridge(std::string name, std::string file_path) :m_name(name),m_file_path(file_path) {
   this->load(name, file_path);
}
 
void LuaPluginBridge::load(std::string name, std::string file_path)  {
    m_lua.open_libraries(sol::lib::base, sol::lib::string,sol::lib::os,sol::lib::math, sol::lib::io, sol::lib::table);
    bind_lua(m_lua);
    m_lua.safe_script_file(std::string(file_path));

}
LuaPluginBridge::~LuaPluginBridge()  {
    m_lua.set(this->m_name, sol::lua_nil);
}

std::shared_ptr<luban::Features> LuaPluginBridge::process_sample(std::shared_ptr<luban::Features> user_feature, PoolGetter* pool){

   sol::protected_function func = m_lua[this->m_name]["process_sample"];
    sol::protected_function_result result = func(user_feature, pool);
    if (!result.valid()) {
        sol::error err = result;
        
        std::cerr << "Error calling lua function: " << err.what() << std::endl;
        return user_feature;
    }else {
  
        auto ret = std::shared_ptr<luban::Features>(result);
        return ret;
    }
}

std::shared_ptr<luban::Features> LuaPluginBridge::process_user(std::shared_ptr<luban::Features>  user_feature, 
        PoolGetter* pool){
 
    sol::protected_function func = m_lua[this->m_name]["process_user"];
    sol::protected_function_result result = func(user_feature, pool);
    if (!result.valid()) {
        sol::error err = result;
        
        std::cerr << "Error calling lua function: " << err.what() << std::endl;
        return nullptr;
    }else {
  
        auto ret = std::shared_ptr<luban::Features>(result);
        return ret;
    }
}
std::shared_ptr<luban::Features> LuaPluginBridge::process_item(std::shared_ptr<luban::Features>  item_feature) {
 
    sol::protected_function func = m_lua[this->m_name]["process_item"];
    sol::protected_function_result result = func(item_feature);
    if (!result.valid()) {
        sol::error err = result;
        
        std::cerr << "Error calling lua function: " << err.what() << std::endl;
        return nullptr;
    }else {
  
        auto ret = std::shared_ptr<luban::Features>(result);
        return ret;
    }
}

std::string LuaPluginBridge::process_label(std::shared_ptr<luban::Features>  sample_features, const std::string& label) {
    sol::optional<sol::protected_function> func = m_lua[this->m_name]["process_label"];
    if (func) {
        sol::protected_function_result result = (*func)(sample_features, label);
        if (!result.valid()) {
            sol::error err = result;
            
            std::cerr << "Error calling lua function: " << err.what() << std::endl;
            return label;
        }else {
    
            std::string ret = result.get<std::string>();
            return ret;
        }
    }
    return label;
}

}