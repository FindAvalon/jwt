//
//  main.cpp
//  jwt
//
//  Created by lkg on 2017/9/21.
//  Copyright © 2017年 lkg. All rights reserved.
//

#include "phpx.h"

#include <iostream>
#include <vector>

using namespace php;
using namespace std;

int main(int argc, const char * argv[]) {
    // insert code here...
    return 0;
}

string urlsafe_b64_encode(string str)
{
    string base64_str = php::exec("base64_encode", str).toString();
    
    base64_str = php::exec("strtr", base64_str, "+/", "-_").toString();
    return php::exec("str_replace", "=", "", base64_str).toString();
}

Variant urlsafe_b64_decode(string str)
{
    int remainder = str.length() % 4;
    
    if (remainder) {
        int padlen = 4 - remainder;
        str += php::exec("str_repeat", padlen).toString();
    }
    str = php::exec("strtr", str, "-_", "+/").toString();
    return php::exec("base64_decode", str);
}

string jwt_encrypt(string alg, string data, string key)
{
    string output_code;
    if ( alg == "HS256" ) {
        output_code = php::exec("hash_hmac", "sha256", data, key, true).toString();
    } else {
        return "";
    }
    return urlsafe_b64_encode(output_code);
}

PHPX_FUNCTION(jwt_encode)
{
    if ( args.count() < 2 ) {
        php::error(E_ERROR, "jwt_encode() expects at least 2 parameter");
    }
    auto payload = args[0];
    
    if ( !payload.isArray() ) {
        php::error(E_ERROR, "Argument #1 is not an array");
    }
    string payload_str = (payload.jsonEncode()).toString();
    
    auto key = args[1];
    
    if ( !(key.isInt() || key.isString()) ) {
        php::error(E_ERROR, "Argument #2 is not a string or a int");
    }
    string alg;
    
    if ( args.count() == 3 ) {
        if ( !args[2].isString() ) {
            php::error(E_ERROR, "Argument #3 is not a string");
        }
        alg = args[2].toString();
        
    } else {
        alg = "HS256";
    }
    Array header;
    header.set("alg", alg);
    header.set("typ", "JWT");
    string header_str = header.jsonEncode().toString();
    string key_str(key.toString());
    string str(header_str + "." + payload_str);
    
    string output = urlsafe_b64_encode(header_str) + "." + urlsafe_b64_encode(payload_str);
    
    string secret = jwt_encrypt(alg, output, key_str);
    output = output + "." + secret;
    retval = output;
}

PHPX_FUNCTION(jwt_decode)
{
    if ( args.count() < 2 ) {
        php::error(E_ERROR, "jwt_decode() expects at least 2 parameter");
    }
    
    Variant data = args[0];
    auto key = args[1];
    
    Array result_n;
    
    if ( !data.isString() ) {
        retval = result_n;
    } else if ( !key.isString() ) {
        retval = result_n;
    } else {
        Variant str_split = php::exec("explode", ".", data);
        Array data_arr(str_split);
        
        int len = data_arr.count();
        if ( len != 3 ) {
            retval = result_n;
        } else {
            string header_str = data_arr[0].toString();
            string payload_str = data_arr[1].toString();
            string secret_str = data_arr[2].toString();
            
            Variant key_v(urlsafe_b64_decode(header_str));
            Variant key_json = key_v.jsonDecode();
            Array key_arr(key_json);
            Variant alg_s("alg");
            auto alg_v = key_arr[alg_s.toCString()].toString();
            
            string key_s = jwt_encrypt(alg_v, header_str+"."+payload_str, key.toString());
            
            if ( secret_str != key_s ) {
                retval = result_n;
            } else {
                Variant result = urlsafe_b64_decode(payload_str);
                retval = result.jsonDecode();
            }
        }
    }
}

PHPX_EXTENSION()
{
    Extension *extension = new Extension("jwt", "0.0.1");
    extension->require("json");
    
    extension->registerFunction(PHPX_FN(jwt_encode));
    extension->registerFunction(PHPX_FN(jwt_decode));
    
    extension->info(
                    {
                        "jwt support", "enabled"
                    },
                    {
                        { "author", "LKG" },
                        { "version", extension->version },
                        { "date", "2017-05-22" },
                    });
    
    return extension;
}
