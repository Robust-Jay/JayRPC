#include "jayrpcconfig.h"

namespace JayRPC
{
    void strip(std::string &str)
    {
        int idx = str.find_first_not_of(' ');
        if (idx != -1)
        {
            str = str.substr(idx, str.size() - idx);
        }
        idx = str.find_last_not_of(' ');
        if (idx != -1)
        {
            str = str.substr(0, idx + 1);
        }
    }

    void JayRpcConfig::LoadConfigFile(const char *configfile)
    {
        std::ifstream pf(configfile, std::ios::in);
        if (!pf.is_open())
        {
            std::cout << configfile << " not exist!" << std::endl;
            exit(EXIT_FAILURE);
        }

        std::string str;
        while (getline(pf, str))
        {
            strip(str);
            if (str.empty() || str[0] == '#')
            {
                continue;
            }

            int idx = str.find('=');
            if (idx == -1)
            {
                continue;
            }

            std::string key;
            std::string value;
            key = str.substr(0, idx);
            strip(key);
            value = str.substr(idx + 1, str.size() - idx);
            strip(value);

            __configMap.insert({key, value});
        }

        pf.close();
    }

    std::string JayRpcConfig::Load(const std::string &key)
    {
        auto it = __configMap.find(key);
        if (it == __configMap.end())
        {
            return "";
        }
        return it->second;
    }

} // namespace JayRPC