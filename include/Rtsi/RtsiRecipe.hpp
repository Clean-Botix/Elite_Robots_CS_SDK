/**
 * @file RtsiRecipe.hpp
 * @author yanxiaojia
 * @brief Abot RTSI recipe
 * @date 2024-08-21
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef __RTSI_RECIPE_HPP__
#define __RTSI_RECIPE_HPP__

#include <Elite/DataType.hpp>
#include <Elite/EliteOptions.hpp>

#if (ELITE_SDK_COMPILE_STANDARD >= 17)
#include <variant>
#elif (ELITE_SDK_COMPILE_STANDARD == 14)
#include <boost/variant.hpp>
#endif
#include <vector>
#include <unordered_map>
#include <string>
#include <array>
#include <stdexcept>
#include <mutex>
#include <atomic>
#include <memory>

namespace ELITE
{

/**
 * @brief 
 *      Rtsi recipe. 
 *      This class just can be got from the function in RtsiClientInterface.
 */
class RtsiRecipe {
public:
    ELITE_EXPORT virtual ~RtsiRecipe() = default;

    /**
     * @brief Retrieve the value corresponding to the variable name in the recipe.
     * 
     * @tparam T The type of output variable, support bool, uint8_t, uint16_t, uint32_t, uint64_t, int32_t, double, vector3d_t, vector6d_t, vector6int32_t, vector6uint32_t
     * @param name The variable name
     * @param out_value Output value
     * @return true success
     * @return false fail
     */
    template<typename T>
    bool getValue(const std::string& name, T& out_value) {
        std::lock_guard<std::mutex> lock(update_mutex_);
        auto iter = value_table_.find(name);
        if (iter != value_table_.end()) {
#if (ELITE_SDK_COMPILE_STANDARD >= 17)
            out_value = std::get<T>(iter->second);
#elif (ELITE_SDK_COMPILE_STANDARD == 14)
            out_value = *(boost::get<T>(&iter->second));
#endif
            return true;
        }
        return false;
    }

    /**
     * @brief Set the value corresponding to the variable name in the recipe
     * 
     * @tparam T The type of variable, support bool, uint8_t, uint16_t, uint32_t, uint64_t, int32_t, double, vector3d_t, vector6d_t, vector6int32_t, vector6uint32_t
     * @param name The variable name
     * @param value The value will be writed
     * @return true success
     * @return false fail
     */
    template<typename T>
    bool setValue(const std::string& name, const T& value) {
        std::lock_guard<std::mutex> lock(update_mutex_);
        auto iter = value_table_.find(name);
        if (iter != value_table_.end()) {
            return setVariantValue(iter->second, value);
        }
        return false;
    }

    /**
     * @brief Get the list of variable names
     * 
     * @return const std::vector<std::string>& The list of variable names
     */
    ELITE_EXPORT const std::vector<std::string>& getRecipe() const { return recipe_list_; }

    /**
     * @brief Get the recipe ID, range 1 ~ 254
     * 
     * @return int The recipe ID
     */
    ELITE_EXPORT int getID() const { return recipe_id_; }

protected:
    RtsiRecipe() = default;
    std::vector<std::string> recipe_list_;
    std::unordered_map<std::string, RtsiTypeVariant> value_table_;
    std::atomic<int> recipe_id_;
    std::mutex update_mutex_;

private:
    template<typename T>
    bool setVariantValue(RtsiTypeVariant& out_value, T value) {
        static_assert(std::is_fundamental<T>::value, "must use base type");
#if (ELITE_SDK_COMPILE_STANDARD >= 17)
        if (std::holds_alternative<bool>(out_value)) {
            bool temp = value;
            out_value = temp;
        } else if(std::holds_alternative<uint8_t>(out_value)) {
            uint8_t temp = value;
            out_value = temp;
        } else if(std::holds_alternative<uint16_t>(out_value)) {
            uint16_t temp = value;
            out_value = temp;
        } else if(std::holds_alternative<uint32_t>(out_value)) {
            uint32_t temp = value;
            out_value = temp;
        } else if(std::holds_alternative<uint64_t>(out_value)) {
            uint64_t temp = value;
            out_value = temp;
        } else if(std::holds_alternative<int32_t>(out_value)) {
            int32_t temp = value;
            out_value = temp;
        } else if(std::holds_alternative<double>(out_value)) {
            double temp = value;
            out_value = temp;
        } else {
            return false;
        }
#elif (ELITE_SDK_COMPILE_STANDARD == 14)
        if (boost::get<bool>(&out_value)) {
            out_value = (bool)value;
        } else if(boost::get<uint8_t>(&out_value)) {
            out_value = (uint8_t)value;
        } else if(boost::get<uint16_t>(&out_value)) {
            out_value = (uint16_t)value;
        } else if(boost::get<uint32_t>(&out_value)) {
            out_value = (uint32_t)value;
        } else if(boost::get<uint64_t>(&out_value)) {
            out_value = (uint64_t)value;
        } else if(boost::get<int32_t>(&out_value)) {
            out_value = (int32_t)value;
        } else if(boost::get<double>(&out_value)) {
            out_value = (double)value;
        } else {
            return false;
        }
#endif
        return true;
    }

    bool setVariantValue(RtsiTypeVariant& out_value, const vector3d_t& value) {
#if (ELITE_SDK_COMPILE_STANDARD >= 17)
        if(std::holds_alternative<vector3d_t>(out_value)) {
            out_value = value;
            return true;
        }
#elif (ELITE_SDK_COMPILE_STANDARD == 14)
        if(boost::get<vector3d_t>(&out_value)) {
            out_value = value;
            return true;
        }
#endif
        return false;
    }

    bool setVariantValue(RtsiTypeVariant& out_value, const vector6d_t& value) {
#if (ELITE_SDK_COMPILE_STANDARD >= 17)
        if(std::holds_alternative<vector6d_t>(out_value)) {
            out_value = value;
            return true;
        }
#elif (ELITE_SDK_COMPILE_STANDARD == 14)
        if(boost::get<vector6d_t>(&out_value)) {
            out_value = value;
            return true;
        }
#endif
        return false;
    }

    bool setVariantValue(RtsiTypeVariant& out_value, const vector6int32_t& value) {
#if (ELITE_SDK_COMPILE_STANDARD >= 17)
        if(std::holds_alternative<vector6int32_t>(out_value)) {
            out_value = value;
            return true;
        }
#elif (ELITE_SDK_COMPILE_STANDARD == 14)
        if(boost::get<vector6int32_t>(&out_value)) {
            out_value = value;
            return true;
        }
#endif
        return false;
    }

    bool setVariantValue(RtsiTypeVariant& out_value, const vector6uint32_t& value) {
#if (ELITE_SDK_COMPILE_STANDARD >= 17)
        if(std::holds_alternative<vector6uint32_t>(out_value)) {
            out_value = value;
            return true;
        }
#elif (ELITE_SDK_COMPILE_STANDARD == 14)
        if(boost::get<vector6uint32_t>(&out_value)) {
            out_value = value;
            return true;
        }
#endif
        return false;
    }

};

using RtsiRecipeSharedPtr = std::shared_ptr<RtsiRecipe>;

} // namespace ELITE


#endif
