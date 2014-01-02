
#include "zce_predefine.h"
#include "zce_mysql_result.h"

//�����Ҫ��MYSQL�Ŀ�
#if defined MYSQL_VERSION_ID

/*********************************************************************************
class ZCE_Mysql_Result
*********************************************************************************/

//���캯��
ZCE_Mysql_Result::ZCE_Mysql_Result():
    mysql_result_(NULL),
    current_row_(NULL),
    current_field_(0),
    fields_length_(NULL),
    num_result_row_(0),
    num_result_field_(0),
    mysql_fields_(NULL)
{
}

//���캯��
ZCE_Mysql_Result::ZCE_Mysql_Result(MYSQL_RES *sqlresult):
    mysql_result_(NULL),
    current_row_(NULL),
    current_field_(0),
    fields_length_(NULL),
    num_result_row_(0),
    num_result_field_(0),
    mysql_fields_(NULL)
{
    set_mysql_result(sqlresult);
}

//��������
ZCE_Mysql_Result::~ZCE_Mysql_Result()
{
    // �ͷŽ�����ϵ��ڴ���Դ
    if (mysql_result_ != NULL)
    {
        mysql_free_result(mysql_result_);
    }
}

//����������
void ZCE_Mysql_Result::set_mysql_result(MYSQL_RES *sqlresult)
{
    ZCE_ASSERT(sqlresult);

    //����Ѿ��н����, �ͷ�ԭ�еĽ����,
    if (NULL != mysql_result_)
    {
        mysql_free_result(mysql_result_);
        mysql_result_  = NULL;
    }

    //��0��ǰ��,���Լ���ǰ�г�������ָ��
    fields_length_ = NULL;
    current_row_   = NULL;
    current_field_ = 0;

    //����Ŀ������Ŀ��0
    num_result_row_   = 0;
    num_result_field_ = 0;

    //������ָ����0
    mysql_fields_        = 0;

    mysql_result_ = sqlresult;

    //�������һ���յĽ������
    if (mysql_result_)
    {
        //�õ�����,����
        num_result_row_   = (unsigned int)  mysql_num_rows(mysql_result_);
        num_result_field_ = mysql_num_fields(mysql_result_);

        //������ָ��,��ʵ���Ƿ���һ�������ָ��,Ч��Ӧ�����б��ϵ�
        mysql_fields_      = mysql_fetch_fields(mysql_result_);
    }

    return;
}

//����Ѿ��н����, �ͷ�ԭ�еĽ����,
void ZCE_Mysql_Result::free_result()
{
    //����Ѿ��н����, �ͷ�ԭ�еĽ����,
    if (NULL != mysql_result_)
    {
        mysql_free_result(mysql_result_);
        mysql_result_  = NULL;
    }
}

//����һ��������ϵ���һ��,�ʼ��0�п�ʼ
bool ZCE_Mysql_Result::fetch_row_next()
{
    if (mysql_result_ == NULL)
    {
        return false;
    }

    //����һ��������ϵ���һ��
    current_row_ = mysql_fetch_row(mysql_result_);

    //���NEXT��Ϊ��,��������
    if (current_row_ == NULL )
    {
        return false;
    }

    //�õ����������еĳ���
    fields_length_ = mysql_fetch_lengths(mysql_result_);

    current_field_ = 0;
    return true;
}

//������row_id ��,
int ZCE_Mysql_Result::seek_row(unsigned int row_id)
{
    //���������Ϊ��,���߲���row����
    if (mysql_result_ == NULL || row_id >= num_result_row_)
    {
        ZCE_ASSERT(false);
        return MYSQL_RETURN_FAIL;
    }

    mysql_data_seek(mysql_result_, row_id);
    current_row_   = mysql_fetch_row(mysql_result_);
    fields_length_ = mysql_fetch_lengths(mysql_result_);
    current_field_ = 0;
    return MYSQL_RETURN_OK;
}

//���������ID�õ��ֶ�ֵ,data��Ϊ����ֵ
const char *ZCE_Mysql_Result::get_field_data(const char *fname) const
{
    //�����е����ֵõ�Field ID
    unsigned int fid = 0;
    int ret = get_field_id(fname, fid);

    if (ret == MYSQL_RETURN_FAIL || current_row_ == NULL)
    {
        ZCE_ASSERT(false);
        return NULL;
    }

    return current_row_[fid];
}

//�����ֶ���ID,�õ��ֶ�ֵ
int ZCE_Mysql_Result::get_field_data(const char *fname, char *pfdata) const
{
    //�����е����ֵõ�Field ID
    unsigned int fid = 0;
    int ret = get_field_id(fname, fid);

    //��������Ϊ��,����û���ҵ���ص���ID
    if ( ret == MYSQL_RETURN_FAIL || current_row_ == NULL || pfdata == NULL)
    {
        ZCE_ASSERT(false);
        return ret;
    }

    //
    memcpy(pfdata , current_row_[fid], fields_length_[fid]);
    return MYSQL_RETURN_OK;

}

//
int ZCE_Mysql_Result::get_field(const char *fname, ZCE_Mysql_Field &ffield) const
{
    //ѭ���Ƚ����е�����,Ч�ʱȽϵ���
    unsigned int fid = 0;
    int ret = get_field_id(fname, fid);

    //��������Ϊ��,����û���ҵ���ص���ID
    if ( ret == MYSQL_RETURN_FAIL || current_row_ == NULL)
    {
        ZCE_ASSERT(false);
        return MYSQL_RETURN_FAIL;
    }

    ffield.set_field(current_row_[fid], fields_length_[fid], mysql_fields_[fid].type);
    return MYSQL_RETURN_OK;
}

//�����ֶ����Ƶõ��ֶα��ṹ���������,Ч�ʽϵ�,
//����MYSQL_RETURN_FAIL ��ʾ����
int ZCE_Mysql_Result::get_field_type(const char *fname, enum_field_types &ftype) const
{

    //ѭ���Ƚ����е�����,Ч�ʱȽϵ���
    unsigned int fid = 0;
    int ret = get_field_id(fname, fid);

    //��������Ϊ��,����û���ҵ���ص���ID
    if ( ret == MYSQL_RETURN_FAIL || current_row_ == NULL)
    {
        ZCE_ASSERT(false);
        return MYSQL_RETURN_FAIL;
    }

    ftype = mysql_fields_[fid].type;
    return MYSQL_RETURN_OK;
}

//����Field Name �õ�����ֵ��ʵ�ʳ���
int ZCE_Mysql_Result::get_field_length(const char *fname, unsigned int &flength ) const
{
    //�����е����ֵõ�Field ID
    unsigned int fid = 0;
    int ret = get_field_id(fname, fid);

    //��������Ϊ��,����û���ҵ���ص���ID
    if ( ret == MYSQL_RETURN_FAIL || current_row_ == NULL)
    {
        ZCE_ASSERT(false);
        return MYSQL_RETURN_FAIL;
    }

    flength = fields_length_[fid];
    return MYSQL_RETURN_OK;
}

//�����ֶ�˳��ID,�õ����ṹ������ֶγ���
int ZCE_Mysql_Result::get_define_field_size(unsigned int fieldid, unsigned int &flength) const
{

    //���������Ϊ��,���߲���fieldid����
    if ( mysql_result_ == NULL && fieldid >= num_result_field_)
    {
        ZCE_ASSERT(false);
        return MYSQL_RETURN_FAIL;
    }

    flength = mysql_fields_[fieldid].length;
    return MYSQL_RETURN_OK;
}

//�����ֶ����Ƶõ����ṹ������ֶγ���,Ч�ʽϵ�
int ZCE_Mysql_Result::get_define_field_size(const char *fname, unsigned int &fdefsz) const
{

    //ѭ���Ƚ����е�����,Ч�ʱȽϵ���
    unsigned int fid = 0;
    int ret = get_field_id(fname, fid);

    //��������Ϊ��
    if ( ret == MYSQL_RETURN_FAIL || mysql_result_ != NULL)
    {
        ZCE_ASSERT(false);
        return MYSQL_RETURN_FAIL;
    }

    fdefsz = mysql_fields_[fid].length;
    return MYSQL_RETURN_OK;
}

//>>�����Ǹ�C++�İ�����׼���ģ��������ڷ����������޷�����(��������),���������쳣
//���ڽ�������еĵ�ǰ�У���ǰ��������������������ֵ��+1
ZCE_Mysql_Result &ZCE_Mysql_Result::operator >> (char &val)
{
    val = 0;
    //��������Ϊ��
    sscanf(current_row_[current_field_], "%c", &val);
    current_field_  = (current_field_ < num_result_field_ - 1) ? current_field_ + 1 : current_field_;
    return *this;
}

ZCE_Mysql_Result &ZCE_Mysql_Result::operator >> (short &val)
{
    val = 0;
    //��������Ϊ��
    sscanf(current_row_[current_field_], "%hd", &val);
    current_field_  = (current_field_ < num_result_field_ - 1) ? current_field_ + 1 : current_field_;
    return *this;
}

ZCE_Mysql_Result &ZCE_Mysql_Result::operator >> (int &val)
{
    val = 0;
    //��������Ϊ��
    sscanf(current_row_[current_field_], "%d", &val);
    current_field_  = (current_field_ < num_result_field_ - 1) ? current_field_ + 1 : current_field_;
    return *this;
}

ZCE_Mysql_Result &ZCE_Mysql_Result::operator >> (long &val)
{
    val = 0;
    //��������Ϊ��
    sscanf(current_row_[current_field_], "%ld", &val);
    ++current_field_;
    return *this;
}

ZCE_Mysql_Result &ZCE_Mysql_Result::operator >> (long long &val)
{
    val = 0;
    //ת���Լ����
    sscanf(current_row_[current_field_], "%lld", &val);
    ++current_field_;
    return *this;
}

ZCE_Mysql_Result &ZCE_Mysql_Result::operator >> (unsigned char &val)
{
    val = 0;
    //��������Ϊ��
    sscanf(current_row_[current_field_], "%c", &val);
    current_field_  = (current_field_ < num_result_field_ - 1) ? current_field_ + 1 : current_field_;
    return *this;
}

ZCE_Mysql_Result &ZCE_Mysql_Result::operator >> (unsigned short &val)
{
    val = 0;
    //ת���Լ����
    sscanf(current_row_[current_field_], "%hu", &val);
    ++current_field_;
    return *this;
}

ZCE_Mysql_Result &ZCE_Mysql_Result::operator >> (unsigned long &val)
{
    val = 0;
    //ת���Լ����
    sscanf(current_row_[current_field_], "%lu", &val);
    current_field_  = (current_field_ < num_result_field_ - 1) ? current_field_ + 1 : current_field_;
    return *this;
}

ZCE_Mysql_Result &ZCE_Mysql_Result::operator >> (unsigned int &val)
{
    val = 0;
    //ת���Լ����
    sscanf(current_row_[current_field_], "%u", &val);
    ++current_field_;
    return *this;
}

ZCE_Mysql_Result &ZCE_Mysql_Result::operator >> (unsigned long long &val)
{
    val = 0;
    //ת���Լ����
    sscanf(current_row_[current_field_], "%llu", &val);
    ++current_field_;
    return *this;
}

ZCE_Mysql_Result &ZCE_Mysql_Result::operator >> (float &val)
{
    val = 0.0;
    //ת���Լ����
    sscanf(current_row_[current_field_], "%f", &val);
    ++current_field_;
    return *this;
}

ZCE_Mysql_Result &ZCE_Mysql_Result::operator >> (double &val)
{
    val = 0.0;
    //ת���Լ����
    sscanf(current_row_[current_field_], "%lf", &val);
    ++current_field_;
    return *this;
}

//����char *,Ĭ�ϵ�����һ���ַ���,����ĩβ����һ��'\0'
ZCE_Mysql_Result &ZCE_Mysql_Result::operator >> (char *val)
{
    ZCE_ASSERT((NULL != val) &&  (NULL != current_row_[current_field_] ));

    //���Ȳ�������������
    memcpy(val , current_row_[current_field_], fields_length_[current_field_]);
    val[fields_length_[current_field_]] = '\0';

    ++current_field_;
    return *this;
}

//����char *,Ĭ�ϵ�����һ���ַ���,����ĩβ����һ��'\0'
//���ǹ�����unsigned char *��һЩ�ر�����������������,��BINARYȥ������
ZCE_Mysql_Result &ZCE_Mysql_Result::operator >> (unsigned char *val)
{
    ZCE_ASSERT((NULL != val) &&  (NULL != current_row_[current_field_] ));

    //���Ȳ�������������
    memcpy(val , current_row_[current_field_], fields_length_[current_field_] );
    val[fields_length_[current_field_]] = '\0';

    ++current_field_;
    return *this;
}

//�����Ƶ�����Ҫ�ر���һ��,�ַ������ر�+1��,�����������ݲ�Ҫ��������
ZCE_Mysql_Result &ZCE_Mysql_Result::operator >> (ZCE_Mysql_Result::BINARY *val)
{
    ZCE_ASSERT((NULL != val) &&  (NULL != current_row_[current_field_] ));

    //���Ȳ�������������
    memcpy(val , current_row_[current_field_], fields_length_[current_field_]);

    ++current_field_;
    return *this;
}

ZCE_Mysql_Result &ZCE_Mysql_Result::operator >> (std::string &val)
{
    if (current_row_[current_field_])
    {
        val.assign(current_row_[current_field_], fields_length_[current_field_]) ;
    }
    else
    {
        val = "";
    }

    ++current_field_;
    return *this;
}

//�����Ҫ��MYSQL�Ŀ�
#endif //#if defined MYSQL_VERSION_ID
