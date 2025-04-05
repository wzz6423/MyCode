#pragma once

#include <iostream>
#include "Protocol.hpp"

enum{
    Div_Zero = 1,
    Mod_Zero,
    Other_Oper
};

class Calculator{
public:
    Calculator(){}
    ~Calculator(){}

public:
    Response CalculatorHelper(const Requst& req){
        Response resp(0, 0);
        switch(req.op){
            case '+':
                resp.result = req.x + req.y;
                break;
            case '-':
                resp.result = req.x - req.y;
                break;
            case '*':
                resp.result = req.x * req.y;
                break;
            case '/':{
                if(req.y == 0){
                    resp.code = Div_Zero;
                }
                else{
                    resp.result = req.x / req.y;
                }
            }
                break;
            case '%':{
                    if(req.y == 0){
                        resp.code = Div_Zero;
                    }
                    else{
                        resp.result = req.x % req.y;
                    }
            }
                break;
            default:
                resp.code = Other_Oper;
                break;
        }
        return resp;
    }

    std::string Handler(std::string& package){
        std::string content;
        bool r = Decode(package, &content);
        if(!r){
            return "";
        }
        Requst req;
        r = req.Deserialize(content);
        if(!r){
            return "";
        }

        content = "";
        Response resp = CalculatorHelper(req);

        resp.Serialize(&content);
        content = Encode(content);

        return content;
    }
};