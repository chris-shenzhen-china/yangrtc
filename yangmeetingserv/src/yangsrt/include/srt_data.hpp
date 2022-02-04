﻿//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef SRT_DATA_H
#define SRT_DATA_H

//#include <srs_core.hpp>

#include <string>
#include <memory>

#define SRT_MSG_DATA_TYPE  0x01
#define SRT_MSG_CLOSE_TYPE 0x02

class SRT_DATA_MSG {
public:
    SRT_DATA_MSG();
    SRT_DATA_MSG(unsigned int len);
    SRT_DATA_MSG(unsigned char* data_p, unsigned int len);
    ~SRT_DATA_MSG();

    unsigned int msg_type();
    unsigned int data_len();
    unsigned char* get_data();
   // std::string get_path();

private:
    unsigned int   _msg_type;
    unsigned int   _len;
    unsigned char* _data_p;
 //   std::string _key_path;
};

typedef std::shared_ptr<SRT_DATA_MSG> SRT_DATA_MSG_PTR;

#endif
