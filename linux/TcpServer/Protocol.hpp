#pragma once

#include <iostream>
#include <string>
#include <jsoncpp/json/json.h>

const std::string blank_space_sep = " ";
const std::string protocol_sep = "\n";

std::string Encode(std::string& content){
    std::string package = std::to_string(content.size());
    package += protocol_sep;
    package += content;
    package += protocol_sep;

    return package;
}

// "len"\n"x op y"\nXXXXXXXXXXXXX
bool Decode(std::string&package, std::string* content){
    size_t pos = package.find(protocol_sep);
    if(pos == std::string::npos){
        return false;
    }
    std::string len_str = package.substr(0,pos);
    size_t len = std::stoi(len_str);
    size_t total_len = len_str.size() + len + 2;
    if(package.size() < total_len){
        return false;
    }

    *content = package.substr(pos+1, len);
    package.erase(0, total_len);
    return true;
}

class Requst{
public:
    Requst(){}

    Requst(int data1, int data2, char oper)
        :x(data1),
        y(data2),
        op(oper)
    {}

public:
    bool Serialize(std::string* out){
        #ifdef Myself

        std::string s = std::to_string(x);
        s += blank_space_sep;
        s += op;
        s += blank_space_sep;
        s += std::to_string(y);
        *out = s;
        return true;

        #else

        Json::Value root;
        root["x"] = x;
        root["y"] = y;
        root["op"] = op;
        // Json::FastWriter w;
        Json::StyledWriter w;
        *out = w.write(root);
        return true;

        #endif
    }

    bool Deserialize(const std::string& in){
        #ifdef Myself

        std::size_t left = in.find(blank_space_sep);
        if(left == std::string::npos){
            return false;
        }
        std::string part_x = in.substr(0, left);

        size_t right = in.rfind(blank_space_sep);
        if(right == std::string::npos){
            return false;
        }
        std::string part_y = in.substr(right + 1);

        if(left + 2 != right){
            return false;
        }
        op = in[left + 1];
        x = std::stoi(part_x);
        y = std::stoi(part_y);
        return true;

        #else

        Json::Value root;
        Json::Reader r;
        r.parse(in, root);

        x = root["x"].asInt();
        y = root["y"].asInt();
        op = root["op"].asInt();
        return true;

        #endif
    }

    void DebugPrint(){
        std::cout << "新请求构建完成: " << x << op << y << "= ? " << std::endl;
    }

public:
    int x;
    int y;
    char op;
};

class Response{
public:
    Response(){}

    Response(int res, int c)
        :result(res),
        code(c)
    {}

public:
    bool Serialize(std::string* out){
        #ifdef Myself

        std::string s = std::to_string(result);
        s += blank_space_sep;
        s += std::to_string(code);
        *out = s;
        return true;

        #else

        Json::Value root;
        root["result"] = result;
        root["code"] = code;
        // Json::FastWriter w;
        Json::StyledWriter w;
        *out = w.write(root);
        return true;

        #endif
    }

    bool Deserialize(const std::string& in){
        #ifdef Myself

        size_t pos = in.find(blank_space_sep);
        if(pos == std::string::npos){
            return false;
        }
        std::string part_left = in.substr(0, pos);
        std::string part_rifht = in.substr(pos + 1);

        result = std::stoi(part_left);
        code = std::stoi(part_right);
        return true;

        #else

        Json::Value root;
        Json::Reader r;
        r.parse(in, root);

        result = root["result"].asInt();
        code = root["code"].asInt();
        return true;

        #endif
    }

    void DebugPrint(){
        std::cout << "结果响应完成" << result << " , code: " << code << std::endl;
    }

public:
    int result;
    int code;
};