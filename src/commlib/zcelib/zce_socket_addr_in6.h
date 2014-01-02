#ifndef ZCE_LIB_SOCKET_ADDR_IN6_
#define ZCE_LIB_SOCKET_ADDR_IN6_

//IPV6��һ��

//IPv6�������ṹ
class ZCELIB_EXPORT ZCE_Sockaddr_In6 : public ZCE_Sockaddr
{
public:

    //Ĭ�Ϲ��캯��
    ZCE_Sockaddr_In6 (void);

    //����sockaddr_in���죬
    ZCE_Sockaddr_In6 (const sockaddr_in6 *addr);

    //���ݶ˿ںţ���IP��ַ�ַ�������,ipv6_addr_strӦ����<40���ֽڵĳ���
    ZCE_Sockaddr_In6 (const char *ipv6_addr_str,
                      uint16_t port_number);

    //���ݶ˿ںţ���IP��ַ��Ϣ����
    ZCE_Sockaddr_In6 (uint16_t port_number,
                      const char ipv6_addr_val[16]);

    //�������죬һ��Ҫд�������Ļ���ָ����ָ���Լ���һ����ַ�ģ�
    ZCE_Sockaddr_In6 (const ZCE_Sockaddr_In6 &others);

    ~ZCE_Sockaddr_In6();

public:
    //���õ�ַ��Ϣ
    virtual  void set_sockaddr (sockaddr *addr, socklen_t len);

    //���ݵ�ַ���֣��˿ں�����
    int set(const char ip_addr_str[],
            uint16_t port_number);
    //���ݵ�ַIP���˿ں�����
    int set(uint16_t port_number,
            const char ipv6_addr_val[16]);

    //���ö˿ںã�
    inline void set_port_number (uint16_t);
    //ȡ�ö˿ں�
    inline uint16_t get_port_number (void) const;

    //���ɳ���ķǰ�ȫ����
    inline const char *get_host_name (void) const;

    //ȡ����:ð�ŵ�IP��ַ��ϢSTRING
    inline const char *get_host_addr (char *addr, int addr_size) const;
    //���ɳ���ķǰ�ȫ����
    inline const char *get_host_addr (void) const;

    //ȡ���ԣ����ŵ�IP��ַ#�˿ں�STRING
    inline const char *get_host_addr_port(char *addr, int addr_size) const;

    //ȡ��IP��ַ,��Ҫ��֤ipv6_addr_val��16���ֽ�
    const char *get_ip_address (char *ipv6_addr_val) const;

    //�Ƚ�������ַ�Ƿ����
    bool operator == (const ZCE_Sockaddr_In6 &others) const;
    //�Ƚ�������ַ�Ƿ����
    bool operator != (const ZCE_Sockaddr_In6 &others) const;

    //���IP��ַ�Ƿ����,���Ӷ˿�
    bool is_ip_equal (const ZCE_Sockaddr_In6 &others) const;

    //���IPV6�ĵ�ַ�Ƿ���IPV4�ĵ�ַӳ���
    bool is_v4mapped() const;
    //��һ��IPV4�ĵ�ַ�õ���Ӧӳ���IPV6�ĵ�ַ��
    int map_from_inaddr(const ZCE_Sockaddr_In &from);
    //������IPV6�ĵ�ַ��IPV4ӳ������ģ����仹ԭΪIPV4�ĵ�ַ
    int mapped_to_inaddr(ZCE_Sockaddr_In &to) const;

    //DNS��غ�����
    //ȡ��IP��ַ��ص�������Ϣ,���õ���getnameinfo
    int get_name_info(char *host_name, size_t name_len) const;
    //ȡ��������ص�IP��ַ��Ϣ�����õ���getaddrinfo
    int get_addr_info(const char *hostname, uint16_t service_port = 0);

    //���ֲ�������ת���������������ʹ�ã���ZCE_Sockaddr_In6����Ϊ��sockaddr_in6����һ��
    //����sockaddr_in6
    operator sockaddr_in6 () const;
    //�����ڲ�const sockaddr_in6��ָ�룬�������Ա��޸ģ�
    operator const sockaddr_in6 *() const;
    //�����ڲ� sockaddr_in6��ָ�룬�����Ա��޸ģ�
    operator sockaddr_in6 *();

protected:

    //IPV6�ĵ�ַ
    sockaddr_in6           in6_addr_;
};

//ȡ��IP��ַ����Ҫ��֤ipv6_addr_val�ĳ�����16���ֽ�
inline const char *ZCE_Sockaddr_In6::get_ip_address (char *ipv6_addr_val) const
{
    memcpy(ipv6_addr_val, &(in6_addr_.sin6_addr), sizeof(in6_addr));
    return ipv6_addr_val;
}

//���ö˿ںã�
inline void ZCE_Sockaddr_In6::set_port_number (uint16_t port_number)
{
    in6_addr_.sin6_port = ntohs(port_number);
}
//ȡ�ö˿ں�
inline uint16_t ZCE_Sockaddr_In6::get_port_number (void) const
{
    return ntohs(in6_addr_.sin6_port);
}

//ȡ����.���ŵ�IP��ַ��ϢSTRING
inline const char *ZCE_Sockaddr_In6::get_host_addr (char *addr_buf, int addr_size) const
{
    return ZCE_OS::socketaddr_ntop(reinterpret_cast<const sockaddr *>(&in6_addr_),
                                   addr_buf,
                                   addr_size);
}

//���ɳ���ķǰ�ȫ����
inline const char *ZCE_Sockaddr_In6::get_host_addr (void) const
{
    const size_t BUF_LEN = 64;
    static char in4_buf[BUF_LEN + 1];
    in4_buf[BUF_LEN] = '\0';

    return ZCE_OS::socketaddr_ntop(reinterpret_cast<const sockaddr *>(&in6_addr_),
                                   in4_buf,
                                   BUF_LEN);
}

//ȡ����.���ŵ�IP��ַ#�˿ں�STRING
inline const char *ZCE_Sockaddr_In6::get_host_addr_port(char *addr_buf, int addr_size) const
{
    return ZCE_OS::socketaddr_ntop_ex(reinterpret_cast<const sockaddr *>(&in6_addr_),
                                      addr_buf,
                                      addr_size);
}

#endif  //ZCE_LIB_SOCKET_ADDR_IN6_