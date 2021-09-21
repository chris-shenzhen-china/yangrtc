

#include "YangThread.h"

#include <iostream>
using namespace std;

YangThread::YangThread(){
	m_thread=0;
}
YangThread::~YangThread(){

}

int YangThread::start()
{
    if (pthread_create( &m_thread, 0, &YangThread::go, this))
    {
        cerr << "YangThread::start could not start thread" << endl;
        return -1;
    }

    return 0;
}

void* YangThread::go(void* obj)
{
    reinterpret_cast<YangThread*>(obj)->run();
    return NULL;
}

void* YangThread::join()
{
    void* ret;
    pthread_join(m_thread, &ret);
    return ret;
}

pthread_t YangThread::getThread()
{
    return m_thread;
}

int YangThread::detach()
{
    return pthread_detach(m_thread);
}

int YangThread::equals(YangThread* t)
{
    return pthread_equal(m_thread, t->getThread());
}

void YangThread::exitThread(void* value_ptr)
{
    pthread_exit(value_ptr);
}

int YangThread::cancel()
{
    return pthread_cancel(m_thread);
}

