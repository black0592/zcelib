/*!
* @copyright  2004-2013  Apache License, Version 2.0 FULLSAIL
* @filename   zce_os_adapt_time.cpp
* @author     Sailzeng <sailerzeng@gmail.com>
* @version    
* @date       Tuesday, December 31, 2011
* @brief      
*             
*             
* @details    ʱ��������������㣬��Ҫ������LINUX�¿�£
*             
*             
*             
* @note       
*             
*/

#include "zce_predefine.h"
#include "zce_os_adapt_predefine.h"
#include "zce_os_adapt_mutex.h"
#include "zce_lock_thread_mutex.h"
#include "zce_trace_log_debug.h"
#include "zce_os_adapt_error.h"
#include "zce_os_adapt_time.h"

//Ϊʲô��������ACE�������ޣ��������ޣ��������������������������ޣ���������������

//�����������д�˴�����long��Ҫ��Ϊ�˼���,�Ȳ�����

//�Ǳ�׼�������õ�������������ʱ�䣬gethrtime��λ�ȽϹ֣����Ƶõ�CPU�����������ڵ�TICK��ʱ�䣬
//ȱ�㣺������ε���֮���ʱ���������������49�죬���޷���֤��õ�׼ȷ��ֵ
//���ϲ�Ҫ49���ֻ����һ���������ѽ�������ұ�֤�������TICK��Ч������������ÿ�����һ�ΰɡ�
//�ڲ�Ϊ����һ�ε��õ�ʱ�䣬����static ��������Ϊ�˱���static����������
const timeval ZCE_OS::get_uptime()
{

#if defined (ZCE_OS_WINDOWS)

    //ע��GetTickCount64��GetTickCount���صĶ���milliseconds������CPU Tick
    const uint64_t SEC_PER_MSEC = 1000LL;
    const uint64_t MSEC_PER_USEC = 1000LL;

    timeval up_time;
    uint64_t now_cpu_tick = 0;

    //Ϊʲô��������GetTickCount64 ,(Vista��֧��),��������ע�͵�ԭ���ǣ������ͨ���ˣ�����Ҳû����,XP��WINSERVER2003���޷�ʹ�ã�
    //VISTA,WINSERVER2008��_WIN32_WINNT����0x0600
#if defined ZCE_SUPPORT_WINSVR2008 && ZCE_SUPPORT_WINSVR2008 == 1
    now_cpu_tick = ::GetTickCount64();
#else

    //GetTickCount���ص���һ��32λ������milliseconds �����ǣ�DWORD��ʵֻ��49����ĳ���,����ֻ�е��۵ķ�װ��
    unsigned int cpu_tick =  static_cast<unsigned int>(::GetTickCount());

    //���˾�̬��������ֹ����
    ZCE_Thread_Light_Mutex lock_static_var;
    ZCE_Thread_Light_Mutex::LOCK_GUARD guard(lock_static_var);

    static unsigned int one_period_tick = 0;
    static uint64_t cpu_tick_count = 0;

    //�����GetTickCountû�й��㣬�����ݿ϶����ھ�����
    if (one_period_tick  <= cpu_tick)
    {
        one_period_tick = cpu_tick;
        cpu_tick_count = (0xFFFFFFFF00000000 & cpu_tick_count) + one_period_tick;
    }
    //������else�������ת��1Ȧ��
    else
    {
        cpu_tick_count +=  0xFFFFFFFF - one_period_tick + cpu_tick;
        one_period_tick = cpu_tick;
    }

    now_cpu_tick = cpu_tick_count;

#endif //

    up_time.tv_sec = static_cast<long>( now_cpu_tick / SEC_PER_MSEC);
    up_time.tv_usec = static_cast<long>( now_cpu_tick % SEC_PER_MSEC * MSEC_PER_USEC);

    return up_time;

#elif defined (ZCE_OS_LINUX)
    //��ù�ķ���LINUX�ܶ�汾��û��֧�����gethrtime�������ҿ�����������
    struct timespec sp;
    timeval up_time;
    int ret = ::clock_gettime(CLOCK_MONOTONIC, &sp);

    if (ret == 0)
    {
        up_time = ZCE_OS::make_timeval(&sp);
    }
    else
    {
        ZLOG_ERROR("::clock_gettime(CLOCK_MONOTONIC, &sp) ret != 0,fail.ret = %d lasterror = %d", ret, ZCE_OS::last_error());
        up_time.tv_sec = 0;
        up_time.tv_usec = 0;
    }

    return up_time;
#endif
}

//
//�õ���ǰ��ϵͳʱ���ַ������
const char *ZCE_OS::timestamp (char *str_date_time, size_t datetime_strlen)
{
    timeval now_time_val (ZCE_OS::gettimeofday());
    return ZCE_OS::timestamp (&now_time_val, str_date_time, datetime_strlen);
}

//������timeval��ֵ��Ϊ��ʱ����ʽ���������ӡ����
const char *ZCE_OS::timestamp (const timeval *timeval, char *str_date_time, size_t datetime_strlen)
{
    ZCE_ASSERT (datetime_strlen > ZCE_OS::LEN_OF_TIME_STAMP);

    //ת��Ϊ���
    time_t now_time = timeval->tv_sec;
    tm tm_data;
    ZCE_OS::localtime_r(&now_time, &tm_data);

    //��������д�����ԭ���õ�����һ�д��룬���ǻ���ֱ���(Windows�µĶ���),��֪��Ϊɶ�𣬺Ǻ�
    //tm now_tm =*localtime(static_cast<time_t *>(&(timeval->tv_sec)));

    snprintf (str_date_time,
              datetime_strlen,
              "%4d-%02d-%02d %02d:%02d:%02d.%06ld",
              tm_data.tm_year + 1900,
              tm_data.tm_mon + 1,
              tm_data.tm_mday,
              tm_data.tm_hour,
              tm_data.tm_min,
              tm_data.tm_sec,
              timeval->tv_usec);

    return str_date_time;
}

//������timeval��ֵ��Ϊ��ʱ����ʽ���������ӡ����
//���Կ��Ƹ��ָ�ʽ���
//����ɹ������ز����ַ���str_date_time�����ʧ�ܷ���NULL
const char *ZCE_OS::timestamp_ex(const timeval *timeval,
                                 char *str_date_time,
                                 size_t datetime_strlen,
                                 int fromat_type)
{
    time_t now_time = timeval->tv_sec;
    tm tm_data;

    if (ZCE_BIT_IS_SET(fromat_type, ZCE_OS::TIME_STRFFMT_LOCALTIME))
    {
        ZCE_OS::localtime_r(&now_time, &tm_data);
    }
    else if (ZCE_BIT_IS_SET(fromat_type, ZCE_OS::TIME_STRFFMT_UTCTIME))
    {
        ZCE_OS::gmtime_r(&now_time, &tm_data);
    }
    else
    {
        ZCE_ASSERT(false);
        errno = EINVAL;
        return NULL;
    }

    static const char *DAY_OF_WEEK_NAME[] =
    {
        ("Sun"),
        ("Mon"),
        ("Tue"),
        ("Wed"),
        ("Thu"),
        ("Fri"),
        ("Sat")
    };

    static const char *MONTH_NAME[] =
    {
        ("Jan"),
        ("Feb"),
        ("Mar"),
        ("Apr"),
        ("May"),
        ("Jun"),
        ("Jul"),
        ("Aug"),
        ("Sep"),
        ("Oct"),
        ("Nov"),
        ("Dec")
    };

    //�����ISO��ʽ���ο���.h�ļ�
    if (ZCE_BIT_IS_SET(fromat_type, ZCE_OS::TIME_STRFFMT_ISO))
    {
        //���������ȵ�΢��
        if (ZCE_BIT_IS_SET(fromat_type, ZCE_OS::TIME_STRFFMT_PRECISION_USEC))
        {
            //���������ͼ��
            ZCE_ASSERT(datetime_strlen > ZCE_OS::LEN_OF_ISO_USEC_TIMESTRING);

            if (datetime_strlen <= ZCE_OS::LEN_OF_ISO_USEC_TIMESTRING)
            {
                return NULL;
            }

            snprintf (str_date_time,
                      datetime_strlen,
                      "%4d-%02d-%02d %02d:%02d:%02d.%06ld",
                      tm_data.tm_year + 1900,
                      tm_data.tm_mon + 1,
                      tm_data.tm_mday,
                      tm_data.tm_hour,
                      tm_data.tm_min,
                      tm_data.tm_sec,
                      timeval->tv_usec);
        }
        //������ȵ���
        else if (ZCE_BIT_IS_SET(fromat_type, ZCE_OS::TIME_STRFFMT_PRECISION_SEC))
        {
            //���������ͼ��
            ZCE_ASSERT(datetime_strlen > ZCE_OS::LEN_OF_ISO_SEC_TIMESTRING);

            if (datetime_strlen <= ZCE_OS::LEN_OF_ISO_SEC_TIMESTRING)
            {
                return NULL;
            }

            snprintf (str_date_time,
                      datetime_strlen,
                      "%4d-%02d-%02d %02d:%02d:%02d",
                      tm_data.tm_year + 1900,
                      tm_data.tm_mon + 1,
                      tm_data.tm_mday,
                      tm_data.tm_hour,
                      tm_data.tm_min,
                      tm_data.tm_sec);
        }
        //ֻ������ȵ���
        else if (ZCE_BIT_IS_SET(fromat_type, ZCE_OS::TIME_STRFFMT_PRECISION_DAY))
        {
            //���������ͼ��
            ZCE_ASSERT(datetime_strlen > ZCE_OS::LEN_OF_ISO_DAY_TIMESTRING);

            if (datetime_strlen <= ZCE_OS::LEN_OF_ISO_DAY_TIMESTRING)
            {
                return NULL;
            }

            snprintf (str_date_time,
                      datetime_strlen,
                      "%4d-%02d-%02d",
                      tm_data.tm_year + 1900,
                      tm_data.tm_mon + 1,
                      tm_data.tm_mday);
        }
        else
        {
            ZCE_ASSERT(false);
            errno = EINVAL;
            return NULL;
        }
    }
    //�������ʽʱ���ʽ������ο���.h�ļ�
    else if ( ZCE_BIT_IS_SET(fromat_type, ZCE_OS::TIME_STRFFMT_US) )
    {
        //������ȵ�΢��
        if (ZCE_BIT_IS_SET(fromat_type, ZCE_OS::TIME_STRFFMT_PRECISION_USEC))
        {
            //���������ͼ��
            ZCE_ASSERT(datetime_strlen > LEN_OF_US_USEC_TIMESTRING);

            if (datetime_strlen <= LEN_OF_US_USEC_TIMESTRING)
            {
                return NULL;
            }

            snprintf (str_date_time,
                      datetime_strlen,
                      "%3s %3s %2d %04d %02d:%02d:%02d.%06d",
                      DAY_OF_WEEK_NAME[tm_data.tm_wday],
                      MONTH_NAME[tm_data.tm_mon],
                      tm_data.tm_mday,
                      tm_data.tm_year + 1900,
                      tm_data.tm_hour,
                      tm_data.tm_min,
                      (int)tm_data.tm_sec,
                      (int)timeval->tv_usec);
        }
        //������ȵ���
        else if (ZCE_BIT_IS_SET(fromat_type, ZCE_OS::TIME_STRFFMT_PRECISION_SEC))
        {

            ZCE_ASSERT(datetime_strlen > LEN_OF_US_SEC_TIMESTRING);

            if (datetime_strlen <= LEN_OF_US_SEC_TIMESTRING)
            {
                return NULL;
            }

            snprintf (str_date_time,
                      datetime_strlen,
                      "%3s %3s %2d %04d %02d:%02d:%02d",
                      DAY_OF_WEEK_NAME[tm_data.tm_wday],
                      MONTH_NAME[tm_data.tm_mon],
                      tm_data.tm_mday,
                      tm_data.tm_year + 1900,
                      tm_data.tm_hour,
                      tm_data.tm_min,
                      tm_data.tm_sec);
        }
        //ֻ������ȵ���
        else if (ZCE_BIT_IS_SET(fromat_type, ZCE_OS::TIME_STRFFMT_PRECISION_DAY))
        {

            ZCE_ASSERT(datetime_strlen > LEN_OF_US_DAY_TIMESTRING);

            if (datetime_strlen <= LEN_OF_US_SEC_TIMESTRING)
            {
                return NULL;
            }

            snprintf (str_date_time,
                      datetime_strlen,
                      "%3s %3s %2d %04d",
                      DAY_OF_WEEK_NAME[tm_data.tm_wday],
                      MONTH_NAME[tm_data.tm_mon],
                      tm_data.tm_mday,
                      tm_data.tm_year + 1900);
        }
        else
        {
            ZCE_ASSERT(false);
            errno = EINVAL;
            return NULL;
        }
    }
    else
    {
        ZCE_ASSERT(false);
        errno = EINVAL;
        return NULL;
    }

    return str_date_time;

}

//----------------------------------------------------------------------------------------------------
const timeval ZCE_OS::timeval_zero()
{
    timeval zero_time;
    zero_time.tv_sec = 0;
    zero_time.tv_usec = 0;
    return zero_time;
}

//��tv����Ϊ0
void ZCE_OS::timeval_clear(timeval &tv)
{
    tv.tv_sec = 0;
    tv.tv_sec = 0;
}

//�����ܼ��Ƕ��ٺ���
uint64_t ZCE_OS::total_milliseconds(const timeval &tv)
{
    //����Ĳ���������Ϊ��Ҫת�������������ڵġ�
    const uint32_t SEC_PER_MSEC = 1000;
    const uint32_t MSEC_PER_USEC = 1000;
    return static_cast<uint64_t>(tv.tv_sec) * SEC_PER_MSEC + tv.tv_usec / MSEC_PER_USEC;
}

//����timeval�ڲ��ܼ��Ƕ���΢��
uint64_t ZCE_OS::total_microseconds(const timeval &tv)
{
    //����Ĳ���������Ϊ��Ҫת�������������ڵġ�
    const uint32_t SEC_PER_USEC = 1000;
    return static_cast<uint64_t>(tv.tv_sec) * SEC_PER_USEC + tv.tv_usec ;
}

//�Ƚ�ʱ���Ƿ�һ��,���һ�·���0��left�󣬷���������right�󷵻ظ���
int ZCE_OS::timeval_compare(const  timeval &left, const timeval &right)
{
    if ( left.tv_sec != right.tv_sec )
    {
        return left.tv_sec - right.tv_sec;
    }
    else
    {
        return left.tv_usec - right.tv_usec;
    }
}

//������ʱ��������,û�������ӵ�������
const timeval ZCE_OS::timeval_add(const timeval &left, const timeval &right)
{
    const int32_t SEC_PER_USEC = 1000000;
    timeval plus_time_val;
    plus_time_val.tv_sec = left.tv_sec + right.tv_sec;
    plus_time_val.tv_usec = left.tv_usec + right.tv_usec;

    if (plus_time_val.tv_usec > SEC_PER_USEC)
    {
        plus_time_val.tv_sec += plus_time_val.tv_usec / SEC_PER_USEC;
        plus_time_val.tv_usec = plus_time_val.tv_usec % SEC_PER_USEC;
    }

    return plus_time_val;

}

//������ʱ��������,û�������ӵ�������,��������>0����ֵ
//safe == true��֤����ֵ>=0,
const  timeval ZCE_OS::timeval_sub(const timeval &left, const  timeval &right, bool safe)
{
    const uint32_t SEC_PER_USEC = 1000000;

    int64_t left_usec_val = left.tv_sec * SEC_PER_USEC + left.tv_usec;
    int64_t right_usec_val = right.tv_sec * SEC_PER_USEC + right.tv_usec;

    //��64λ��Ϊ��׼ȥ��
    int64_t minus_usec_val = left_usec_val - right_usec_val;

    timeval minus_time_val;

    // >0 ���߱�ʶ��������͵�����0
    if (minus_usec_val >= 0 || (minus_usec_val < 0 && safe == false))
    {
        minus_time_val.tv_sec = static_cast<long>( minus_usec_val / SEC_PER_USEC);
        minus_time_val.tv_usec = static_cast<long>(  minus_usec_val % SEC_PER_USEC);
    }
    else
    {
        minus_time_val.tv_sec = 0;
        minus_time_val.tv_usec = 0;
    }

    return minus_time_val;
}

//������ʱ��������,���С��0������0
const timeval timeval_safe_sub(const timeval &left, const timeval &right);

//������TIMEVALUE�Ƿ���ʣ���ʱ��
void ZCE_OS::timeval_adjust(timeval &tv)
{
    const uint32_t SEC_PER_USEC = 1000000;
    int64_t tv_usec_val = tv.tv_sec * SEC_PER_USEC + tv.tv_usec;

    tv.tv_sec = static_cast<long>( tv_usec_val / SEC_PER_USEC );
    tv.tv_usec = static_cast<long>( tv_usec_val % SEC_PER_USEC);
}

//������TIMEVALUE�Ƿ���ʣ���ʱ��
bool ZCE_OS::timeval_havetime(const timeval &tv)
{
    const uint32_t SEC_PER_USEC = 1000000;
    int64_t tv_usec_val = tv.tv_sec * SEC_PER_USEC + tv.tv_usec;

    if (tv_usec_val > 0)
    {
        return true;
    }

    return false;
}

//��ֻtimeval����ṹ
const timeval ZCE_OS::make_timeval(time_t sec, time_t usec)
{
    timeval to_timeval;
#if defined (ZCE_OS_WINDOWS)
    to_timeval.tv_sec = static_cast<long>( sec);
    to_timeval.tv_usec = static_cast<long>( usec);
#elif defined (ZCE_OS_LINUX)
    to_timeval.tv_sec = sec;
    to_timeval.tv_usec =  usec ;
#endif

    return to_timeval;
}

//ת���õ�timeval����ṹ
const timeval ZCE_OS::make_timeval(std::clock_t clock_value)
{
    const uint32_t SEC_PER_USEC = 1000000;

    timeval to_timeval;

    to_timeval.tv_sec = clock_value /  CLOCKS_PER_SEC;
    clock_t remain_val = clock_value %  CLOCKS_PER_SEC;

    //
#if defined (ZCE_OS_WINDOWS)
    to_timeval.tv_usec = static_cast<long>( static_cast<double>(remain_val  * SEC_PER_USEC / CLOCKS_PER_SEC)) ;
#elif defined (ZCE_OS_LINUX)
    to_timeval.tv_usec = static_cast<suseconds_t >( static_cast<double>(remain_val  * SEC_PER_USEC / CLOCKS_PER_SEC)) ;
#endif

    return to_timeval;
}

//ת���õ�timeval����ṹ
const timeval ZCE_OS::make_timeval(const ::timespec *timespec_val)
{
    //ÿ�����Լ�������δ��붼���ɻ�ð��죬ʵ����û�д����ðɣ�д��ע�Ͱѣ�
    //NSEC ���� 10-9s
    //USEC ΢�� 10-6s
    const uint32_t USEC_PER_NSEC = 1000;
    timeval to_timeval;

    // Windowsƽ̨��tv_sec�������long
#if defined (ZCE_OS_WINDOWS)
    to_timeval.tv_sec = static_cast<long>( timespec_val->tv_sec);
#elif defined (ZCE_OS_LINUX)
    to_timeval.tv_sec =  timespec_val->tv_sec;
#endif

    to_timeval.tv_usec = timespec_val->tv_nsec / USEC_PER_NSEC;
    return to_timeval;
}

#if defined (ZCE_OS_WINDOWS)

//ת��FILETIME��timeval
const timeval ZCE_OS::make_timeval(const FILETIME *file_time)
{
    timeval to_timeval;

    ULARGE_INTEGER ui;
    ui.LowPart = file_time->dwLowDateTime;
    ui.HighPart = file_time->dwHighDateTime;

    //The FILETIME structure is a 64-bit value representing the number of
    //100-nanosecond intervals since January 1, 1601.

    //�õ�time_t����
    to_timeval.tv_sec = static_cast<long>((ui.QuadPart - 116444736000000000) / 10000000);
    //�õ�΢�벿�֣�FILETIME��ŵ���100-nanosecond
    to_timeval.tv_usec = static_cast<long>(((ui.QuadPart - 116444736000000000) % 10000000) / 10);

    return to_timeval;
}

//ת��SYSTEMTIME��timeval
const timeval ZCE_OS::make_timeval(const SYSTEMTIME *system_time)
{
    FILETIME ft;
    ::SystemTimeToFileTime(system_time, &ft);
    return make_timeval(&ft);
}

//ת��FILETIME��timeval,����ǰ�FILETIME����һ��ʱ���������е�
const timeval ZCE_OS::make_timeval2(const FILETIME *file_time)
{
    timeval to_timeval;

    ULARGE_INTEGER ui;
    ui.LowPart = file_time->dwLowDateTime;
    ui.HighPart = file_time->dwHighDateTime;

    //FILETIME�ĵ�λ��100-nanosecond
    to_timeval.tv_sec = static_cast<long>((ui.QuadPart) / 10000000);
    to_timeval.tv_usec = static_cast<long>(((ui.QuadPart) % 10000000) / 10);

    return to_timeval;
}

#endif

//----------------------------------------------------------------------------------------------------
//ת���õ�timeval����ṹ
const ::timespec ZCE_OS::make_timespec(const ::timeval *timeval_val)
{
    //ÿ�����Լ�������δ��붼���ɻ�ð��죬ʵ����û�д����ðɣ�д��ע�Ͱѣ�
    //NSEC ���� 10-9s
    //USEC ΢�� 10-6s
    const uint32_t USEC_PER_NSEC = 1000;
    ::timespec to_timespec;

    to_timespec.tv_sec =  timeval_val->tv_sec;
    to_timespec.tv_nsec = timeval_val->tv_usec * USEC_PER_NSEC;

    return to_timespec;
}

//�����ܼ��Ƕ��ٺ���
uint64_t ZCE_OS::total_milliseconds(const ::timespec &ts)
{
    //����Ĳ���������Ϊ��Ҫת�������������ڵġ�
    const uint32_t SEC_PER_MSEC = 1000;
    const uint32_t MSEC_PER_NSEC = 1000000000;
    return static_cast<uint64_t>(ts.tv_sec) * SEC_PER_MSEC + ts.tv_nsec / MSEC_PER_NSEC;
}

//----------------------------------------------------------------------------------------------------
//���ߺ���
//������ߺ���
int ZCE_OS::sleep (uint32_t seconds)
{
#if defined (ZCE_OS_WINDOWS)
    const int ONE_SECOND_IN_MSECS = 1000;
    ::Sleep (seconds * ONE_SECOND_IN_MSECS);
    return 0;
#endif //#if defined (ZCE_OS_WINDOWS)

#if defined (ZCE_OS_LINUX)
    return ::sleep (seconds);
#endif //
}

//��Ϣһ��timeval��ʱ��
int ZCE_OS::sleep (const timeval &tv)
{
    //
#if defined (ZCE_OS_WINDOWS)
    const int ONE_SECOND_IN_MSECS = 1000;
    const int ONE_MSECS_IN_USECS = 1000;
    ::Sleep (tv.tv_sec * ONE_SECOND_IN_MSECS + tv.tv_usec / ONE_MSECS_IN_USECS );
    return 0;
#endif //

#if defined (ZCE_OS_LINUX)
    const int ONE_SECOND_IN_USECS = 1000 * 1000;
    return ::usleep (tv.tv_sec * ONE_SECOND_IN_USECS + tv.tv_usec );
#endif //
}

//΢������ߺ���
int ZCE_OS::usleep (unsigned long usec)
{
#if defined (ZCE_OS_WINDOWS)
    const int ONE_MSECS_IN_USECS = 1000;
    ::Sleep (usec / ONE_MSECS_IN_USECS);
    return 0;
#endif //#if defined (ZCE_OS_WINDOWS)

#if defined (ZCE_OS_LINUX)
    return ::usleep (usec);
#endif //
}

//----------------------------------------------------------------------------------------

uint64_t ZCE_OS::rdtsc()
{

    uint64_t tsc_value = 0;

#if defined (ZCE_WIN32) && !defined (ZCE_WIN64)

    uint32_t hiword, loword;
    //#define rdtsc __asm __emit 0fh __asm __emit 031h
    //#define cpuid __asm __emit 0fh __asm __emit 0a2h
    __asm
    {
        //CPUID
        __emit 0fh
        __emit 0a2h
        //TSC
        __emit 0fh
        __emit 031h
        //��ȡedx��eax��
        mov hiword , edx
        mov loword , eax
    }
    tsc_value = (uint64_t( hiword ) << 32) + loword ;

#elif defined (ZCE_WIN64)

    int registers[4];
    __cpuid(registers, 0);
    tsc_value = __rdtsc();

#elif defined (ZCE_OS_LINUX)

    uint32_t hiword, loword;
    asm("cpuid");
    asm volatile("rdtsc" : "=a" (hiword), "=d" (loword));
    tsc_value = (uint64_t( hiword ) << 32) + loword ;
#endif

    return tsc_value ;
}