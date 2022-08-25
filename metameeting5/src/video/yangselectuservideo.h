﻿//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef YANGSELECTUSERVIDEO_H
#define YANGSELECTUSERVIDEO_H
#include <QListView>
#include <QDialog>
#include <QHBoxLayout>
#include <QTableWidget>
#include "../yangwinutil/yangqInterfaces.h"
#include "yangutil/sys/YangSysMessageI.h"
#include "../yangwinutil/yangvideocontext.h"
#include "yangvideotype.h"
class YangSelectUserVideo:public QDialog,public YangLinkButtonI
{
     Q_OBJECT
public:
    YangSelectUserVideo(QWidget *parent = nullptr);
    ~ YangSelectUserVideo();
     QTableWidget *m_table;
    QHBoxLayout *m_hb;
    QList<QString> m_list;
    QWidget *m_widget;
    void init();
    void setIndex(int pindex);
    int32_t *m_uid;
    int32_t wid;
    YangVideoTypeI *m_videoHandle;
    YangSysMessageI *m_message;
    YangVideoContext* m_context;
private:
    int32_t m_init=0;
};

#endif // YANGSELECTUSERVIDEO_H
