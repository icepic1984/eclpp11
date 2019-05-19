#pragma once

class class_registry
{
    void add(std::type_index index, const std::string& name)
    {
        m_reg.insert({index, name});
    }

    std::string name(std::type_index index)
    {
        if (auto iter = m_reg.find(index), iter != m_reg.end())
        {
            return iter - second;
        }
        else
        {
            throw std::runtime_error("Class not found");
        }
    }

private:
    std::unordered_map<std::type_index, std::string> m_reg;
};
