/*!
* @copyright  2004-2013  Apache License, Version 2.0 FULLSAIL
* @filename   zce_event_handle_inotify.h
* @author     Sailzeng <sailerzeng@gmail.com>
* @version
* @date       2013��9��22��
* 
* @brief      һ��������Linux�´���Inotify���¼�������࣬
*             ���Լ������Ŀ¼�ķ�ӳ�����ڼ���ļ�ϵͳ�ı仯��
*             ������Ŀ���Ǻ�Reactor����ݣ����Ҹ�����Ȼ
*             �����ϣ����ƽ̨��ZCE_INotify_Dir_Reactor Ҳ���Ǹ��õ�ѡ��
*             ��ZCE_INotify_Dir_ReactorΪ�˼��ݶ��ƽ̨���е��Ť��
* 
* @details    
* 
* @note       Kliu���ѣ�EpollҲ�������ڴ���Inotify��ʱ�䷴Ӧ����
*             �ش���������ʾ��л��
* 
*/

#ifndef ZCE_LIB_EVENT_HANDLE_INOTIFY_H_
#define ZCE_LIB_EVENT_HANDLE_INOTIFY_H_

//�˴���ֻ����LINUX��������
#if defined ZCE_OS_LINUX

#include "zce_event_handle_base.h"

class ZCE_Reactor;

/*!
@brief      INotify �¼������ľ����ֻ����Linux��ʹ�ã�����ʹ��ZCE_Select_Reactor��ZCE_Epoll_Reactor
��Ϊ��Ӧ����
������
*/
class ZCELIB_EXPORT ZCE_Event_INotify : public ZCE_Event_Handler
{


public:


    /*!
    @brief      ���캯����ͬʱ������۵ķ�Ӧ��ָ��
    @param      reactor �����صķ�Ӧ��ָ��
    */
    ZCE_Event_INotify(void);
    /*!
    @brief      ��������
    */
    virtual ~ZCE_Event_INotify();

public:

    /*!
    @brief      �򿪼�ؾ���ȣ���reactor��
    @param      reactor_base �����صķ�Ӧ��ָ��,
    @return     ����0��ʾ�ɹ�������ʧ��
    */
    int open(ZCE_Reactor *reactor_base);

    /*!
    @brief      �رռ�ؾ���ȣ�ȡ����reactor��
    @return     ����0��ʾ�ɹ�������ʧ��
    */
    int close();



    /*!
    @brief      ȡ�ض�Ӧ��ZCE_SOCKET ���
    @return     int ZCE_Event_INotify ��Ӧ�ľ����ע��LINUX�¾����ZCE_SOCKET����int
    */
    virtual int get_handle (void) const
    {
        return inotify_handle_;
    }


    /*!
    @brief      ����һ��Ҫ���м�ص��ļ�����
    @return     int           ����0��ʾ�ɹ�������-1��ʾʧ��
    @param[in]  pathname      ��ص�·��
    @param[in]  mask          ��ص�ѡ��
    @param[out] watch_handle  ���صļ�ض�Ӧ�ľ��
    */
    int add_watch(const char *pathname,
        uint32_t mask,
        ZCE_HANDLE *watch_handle);


    /*!
    @brief      ͨ���ļ�������Ƴ�һ��Ҫ��ص���Ŀ��
    @return     int          ����0��ʾ�ɹ�������-1��ʾʧ��
    @param[in]  watch_handle ���Ŀ¼���ļ����
    */
    int rm_watch(ZCE_HANDLE watch_handle);


    /*!
    @brief      ��ȡ�¼��������ú��������ڶ�ȡ���ݣ�����ʱ�䷢��ʱ������������ص���
                �����ڲ��������巢�����¼���
    @return     int ����0��ʾ�������������return -1�󣬷�Ӧ��������handle_close�������������
    */
    virtual int handle_input ();

    /*!
    @brief
    @return     int
    */
    virtual int handle_close ();

    ///��Ҫ��̳�ʹ�õ��麯�������עʲô�¼���������ʲô����
protected:

    /*!
    @brief      ��⵽���ļ���Ŀ¼�������Ļص��������������Ҫ����������Ϊ����̳����أ�
                ��Ӧ����IN_CREATE�����溯���Ĳ��������ƣ���ο�inotify_create��
    @return     int          ����0��ʾ�ɹ�������-1��ʾ
    @param[in]  watch_handle ����ļ������ע���Ǽ�صľ�������ǲ����ļ��ľ��
    @param[in]  watch_mask   ��ط�������Ϊ�����룬����ͨ�������ж����ļ�����Ŀ¼
    @param[in]  watch_path   ��ص�·��
    @param[in]  active_path  ������������Ϊ���ļ�����Ŀ¼��·��
    */
    virtual int inotify_create(int watch_handle,
        uint32_t watch_mask,
        const char *watch_path,
        const char *active_path)
    {
        ZCE_UNUSED_ARG(watch_handle);
        ZCE_UNUSED_ARG(watch_mask);
        ZCE_UNUSED_ARG(watch_path);
        ZCE_UNUSED_ARG(active_path);
        return 0;
    }

    ///��⵽��ɾ���ļ�����Ŀ¼,��Ӧ����IN_DELETE������˵���ο�@fun inotify_create
    virtual int inotify_delete(int /*watch_handle*/,
        uint32_t /*watch_mask*/,
        const char * /*watch_path*/,
        const char * /*active_path*/)
    {
        return 0;
    }

    ///��⵽���ļ����޸�,��Ӧ����IN_MODIFY������˵���ο�@fun inotify_create
    virtual int inotify_modify(int /*watch_handle*/,
        uint32_t /*watch_mask*/,
        const char * /*watch_path*/,
        const char * /*active_path*/)
    {
        return 0;
    }

    ///����ļ���ĳ��Ŀ¼�ƶ���ȥ��IN_MOVED_FROM,����˵���ο�@fun inotify_create
    virtual int inotify_moved_from(int /*watch_handle*/,
        uint32_t /*watch_mask*/,
        const char * /*watch_path*/,
        const char * /*active_path*/)
    {
        return 0;
    }

    ///����ļ��ƶ���ĳ��Ŀ¼��IN_MOVED_TO,(���Լ�����ֻ���ڼ��Ŀ¼���ƶ��Żᷢ������¼�),
    ///����˵���ο�@fun inotify_create
    virtual int inotify_moved_to(int /*watch_handle*/,
        uint32_t /*watch_mask*/,
        const char * /*watch_path*/,
        const char * /*active_path*/)
    {
        return 0;
    }

    ///�������Ŀ¼���ļ�������ʱ���ص���IN_ACCESS,����˵���ο�@fun inotify_create
    virtual int inotify_access(int /*watch_handle*/,
        uint32_t /*watch_mask*/,
        const char * /*watch_path*/,
        const char * /*active_path*/)
    {
        return 0;
    }

    ///�������Ŀ¼���ļ�����ʱ���ص���IN_OPEN,����˵���ο�@fun inotify_create
    virtual int inotify_open(int /*watch_handle*/,
        uint32_t /*watch_mask*/,
        const char * /*watch_path*/,
        const char * /*active_path*/)
    {
        return 0;
    }

    ///�������Ŀ¼���ļ����ر��¼�ʱ���ص���IN_CLOSE_WRITE,IN_CLOSE_NOWRITE,
    ///����˵���ο�@fun inotify_create
    virtual int inotify_close(int /*watch_handle*/,
        uint32_t /*watch_mask*/,
        const char * /*watch_path*/,
        const char * /*active_path*/)
    {
        return 0;
    }

    ///����Ŀ¼�����ļ�����Ŀ¼���Ա��޸��¼�ʱ���ص���IN_ATTRIB�� permissions, timestamps,
    ///extended attributes, link count (since Linux 2.6.25), UID, GID,
    ///����˵���ο�@fun inotify_create
    virtual int inotify_attrib(int /*watch_handle*/,
        uint32_t /*watch_mask*/,
        const char * /*watch_path*/,
        const char * /*active_path*/)
    {
        return 0;
    }

    ///������ص�Ŀ¼���ƶ�ʱ���ص���IN_MOVE_SELF,����˵���ο�@fun inotify_create
    virtual int inotify_move_slef(int /*watch_handle*/,
        uint32_t /*watch_mask*/,
        const char * /*watch_path*/,
        const char * /*active_path*/)
    {
        return 0;
    }

    ///������ص�Ŀ¼��ɾ��ʱ���ص���IN_DELETE_SELF,����˵���ο�@fun inotify_create
    virtual int inotify_delete_slef(int /*watch_handle*/,
        uint32_t /*watch_mask*/,
        const char * /*watch_path*/,
        const char * /*active_path*/)
    {
        return 0;
    }


protected:

    ///�����ļ���صĽڵ�
    struct EVENT_INOTIFY_NODE
    {
        EVENT_INOTIFY_NODE():
            watch_handle_(ZCE_INVALID_HANDLE),
            watch_mask_(0)
        {
            watch_path_[0] = '\0';
        }
        ~EVENT_INOTIFY_NODE()
        {
        }

        ///��صľ��
        ZCE_HANDLE              watch_handle_;
        ///���ӵ��ļ�·��
        char                    watch_path_[MAX_PATH];
        ///����������
        uint32_t                watch_mask_;

    };



protected:

    ///BUFFER�ĳ���
    static const size_t     READ_BUFFER_LEN = 16 * 1024 - 1;

protected:

    ///EINN��Event��Inotify Node����д
    typedef unordered_map<ZCE_HANDLE, EVENT_INOTIFY_NODE >  HDL_TO_EIN_MAP;
    ///��Ӧ��������Ŀ¼�ڵ���Ϣ��MAP,
    HDL_TO_EIN_MAP     watch_event_map_;

    ///inotify_init ��ʼ���õ��ľ��
    int                inotify_handle_;

    ///
    char              *read_buffer_;


};

#endif //#if defined ZCE_OS_LINUX


#endif //ZCE_LIB_EVENT_HANDLE_INOTIFY_H_
