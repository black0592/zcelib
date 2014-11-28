
#include "zce_predefine.h"
#include "zce_sqlite_process.h"

//对于SQLITE的最低版本限制
#if SQLITE_VERSION_NUMBER >= 3005000

/******************************************************************************************
SQLite3_DB_Handler SQLite3DB Handler 连接处理一个SQLite3数据库的Handler
******************************************************************************************/
SQLite3_DB_Handler::SQLite3_DB_Handler():
    sqlite3_handler_(NULL)
{
}

SQLite3_DB_Handler::~SQLite3_DB_Handler()
{
    close_database();
}

//const char* db_file ,数据库名称文件路径,接口要求UTF8编码，
//int == 0表示成功，否则失败
int SQLite3_DB_Handler::open_database(const char *db_file, bool create_db)
{
    int flags = SQLITE_OPEN_READWRITE;
    if (create_db)
    {
        flags |= SQLITE_OPEN_CREATE;
    }

    int ret = ::sqlite3_open_v2(db_file,
                                &sqlite3_handler_,
                                flags,
                                NULL);
    if (ret != SQLITE_OK )
    {
        return -1;
    }

    return 0;

}



//以只读的方式打开一个数据库
//这个特性要3.5以后的版本才可以用。
int SQLite3_DB_Handler::open_readonly_db(const char *db_file)
{

    int ret = ::sqlite3_open_v2(db_file,
                                &sqlite3_handler_,
                                SQLITE_OPEN_READONLY,
                                NULL);
    //
    if (ret != SQLITE_OK )
    {
        return -1;
    }

    return 0;

}


//关闭数据库。
void SQLite3_DB_Handler::close_database()
{
    if (sqlite3_handler_)
    {
        ::sqlite3_close_v2(sqlite3_handler_);
        sqlite3_handler_ = NULL;
    }
}

//错误语句Str
const char *SQLite3_DB_Handler::get_dbret_errstr()
{
    return ::sqlite3_errmsg(sqlite3_handler_);
}

//DB返回的错误ID
unsigned int SQLite3_DB_Handler::get_dbret_errid()
{
    return ::sqlite3_errcode(sqlite3_handler_);
}

//开始一个事务
int SQLite3_DB_Handler::begin_transaction()
{
    int ret = 0;
    char *err_msg = NULL;
    ret = ::sqlite3_exec(sqlite3_handler_,
                         "BEGIN TRANSACTION;",
                         NULL,
                         NULL,
                         &err_msg);

    if (ret == SQLITE_OK)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

//提交一个事务
int SQLite3_DB_Handler::commit_transction()
{
    int ret = 0;
    char *err_msg = NULL;
    ret = ::sqlite3_exec(sqlite3_handler_,
                         "COMMIT TRANSACTION;",
                         NULL,
                         NULL,
                         &err_msg);

    if (ret == SQLITE_OK)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

//将同步选项关闭，可以适当的提高insert的速度，但是为了安全起见，建议不要使用
int SQLite3_DB_Handler::turn_off_synch()
{
    int ret = 0;
    char *err_msg = NULL;
    ret = ::sqlite3_exec(sqlite3_handler_,
                         "PRAGMA synchronous=OFF;",
                         NULL,
                         NULL,
                         &err_msg);

    if (ret == SQLITE_OK)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

#endif //#if SQLITE_VERSION_NUMBER >= 3005000

