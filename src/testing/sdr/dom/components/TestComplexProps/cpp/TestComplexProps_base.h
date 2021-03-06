#ifndef TESTCOMPLEXPROPS_IMPL_BASE_H
#define TESTCOMPLEXPROPS_IMPL_BASE_H

#include <boost/thread.hpp>
#include <ossie/Resource_impl.h>

#include "struct_props.h"

#define NOOP 0
#define FINISH -1
#define NORMAL 1

class TestComplexProps_base;

template < typename TargetClass >
class ProcessThread
{
    public:
        ProcessThread(TargetClass *_target, float _delay) :
            target(_target)
        {
            _mythread = 0;
            _thread_running = false;
            _udelay = (__useconds_t)(_delay * 1000000);
        };

        // kick off the thread
        void start() {
            if (_mythread == 0) {
                _thread_running = true;
                _mythread = new boost::thread(&ProcessThread::run, this);
            }
        };

        // manage calls to target's service function
        void run() {
            int state = NORMAL;
            while (_thread_running and (state != FINISH)) {
                state = target->serviceFunction();
                if (state == NOOP) usleep(_udelay);
            }
        };

        // stop thread and wait for termination
        bool release(unsigned long secs = 0, unsigned long usecs = 0) {
            _thread_running = false;
            if (_mythread)  {
                if ((secs == 0) and (usecs == 0)){
                    _mythread->join();
                } else {
                    boost::system_time waitime= boost::get_system_time() + boost::posix_time::seconds(secs) +  boost::posix_time::microseconds(usecs) ;
                    if (!_mythread->timed_join(waitime)) {
                        return 0;
                    }
                }
                delete _mythread;
                _mythread = 0;
            }
    
            return 1;
        };

        virtual ~ProcessThread(){
            if (_mythread != 0) {
                release(0);
                _mythread = 0;
            }
        };

        void updateDelay(float _delay) { _udelay = (__useconds_t)(_delay * 1000000); };

    private:
        boost::thread *_mythread;
        bool _thread_running;
        TargetClass *target;
        __useconds_t _udelay;
        boost::condition_variable _end_of_run;
        boost::mutex _eor_mutex;
};

class TestComplexProps_base : public Resource_impl
{
    public:
        TestComplexProps_base(const char *uuid, const char *label);

        void start() throw (CF::Resource::StartError, CORBA::SystemException);

        void stop() throw (CF::Resource::StopError, CORBA::SystemException);

        void releaseObject() throw (CF::LifeCycle::ReleaseError, CORBA::SystemException);

        void initialize() throw (CF::LifeCycle::InitializeError, CORBA::SystemException);

        void loadProperties();

        virtual int serviceFunction() = 0;

    protected:
        ProcessThread<TestComplexProps_base> *serviceThread; 
        boost::mutex serviceThreadLock;

        // Member variables exposed as properties
        std::complex<bool>  complexBooleanProp;
        std::complex<CORBA::ULong>  complexULongProp;
        std::complex<short>  complexShortProp;
        std::complex<float>  complexFloatProp;
        std::complex<unsigned char>  complexOctetProp;
        std::complex<char>  complexCharProp;
        std::complex<unsigned short>  complexUShort;
        std::complex<double>  complexDouble;
        std::complex<CORBA::Long>  complexLong;
        std::complex<CORBA::LongLong>  complexLongLong;
        std::complex<CORBA::ULongLong>  complexULongLong;
        std::vector<std::complex<float> > complexFloatSequence;
        FloatStruct_struct FloatStruct;
        complexFloatStruct_struct complexFloatStruct;
        std::vector<FloatStructSequenceMember_struct> FloatStructSequence;
        std::vector<complexFloatStructSequenceMember_struct> complexFloatStructSequence;

    private:
        void construct();

};
#endif
