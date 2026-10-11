#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>

std::string default_path = "./dict.txt";

class Dict
{
public:
    Dict(std::string path = default_path)
    :_path(path)
    {
        Load(); // dict一创建就是就绪状态，可以直接调用Translate
    }

    // 加载字典内容
    void Load()
    {
        // 打开文件，涉及到文件流
        std::ifstream in(_path); 

        if (!in.is_open())
        {
            LOG(LogLevel::FATAL) << "Load 打开文件出错，路径：" << _path;
            exit(1);
        }

        // 逐行读
        std::string line;
        while (std::getline(in, line))
        {   // line就是文件中的一整行
            // 然后切行入库
            size_t pos = line.find(": ");
            
            if (pos == std::string::npos)
            {
                LOG(LogLevel::WARNING) << "format error, skip: " << line;
                continue;
            }

            std::string word = line.substr(0, pos); // 切字串
            std::string mean = line.substr(pos+2);  // 后一个参数不填，默认到尾
            _dict[word] = mean; // 装入容器，方便Translate查取
        }
        // 没有fclose，因为是对象出作用域就自动关        
    }

    // 这个字典，一定要能够翻译，你把单词交给我，我把结果交给你
    std::string Translate(const std::string& word)
    {
        // 查_dict
        // 返回单词释义
        auto it = _dict.find(word);
        if (it == _dict.end())
        {
            return "未找到，单词不存在";
        }
        return it->second;
    }

    ~Dict()
    {}
private:
    std::string _path;
    std::unordered_map<std::string, std::string> _dict;
};