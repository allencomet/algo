#include "../util/util.h"

#include <mongoc.h>
#include <bson.h>
#include <bcon.h>

#if __cplusplus >= 201103L

#include <functional>

#else

#include <boost/function.hpp>

#endif

/*	通过配置文件启动MongoDB

mongodb的参数说明：

mongodb的参数说明：
--dbpath 数据库路径(数据文件)
--logpath 日志文件路径
--master 指定为主机器
--slave 指定为从机器
--source 指定主机器的IP地址
--pologSize 指定日志文件大小不超过64M.因为resync是非常操作量大且耗时，最好通过设置一个足够大的oplogSize来避免resync(默认的 oplog大小是空闲磁盘大小的5%)。
--logappend 日志文件末尾添加
--port 启用端口号
--fork 在后台运行
--only 指定只复制哪一个数据库
--slavedelay 指从复制检测的时间间隔
--auth 是否需要验证权限登录(用户名和密码)
--bind_ip 默认为绑定在127.0.0.1，默认只有本机可以连接，此时，需要将bind_ip配置为0.0.0.0，表示接受任何IP的连接

注：mongodb配置文件里面的参数很多，不懂的请查看 --help,如果定制特定的需求，请参考官方文档



1、新建自定义MongoDB相关数据存放目录
----------------------------
sudo mkdir -p /data/db
sudo mkdir -p /data/db/log
sudo touch /data/db/log/mongodb.log
sudo touch  /etc/mongodb.conf

2、编辑配置文件
----------------------------
vim mongodb.conf

dbpath=/data/db					#指定数据库路径
logpath=/data/log/mongodb.log	#指定log路径
logappend=true					#在log末尾持续添加
bind_ip=0.0.0.0					#接受任何IP的连接（注：如果未设置则只允许本地连接，无法远程连接）
port=27017						#端口号
fork=true						#创建子进程

3、通过配置文件启动
----------------------------
./mongod -f /etc/mongodb.conf

如果输出内容类似如下，那么就启动成功了
about to fork child process, waiting until server is ready for connections.
forked process: 9392
child process started successfully, parent exiting
*/

namespace test {

DEF_test(conn_mongo) {
	const char *uri_str = "mongodb://localhost:27017";
	const char *uri_with_appname = "mongodb://localhost:27017/?appname=proxy";
	const char *appname = "proxy";

	//初始化MongoDB
	mongoc_init();

	//获取一个客户实例方法一
	mongoc_client_t *client = mongoc_client_new(uri_str);
	mongoc_client_set_appname(client, appname);

	//获取一个客户实例方法二
	//mongoc_client_t *client = mongoc_client_new(uri_with_appname);

	//获取数据库句柄
	mongoc_database_t *db = mongoc_client_get_database(client, "db_name");
	//获取集合句柄
	mongoc_collection_t *col = mongoc_client_get_collection(client, "db_name", "col_name");

	//执行一些命令行的操作
	bson_t *cmd = BCON_NEW("ping", BCON_INT32(1));
	bson_t reply;
	bson_error_t error;
	bool ret = mongoc_client_command_simple(client, "admin", cmd, NULL, &reply, &error);

	if (!ret) {
		CERR << "Error has been occurred: " << error.message;
		return;
	}

	char *result = bson_as_json(&reply, NULL);
	COUT << result;

	bson_t *insert = BCON_NEW("hello", BCON_UTF8("world"));

	ret = mongoc_collection_insert_one(col, insert, NULL, NULL, &error);

	if (!ret) CERR << error.message;

	//释放一些文档集合资源
	bson_destroy(insert);
	bson_destroy(&reply);
	bson_destroy(cmd);
	bson_free(result);

	//清理一些句柄资源
	mongoc_collection_destroy(col);
	mongoc_database_destroy(db);
	mongoc_client_destroy(client);
	mongoc_cleanup();
}

/*
{
born : ISODate("1906-12-09"),
died : ISODate("1992-01-01"),
name : {
first : "Grace",
last : "Hopper"
},
languages : [ "MATH-MATIC", "FLOW-MATIC", "COBOL" ],
degrees: [ { degree: "BA", school: "Vassar" }, { degree: "PhD", school: "Yale" } ]
}
*/
DEF_test(create_bson) {
	bson_t     *document = bson_new();

	struct tm   born = { 0 };
	born.tm_year = 6;  /* years are 1900-based */
	born.tm_mon = 11;  /* months are 0-based */
	born.tm_mday = 9;
	bson_append_date_time(document, "born", -1, mktime(&born) * 1000);


	struct tm   died = { 0 };
	died.tm_year = 92;
	died.tm_mon = 0;
	died.tm_mday = 1;
	//用宏定义默认第三个参数为-1
	BSON_APPEND_DATE_TIME(document, "died", mktime(&died) * 1000);

	/*  创建子文档
		name : {
			first : "Grace",
			last : "Hopper"
			}
	*/
	bson_t      child;
	BSON_APPEND_DOCUMENT_BEGIN(document, "name", &child);
	BSON_APPEND_UTF8(&child, "first", "allen");
	BSON_APPEND_UTF8(&child, "last", "comet");
	bson_append_document_end(document, &child);


	std::vector<std::string> lang_names{ "MATH-MATIC", "FLOW-MATIC", "COBOL" };
	char buf[16] = { 0 };
	const char *key;
	size_t keylen;


	/*  创建json数组
		languages: [
			"MATH-MATIC",
			"FLOW-MATIC",
			"COBOL"
		]
	*/
	BSON_APPEND_ARRAY_BEGIN(document, "languages", &child);
	for (size_t i = 0; i < lang_names.size(); ++i) {
		keylen = bson_uint32_to_string(i, &key, buf, sizeof(buf));
		bson_append_utf8(&child, key, (int)keylen, lang_names[i].c_str(), -1);
	}
	bson_append_array_end(document, &child);

	/*  创建json数组
		degrees: [ {
			degree: "BA", school: "Vassar"
			},
			...
		]
	*/
	bson_t      child2;
	std::vector<std::string> schools{ "Vassar", "Yale" };
	std::vector<std::string> degrees{ "BA", "PhD" };
	BSON_APPEND_ARRAY_BEGIN(document, "degrees", &child);
	for (size_t i = 0; i < degrees.size(); ++i) {
		keylen = bson_uint32_to_string(i, &key, buf, sizeof(buf));
		bson_append_document_begin(&child, key, (int)keylen, &child2);
		BSON_APPEND_UTF8(&child2, "degree", degrees[i].c_str());
		BSON_APPEND_UTF8(&child2, "school", schools[i].c_str());
		bson_append_document_end(&child, &child2);
	}
	bson_append_array_end(document, &child);

	char *result = bson_as_canonical_extended_json(document, NULL);
	COUT << result;
	bson_free(result);

	/*
	* Clean up allocated bson documents.
	*/
	bson_destroy(document);
}

//JSON字符串可以通过bson_new_from_json创建单个BSON文档
//若要从一个JSON文档序列初始化BSON，使用bson_json_reader_t
DEF_test(create_bcon) {
	struct tm born = { 0 };
	struct tm died = { 0 };

	born.tm_year = 6;
	born.tm_mon = 11;
	born.tm_mday = 9;

	died.tm_year = 92;
	died.tm_mon = 0;
	died.tm_mday = 1;

	bson_t   *document = BCON_NEW(
		"born", BCON_DATE_TIME(mktime(&born) * 1000),
		"died", BCON_DATE_TIME(mktime(&died) * 1000),
		"name", "{",
		"first", BCON_UTF8("Grace"),
		"last", BCON_UTF8("Hopper"),
		"}",
		"languages", "[",
		BCON_UTF8("MATH-MATIC"),
		BCON_UTF8("FLOW-MATIC"),
		BCON_UTF8("COBOL"),
		"]",
		"degrees", "[",
		"{", "degree", BCON_UTF8("BA"), "school", BCON_UTF8("Vassar"), "}",
		"{", "degree", BCON_UTF8("PhD"), "school", BCON_UTF8("Yale"), "}",
		"]");

	char *result = bson_as_canonical_extended_json(document, NULL);
	COUT << result;
	bson_free(result);

	/*
	* Clean up allocated bson documents.
	*/
	bson_destroy(document);
}


DEF_test(insert_doc) {
	//使用前初始化
	mongoc_init();

	//直接在连接初始化时这只appname
	std::string uri("mongodb://localhost:27017/?appname=insert-example");
	mongoc_client_t *client = mongoc_client_new(uri.c_str());

	//获取集合
	mongoc_collection_t *coll = mongoc_client_get_collection(client, "db_name", "coll_name");

	bson_t *doc = bson_new();
	bson_oid_t oid;
	bson_oid_init(&oid, NULL);
	BSON_APPEND_OID(doc, "_id", &oid);
	BSON_APPEND_UTF8(doc, "name", "allen");
	BSON_APPEND_UTF8(doc, "gender", "boy");

	bson_error_t error;
	bool ret = mongoc_collection_insert_one(coll, doc, NULL, NULL, &error);
	if (!ret) CERR << error.message;

	struct tm born = { 0 };
	struct tm died = { 0 };

	born.tm_year = 6;
	born.tm_mon = 11;
	born.tm_mday = 9;

	died.tm_year = 92;
	died.tm_mon = 0;
	died.tm_mday = 1;

	bson_t   *doc2 = BCON_NEW(
		"born", BCON_DATE_TIME(mktime(&born) * 1000),
		"died", BCON_DATE_TIME(mktime(&died) * 1000),
		"name", "{",
		"first", BCON_UTF8("Grace"),
		"last", BCON_UTF8("Hopper"),
		"}",
		"languages", "[",
		BCON_UTF8("MATH-MATIC"),
		BCON_UTF8("FLOW-MATIC"),
		BCON_UTF8("COBOL"),
		"]",
		"degrees", "[",
		"{", "degree", BCON_UTF8("BA"), "school", BCON_UTF8("Vassar"), "}",
		"{", "degree", BCON_UTF8("PhD"), "school", BCON_UTF8("Yale"), "}",
		"]");
	ret = mongoc_collection_insert_one(coll, doc2, NULL, NULL, &error);
	if (!ret) CERR << error.message;


	bson_destroy(doc);
	bson_destroy(doc2);
	mongoc_collection_destroy(coll);
	mongoc_client_destroy(client);
	mongoc_cleanup();
}

DEF_test(find_doc) {
	//使用前初始化
	mongoc_init();

	std::string uri("mongodb://localhost:27017/?appname=find-example");
	mongoc_client_t *client = mongoc_client_new(uri.c_str());

	//获取集合
	mongoc_collection_t *coll = mongoc_client_get_collection(client, "db_name", "coll_name");

	//查找条件
	bson_t *query = bson_new();
	BSON_APPEND_UTF8(query, "name", "allen");

	//查询
	mongoc_cursor_t *cursor = mongoc_collection_find_with_opts(coll, query, NULL, NULL);

	//遍历查询结果集
	char *result = NULL;
	const bson_t *doc = NULL;
	while (mongoc_cursor_next(cursor, &doc)) {
		result = bson_as_canonical_extended_json(doc, NULL);
		COUT << result;
		bson_free(result);
	}

	bson_destroy(query);
	mongoc_cursor_destroy(cursor);
	mongoc_collection_destroy(coll);
	mongoc_client_destroy(client);
	mongoc_cleanup();
}

DEF_test(find_specific) {
	//使用前初始化
	DEF_case(initizlize) {
		mongoc_init();
	}

	mongoc_client_t *client;
	mongoc_collection_t *coll;

	DEF_case(create_client) {
		std::string uri("mongodb://localhost:27017/");
		client = mongoc_client_new(uri.c_str());
		EXPECT(NULL != client);
	}

	DEF_case(get_collection) {
		coll = mongoc_client_get_collection(client, "db_name", "coll_name");
		EXPECT(NULL != coll);
	}

	DEF_case(find) {
		bson_t *query = bson_new();
		EXPECT(NULL != query);
		BSON_APPEND_UTF8(query, "hello", "world");

		mongoc_cursor_t *cursor = mongoc_collection_find_with_opts(coll, query, NULL, NULL);
		//mongoc_cursor_t *cursor = mongoc_collection_find(coll, MONGOC_QUERY_NONE, 0, 0, 0, query, NULL, NULL);
		char *result = NULL;
		const bson_t *qry_doc;
		int index = 0;
		while (mongoc_cursor_next(cursor, &qry_doc)) {
			result = bson_as_json(qry_doc, NULL);
			COUT << "[" << ++index << "]: " << result;
			bson_free(result);
		}

		bson_destroy(query); query = NULL;
		mongoc_cursor_destroy(cursor); cursor = NULL;
	}

	DEF_case(clean_up) {
		mongoc_collection_destroy(coll); coll = NULL;
		mongoc_client_destroy(client); client = NULL;
		mongoc_cleanup();
	}
}

DEF_test(delete_doc) {
	//使用前初始化
	DEF_case(initizlize) {
		mongoc_init();
	}

	mongoc_client_t *client = NULL;
	mongoc_collection_t *coll = NULL;
	bson_oid_t oid;
	

	//创建一个客户实例
	DEF_case(create_client) {
		COUT << "创建客户实例...";
		std::string uri("mongodb://localhost:27017/?appname=delete-example");
		client = mongoc_client_new(uri.c_str());
		EXPECT(NULL != client);
	}
	
	//获取集合对象
	DEF_case(get_collection) {
		COUT << "获取集合对象...";
		coll = mongoc_client_get_collection(client, "db_name", "coll_name");
		EXPECT(NULL != coll);
	}
	
	//删除所有文档
	DEF_case(del_all_doc) {
		COUT << "删除集合内所有文档...";
		bson_t *doc = bson_new();
		EXPECT(NULL != doc);
		bson_error_t error;
		bool ret = mongoc_collection_delete_many(coll, doc, NULL, NULL, &error);
		EXPECT_EQ(true, ret);
		if (!ret) COUT << "Delete failed: " << error.message;
		bson_destroy(doc);
		doc = NULL;
	}
	
	//插入一个文档到集合
	DEF_case(insert_one_doc) {
		COUT << "插入一个集合到文档...";
		bson_t *doc = bson_new();
		EXPECT(NULL != doc);
		bson_oid_init(&oid, NULL);
		BSON_APPEND_OID(doc, "_id", &oid);
		BSON_APPEND_UTF8(doc, "name", "allen");
		BSON_APPEND_UTF8(doc, "gender", "comet");

		bson_error_t error;
		bool ret = mongoc_collection_insert_one(coll, doc, NULL, NULL, &error);
		EXPECT_EQ(true, ret);
		if (!ret) COUT << "Insert failed: " << error.message;
		bson_destroy(doc);
		doc = NULL;
	}
	
	//查询集合中的所有文档
	DEF_case(find_all_doc) {
		COUT << "查询集合中所有文档...";
		bson_t *query = bson_new();
		EXPECT(NULL != query);
		mongoc_cursor_t *cursor = mongoc_collection_find_with_opts(coll, query, NULL, NULL);

		//遍历查询结果集
		char *result = NULL;
		const bson_t *qry_doc = NULL;
		int index = 0;
		while (mongoc_cursor_next(cursor, &qry_doc)) {
			result = bson_as_canonical_extended_json(qry_doc, NULL);
			COUT << "[" << ++index << "]: " << result;
			bson_free(result);
		}
		mongoc_cursor_destroy(cursor);
		cursor = NULL;
		result = NULL;
	}

	//删除文档
	DEF_case(del_one_doc) {
		COUT << "删除刚才插入的到集合的文档...";
		bson_t *doc = bson_new();
		EXPECT(NULL != doc);
		bson_error_t error;
		BSON_APPEND_OID(doc, "_id", &oid);
		bool ret = mongoc_collection_delete_one(coll, doc, NULL, NULL, &error);
		EXPECT_EQ(true, ret);
		if (!ret) COUT << "Delete failed: " << error.message;
		bson_destroy(doc);
		doc = NULL;
	}
	
	//再次查询删除后的结果
	DEF_case(find_all_doc) {
		COUT << "删除后再次查询集合内所有文档...";
		bson_t *query = bson_new();
		EXPECT(NULL != query);
		mongoc_cursor_t *cursor = mongoc_collection_find_with_opts(coll, query, NULL, NULL);

		//遍历查询结果集
		char *result = NULL;
		const bson_t *qry_doc = NULL;
		int index = 0;
		while (mongoc_cursor_next(cursor, &qry_doc)) {
			result = bson_as_canonical_extended_json(qry_doc, NULL);
			COUT << "[" << ++index << "]: " << result;
			bson_free(result);
		}
		mongoc_cursor_destroy(cursor);
		result = NULL;
	}
	
	DEF_case(clean_up) {
		mongoc_collection_destroy(coll); coll = NULL;
		mongoc_client_destroy(client); client = NULL;
		mongoc_cleanup();
	}
	
	COUT << "操作完毕，资源清理完成...";
}

DEF_test(update_doc) {
	//使用前初始化
	DEF_case(initizlize) {
		mongoc_init();
	}

	mongoc_client_t *client;
	mongoc_collection_t *coll;
	bson_oid_t oid;

	DEF_case(create_client) {
		std::string uri("mongodb://localhost:27017/?appname=update-example");
		client = mongoc_client_new(uri.c_str());
		EXPECT(NULL != client);
	}

	DEF_case(get_collection) {
		coll = mongoc_client_get_collection(client, "db_name", "coll_name");
		EXPECT(NULL != coll);
	}

	DEF_case(find_all_doc) {
		COUT << "查询集合内所有文档...";
		bson_t *query = bson_new();
		EXPECT(NULL != query);
		mongoc_cursor_t *cursor = mongoc_collection_find_with_opts(coll, query, NULL, NULL);

		//遍历查询结果集
		char *result = NULL;
		const bson_t *qry_doc = NULL;
		int index = 0;
		while (mongoc_cursor_next(cursor, &qry_doc)) {
			result = bson_as_canonical_extended_json(qry_doc, NULL);
			COUT << "[" << ++index << "]: " << result;
			bson_free(result);
		}
		mongoc_cursor_destroy(cursor);
		result = NULL;
	}

	DEF_case(insert_one_doc) {
		bson_oid_init(&oid, NULL);
		bson_t *doc = BCON_NEW("_id", BCON_OID(&oid), "key", BCON_UTF8("old_value"));
		EXPECT(NULL != doc);

		bson_error_t error;
		bool ret = mongoc_collection_insert_one(coll, doc, NULL, NULL, &error);
		if (!ret) COUT << error.message;
		bson_destroy(doc);
		doc = NULL;
	}

	DEF_case(find_all_doc) {
		COUT << "查询集合内所有文档...";
		bson_t *query = bson_new();
		EXPECT(NULL != query);
		mongoc_cursor_t *cursor = mongoc_collection_find_with_opts(coll, query, NULL, NULL);

		//遍历查询结果集
		char *result = NULL;
		const bson_t *qry_doc = NULL;
		int index = 0;
		while (mongoc_cursor_next(cursor, &qry_doc)) {
			result = bson_as_canonical_extended_json(qry_doc, NULL);
			COUT << "[" << ++index << "]: " << result;
			bson_free(result);
		}
		mongoc_cursor_destroy(cursor);
		result = NULL;
	}

	DEF_case(update) {
		bson_t *query = BCON_NEW("_id", BCON_OID(&oid));
		EXPECT(NULL != query);
		bson_t *update = BCON_NEW("$set",
			"{",
			"key",
			BCON_UTF8("new_value"),
			"updated",
			BCON_BOOL(true),
			"}");
		EXPECT(NULL != update);

		bson_error_t error;
		bool ret = mongoc_collection_update_one(coll, query, update, NULL, NULL, &error);
		if(!ret) COUT << error.message;

		bson_destroy(query);
		bson_destroy(update);
	}

	DEF_case(find_all_doc) {
		COUT << "查询集合内所有文档...";
		bson_t *query = bson_new();
		EXPECT(NULL != query);
		mongoc_cursor_t *cursor = mongoc_collection_find_with_opts(coll, query, NULL, NULL);

		//遍历查询结果集
		char *result = NULL;
		const bson_t *qry_doc = NULL;
		int index = 0;
		while (mongoc_cursor_next(cursor, &qry_doc)) {
			result = bson_as_canonical_extended_json(qry_doc, NULL);
			COUT << "[" << ++index << "]: " << result;
			bson_free(result);
		}
		mongoc_cursor_destroy(cursor);
		result = NULL;
	}

	DEF_case(clean_up) {
		mongoc_collection_destroy(coll); coll = NULL;
		mongoc_client_destroy(client); client = NULL;
		mongoc_cleanup();
	}
}

DEF_test(count_doc) {
	//使用前初始化
	mongoc_init();

	mongoc_client_t *client;
	mongoc_collection_t *coll;

	DEF_case(create_client) {
		std::string uri("mongodb://localhost:27017/?appname=count-example");
		client = mongoc_client_new(uri.c_str());
		EXPECT(NULL != client);
	}

	DEF_case(get_collection) {
		coll = mongoc_client_get_collection(client, "db_name", "db_coll");
		EXPECT(NULL != coll);
	}

	DEF_case(count) {
		bson_error_t error;
		bson_t *doc = bson_new_from_json(
			(const uint8_t *) "{\"key\" : \"new_value\"}", -1, &error);

		EXPECT(NULL != doc);
		if (NULL == doc) CERR << "bson_new_from_json Failed: " << error.message;

		int64 count = mongoc_collection_count(
			coll, MONGOC_QUERY_NONE, doc, 0, 0, NULL, &error);
		if (count < 0) CERR << "mongoc_collection_count Failed: " << error.message;
		else COUT << "count: " << count;
		bson_destroy(doc); doc = NULL;
	}

	mongoc_collection_destroy(coll);
	mongoc_client_destroy(client);
	mongoc_cleanup();
}

DEF_test(exec_cmd) {
	//使用前初始化
	mongoc_init();

	mongoc_client_t *client;
	mongoc_collection_t *coll;

	DEF_case(create_client) {
		std::string uri("mongodb://localhost:27017/?appname=executing-example");
		client = mongoc_client_new(uri.c_str());
		EXPECT(NULL != client);
	}

	DEF_case(get_collection) {
		coll = mongoc_client_get_collection(client, "db_name", "coll_name");
		EXPECT(NULL != coll);
	}

	DEF_case(cmd) {
		bson_error_t error;
		bson_t reply;
		char *result;
		bson_t *cmd = BCON_NEW("collStats", BCON_UTF8("coll_name"));
		if (mongoc_collection_command_simple(coll, cmd, NULL, &reply, &error)) {
			result = bson_as_canonical_extended_json(&reply, NULL);
			COUT << result;
			bson_free(result);
		} else {
			CERR << "Failed to run command: %s\n" << error.message;
		}
		bson_destroy(cmd);
		bson_destroy(&reply);
	}

	mongoc_collection_destroy(coll);
	mongoc_client_destroy(client);
	mongoc_cleanup();
}


/************************************************************************/
/* 
database: stumanager
collection: baseinfo

{
"id": "0000001",
"name": "allen",
"gender": "boy",
"age": 24,
"subject": [{
		"name": "math",
		"score": 95.00
	}, {
		"name": "english",
		"score": 97.00
	}, {
		"name": "chinese",
		"score": 88.50
	}]
}

{
"id": "0000002",
"name": "comet",
"gender": "boy",
"age": 20,
"subject": [{
		"name": "math",
		"score": 70.00
	}, {
		"name": "english",
		"score": 59.00
	}, {
		"name": "chinese",
		"score": 68.50
	}]
}

{
"id": "0000003",
"name": "ciel",
"gender": "girl",
"age": 22,
"subject": [{
		"name": "math",
		"score": 70.00
	}, {
		"name": "english",
		"score": 59.00
	}, {
		"name": "chinese",
		"score": 68.50
	}]
}

{
"id": "0000004",
"name": "carol",
"gender": "girl",
"age": 23,
"subject": [{
		"name": "math",
		"score": 70.00
	}, {
		"name": "english",
		"score": 59.00
	}, {
		"name": "chinese",
		"score": 68.50
	}]
}

(1)查询所有男孩的信息

(2)查询所有女孩的信息

(3)查询数学90分以上的人数

(4)查询英语90分以上的女生人数

(5)查询数学90分以上或英语90分以上的人数

(6)查询数学90分以上或英语90分以上的同学总分

(7)查询数学90分以上或英语90分以上的同学平均分

(8)按英语成绩从高到低排序

(9)统计英语、数学、语文总分及平均分
*/
/************************************************************************/

namespace mongo {

bool insert_collection(mongoc_collection_t *coll, bson_t *doc, std::string &error) {
	if (nullptr == coll || nullptr == doc) {
		error = "collection or document is null";
		return false;
	}

	bson_error_t err;
	bool ret = mongoc_collection_insert_one(coll, doc, NULL, NULL, &err);
	if (!ret) {
		error = err.message;
		return false;
	} else {
		return true;
	}
}

std::string show_as_json(bson_t *doc) {
	if (nullptr == doc) return std::string();
	std::string ret;
	char *result = bson_as_canonical_extended_json(doc, NULL);
	if (nullptr != result) {
		ret = result;
		bson_free(result);
	}
	return ret;
}

std::vector<std::string> results_as_json(mongoc_cursor_t *cursor) {
	if (nullptr == cursor) return std::vector<std::string>();
	std::vector<std::string> results;
	char *result = NULL;
	const bson_t *qry_doc = NULL;
	int index = 0;
	while (mongoc_cursor_next(cursor, &qry_doc)) {//遍历查询结果集
		result = bson_as_canonical_extended_json(qry_doc, NULL);
		if (nullptr != result) {
			results.push_back(std::string(result));
			bson_free(result);
		}
	}
	return results;
}

}//namespace mogo


DEF_test(full_test) {
	//初始化MongoDB
	mongoc_init();

	mongoc_client_t *client;
	mongoc_collection_t *coll;
	std::string db_name("stumanager");
	std::string coll_name("baseinfo");
	std::string uri("mongodb://localhost:27017/?appname=full_test");

	DEF_case(create_client) {
		client = mongoc_client_new(uri.c_str());
		EXPECT(NULL != client);
	}

	DEF_case(get_collection) {
		coll = mongoc_client_get_collection(client, db_name.c_str(), coll_name.c_str());
		EXPECT(NULL != coll);
	}

	//删除所有文档
	DEF_case(del_all_doc) {
		COUT << "删除集合内所有文档...";
		bson_t *doc = bson_new();
		EXPECT(NULL != doc);
		bson_error_t error;
		bool ret = mongoc_collection_delete_many(coll, doc, NULL, NULL, &error);
		EXPECT_EQ(true, ret);
		if (!ret) COUT << "Delete failed: " << error.message;
		bson_destroy(doc);
		doc = NULL;
	}

	DEF_case(insert_first_students) {
		bson_t   *document = BCON_NEW(
			"id", BCON_UTF8("0000001"),
			"name", BCON_UTF8("allen"),
			"gender", BCON_UTF8("boy"),
			"age", BCON_INT32(24),
			"subject", "[",
			"{", "name", BCON_UTF8("math"), "score", BCON_DOUBLE(95.00), "}",
			"{", "name", BCON_UTF8("english"), "score", BCON_DOUBLE(88.00), "}",
			"{", "name", BCON_UTF8("chinese"), "score", BCON_DOUBLE(79.50), "}",
			"]");

		COUT << "新增第一条文档：" << mongo::show_as_json(document);

		std::string error;
		bool ret = mongo::insert_collection(coll, document, error);
		if (!ret) CERR << error;

		bson_destroy(document);
	}

	DEF_case(insert_second_students) {
		bson_t   *document = BCON_NEW(
			"id", BCON_UTF8("0000002"),
			"name", BCON_UTF8("comet"),
			"gender", BCON_UTF8("boy"),
			"age", BCON_INT32(21),
			"subject", "[",
			"{", "name", BCON_UTF8("math"), "score", BCON_DOUBLE(88.00), "}",
			"{", "name", BCON_UTF8("english"), "score", BCON_DOUBLE(78.50), "}",
			"{", "name", BCON_UTF8("chinese"), "score", BCON_DOUBLE(90.00), "}",
			"]");

		COUT << "新增第二条文档：" << mongo::show_as_json(document);

		std::string error;
		bool ret = mongo::insert_collection(coll, document, error);
		if (!ret) CERR << error;

		bson_destroy(document);
	}

	DEF_case(insert_third_students) {
		bson_t   *document = BCON_NEW(
			"id", BCON_UTF8("0000003"),
			"name", BCON_UTF8("ciel"),
			"gender", BCON_UTF8("girl"),
			"age", BCON_INT32(22),
			"subject", "[",
			"{", "name", BCON_UTF8("math"), "score", BCON_DOUBLE(78.00), "}",
			"{", "name", BCON_UTF8("english"), "score", BCON_DOUBLE(68.50), "}",
			"{", "name", BCON_UTF8("chinese"), "score", BCON_DOUBLE(70.00), "}",
			"]");

		COUT << "新增第三条文档：" << mongo::show_as_json(document);

		std::string error;
		bool ret = mongo::insert_collection(coll, document, error);
		if (!ret) CERR << error;

		bson_destroy(document);
	}

	DEF_case(insert_fourth_students) {
		bson_t   *document = BCON_NEW(
			"id", BCON_UTF8("0000004"),
			"name", BCON_UTF8("carol"),
			"gender", BCON_UTF8("girl"),
			"age", BCON_INT32(23),
			"subject", "[",
			"{", "name", BCON_UTF8("math"), "score", BCON_DOUBLE(88.00), "}",
			"{", "name", BCON_UTF8("english"), "score", BCON_DOUBLE(85.50), "}",
			"{", "name", BCON_UTF8("chinese"), "score", BCON_DOUBLE(78.00), "}",
			"]");

		COUT << "新增第四条文档：" << mongo::show_as_json(document);

		std::string error;
		bool ret = mongo::insert_collection(coll, document, error);
		if (!ret) CERR << error;

		bson_destroy(document);
	}

	DEF_case(find_all) {
		COUT << "查询集合内所有文档...";
		bson_t *query = bson_new();
		EXPECT(NULL != query);
		mongoc_cursor_t *cursor = mongoc_collection_find_with_opts(coll, query, NULL, NULL);

		std::vector<std::string> v = mongo::results_as_json(cursor);
		for (const auto &x : v) {
			COUT << x;
		}
		mongoc_cursor_destroy(cursor);
		bson_destroy(query);
	}

	//(2)查询集合内所有男孩的文档
	DEF_case(first_question) {
		COUT << "查询集合内所有男孩的文档...";
		bson_t *query = bson_new();
		BSON_APPEND_UTF8(query, "gender", "boy");
		EXPECT(NULL != query);
		mongoc_cursor_t *cursor = mongoc_collection_find_with_opts(coll, query, NULL, NULL);

		std::vector<std::string> v = mongo::results_as_json(cursor);
		for (const auto &x : v) {
			COUT << x;
		}

		mongoc_cursor_destroy(cursor);
		bson_destroy(query);
	}

	//(2)查询集合内所有女孩的文档
	DEF_case(second_question) {
		COUT << "查询集合内所有女孩的文档...";
		bson_t *query = bson_new();
		BSON_APPEND_UTF8(query, "gender", "girl");
		EXPECT(NULL != query);
		mongoc_cursor_t *cursor = mongoc_collection_find_with_opts(coll, query, NULL, NULL);

		std::vector<std::string> v = mongo::results_as_json(cursor);
		for (const auto &x : v) {
			COUT << x;
		}

		mongoc_cursor_destroy(cursor);
		bson_destroy(query);
	}

	//(3)查询数学80分以上的记录：
	//db.getCollection('baseinfo').find({"subject.name":"math","subject.score":{$gt:80.00}})
	//查询数学80分以上的人数：
	//db.getCollection('baseinfo').find({"subject.name":"math","subject.score":{$gt:80.00}}).count()
	DEF_case(third_question) {
		bson_error_t error;
		bson_t *query = bson_new();
		query = BCON_NEW(\
			"subject.name","math",\
			"subject.score",\
			"{","$gt",BCON_INT64(80.00),"}"\
		);
		EXPECT(NULL != query);

		int64_t count = mongoc_collection_count(
			coll, MONGOC_QUERY_NONE, query, 0, 0, NULL, &error);

		COUT << "查询数学80分以上的人数: " << count;
		bson_destroy(query);
	}

	//(4)查询男孩数学80分以上的记录：
	//db.getCollection('baseinfo').find({"gender":"boy","subject.name":"math","subject.score":{$gt:80.00}})
	//查询男孩数学80分以上的人数：
	//db.getCollection('baseinfo').find({"gender":"boy","subject.name":"math","subject.score":{$gt:80.00}}).count()
	DEF_case(fourth_question) {
		bson_error_t error;
		bson_t *query = bson_new();
		query = BCON_NEW(\
			"gender", "boy", \
			"subject.name", "math", \
			"subject.score", \
			"{", "$gt", BCON_INT64(80.00), "}"\
		);
		EXPECT(NULL != query);

		int64_t count = mongoc_collection_count(
			coll, MONGOC_QUERY_NONE, query, 0, 0, NULL, &error);

		COUT << "查询男孩数学80分以上的人数: " << count;
		bson_destroy(query);
	}

	//(5)查询英语80分以上或数学90分以上的人数:
	/*
		db.getCollection('baseinfo').find({$or:[
			{"subject.name":"english","subject.score":{$gt:80.00}},
			{"subject.name":"math","subject.score":{$gt:90.00}}]}
		).count()
	*/
	DEF_case(fifth_question) {
		bson_error_t error;
		bson_t *query = bson_new();
		query = BCON_NEW(\
			"$or",\
			"[",\
				"{", \
					"subject.name", "english",\
					"subject.score", "{", \
						"$gt", BCON_INT64(80.00), \
					"}",\
				"}",\
				"{", \
					"subject.name", "math", \
					"subject.score", "{", \
						"$gt", BCON_INT64(90.00), \
					"}", \
				"}", \
			"]"
		);
		EXPECT(NULL != query);

		int64_t count = mongoc_collection_count(
			coll, MONGOC_QUERY_NONE, query, 0, 0, NULL, &error);

		COUT << "查询英语80分以上或数学90分以上的人数: " << count;
		bson_destroy(query);
	}

	//(6)查询数学90分以上或英语80分以上的总分
	/*
		db.getCollection('baseinfo').aggregate([
			{$match:{
				$or:[
					{"subject.name":"english","subject.score":{$gt:80.00}},
					{"subject.name":"math","subject.score":{$gt:90.00}}
				]
			}},
			{$unwind: "$subject"},
			{$group:{_id:{name:"$name"},sum:{$sum:"$subject.score"}}}
		])
	*/
	DEF_case(sixth_question) {
	}

	//(7)查询数学90分以上或英语90分以上的平均分
	/*
		db.getCollection('baseinfo').aggregate([
			{$match:{
				$or:[
					{"subject.name":"english","subject.score":{$gt:80.00}},
					{"subject.name":"math","subject.score":{$gt:90.00}}
				]
			}},
			{$unwind: "$subject"},
			{$group:{_id:{name:"$name"},avg:{$avg:"$subject.score"}}}
		])
	*/

	//(8)按英语成绩从高到低排序（非关系数据库内联操作比较麻烦，如果存在这样的操作，最好重新设计文档结构，
	//在这里通过管道实现，即上一个命令的输出作为下一个命令的输入）
	/*
		db.getCollection('baseinfo').aggregate([
			{$unwind: "$subject"},
			{$group:{_id:{name:"$name",subname:"$subject.name",score:"$subject.score"}}},
			{$match:{"_id.subname":"english"}},
			{$sort:{"_id.score":-1}}
		])

		db.getCollection('baseinfo').aggregate([
			{$match:{
				$or:[
					{"subject.name":"english","subject.score":{$gt:80.00}},
					{"subject.name":"math","subject.score":{$gt:90.00}}
				]
			}},
			{$unwind: "$subject"},
			{$group:{_id:{name:"$name",subname:"$subject.name",score:"$subject.score"}}},
            {$match:{"_id.subname":"english"}},
            {$sort:{"_id.score":-1}}
		])
	*/

	//(9)统计所有人英语、数学、语文总分及平均分
	/*
		//所有人英语总分
		db.getCollection('baseinfo').aggregate([
			{$unwind: "$subject"},
			{$group:{_id:{name:"$name",subname:"$subject.name",score:"$subject.score"}}},
            {$match:{"_id.subname":"english"}},
			{$unwind: "$_id"},
			{$group:{_id:"english",sum:{$sum:"$_id.score"}}}
		])

		//所有人英语平均分
		db.getCollection('baseinfo').aggregate([
			{$unwind: "$subject"},
			{$group:{_id:{name:"$name",subname:"$subject.name",score:"$subject.score"}}},
			{$match:{"_id.subname":"english"}},
			{$unwind: "$_id"},
			{$group:{_id:"english",avg:{$avg:"$_id.score"}}}
		])

		//所有人数学总分
		db.getCollection('baseinfo').aggregate([
			{$unwind: "$subject"},
			{$group:{_id:{name:"$name",subname:"$subject.name",score:"$subject.score"}}},
			{$match:{"_id.subname":"math"}},
			{$unwind: "$_id"},
			{$group:{_id:"math",sum:{$sum:"$_id.score"}}}
		])

		//所有人数学平均分
		db.getCollection('baseinfo').aggregate([
			{$unwind: "$subject"},
			{$group:{_id:{name:"$name",subname:"$subject.name",score:"$subject.score"}}},
			{$match:{"_id.subname":"math"}},
			{$unwind: "$_id"},
			{$group:{_id:"math",avg:{$avg:"$_id.score"}}}
		])

		//所有人语文总分
		db.getCollection('baseinfo').aggregate([
			{$unwind: "$subject"},
			{$group:{_id:{name:"$name",subname:"$subject.name",score:"$subject.score"}}},
			{$match:{"_id.subname":"chinese"}},
			{$unwind: "$_id"},
			{$group:{_id:"chinese",sum:{$sum:"$_id.score"}}}
		])

		//所有人语文平均分
		db.getCollection('baseinfo').aggregate([
			{$unwind: "$subject"},
			{$group:{_id:{name:"$name",subname:"$subject.name",score:"$subject.score"}}},
			{$match:{"_id.subname":"chinese"}},
			{$unwind: "$_id"},
			{$group:{_id:"chinese",avg:{$avg:"$_id.score"}}}
		])
	*/

	mongoc_collection_destroy(coll);
	mongoc_client_destroy(client);
	mongoc_cleanup();
}

namespace anony{

typedef struct MonQry {
	bson_t *query;
	int32 flow;
}MonQry;

typedef struct MonRsp {
	std::string result;
	int32 flow;
}MonRsp;

safe::block_queue<MonQry> qry;
safe::block_queue<MonRsp> rsp;
safe::SyncEvent ev_build;


void qry_doc(mongoc_client_t *client, anony::MonQry &pack) {
	std::string db_name("stumanager");
	std::string coll_name("baseinfo");

	mongoc_collection_t *coll = mongoc_client_get_collection(client, db_name.c_str(), coll_name.c_str());

	mongoc_cursor_t *cursor = mongoc_collection_find_with_opts(coll, pack.query, NULL, NULL);

	std::vector<std::string> v = mongo::results_as_json(cursor);
	for (const auto &x : v) COUT << x;
	mongoc_cursor_destroy(cursor);

	bson_destroy(pack.query);
	mongoc_collection_destroy(coll);
}

}//namespace anony


void qry_worker(mongoc_client_pool_t *pool) {
	for (;;) {
		anony::MonQry pack;
		if (anony::qry.pop(pack)) {
			// 从连接池取出一条连接
			mongoc_client_t *client = mongoc_client_pool_pop(pool);

			anony::qry_doc(client, pack);

			// 用完后需要还回客户端池
			mongoc_client_pool_push(pool, client);
		}
	}
}

void rsp_worker() {
	for (;;) {
		anony::MonRsp pack;
		anony::rsp.pop(pack);

		// do something
		
	}
}

void build_qry() {
	for (int i = 0; i < 100; ++i) {
		bson_t *query = bson_new();

		anony::MonQry pack;
		pack.query = query;
		pack.flow = i;

		anony::qry.push(pack);
	}

	sys::sleep(5);
	anony::ev_build.signal();
}

DEF_test(mongo_mt) {
	safe::Thread th_rsp(
#if __cplusplus >= 201103L
		std::bind(&rsp_worker)
#else
		boost::bind(&rsp_worker)
#endif
	);
	th_rsp.start();

	mongoc_client_pool_t *pool;
	mongoc_uri_t         *uri;

	mongoc_init();

	uri = mongoc_uri_new("mongodb://localhost:27017/?appname=mongo_threadpool");
	// 创建客户端池
	pool = mongoc_client_pool_new(uri);

#if __cplusplus >= 201103L
	typedef std::shared_ptr<safe::Thread> ThreadPtr;
#else
	typedef boost::shared_ptr<safe::Thread> ThreadPtr;
#endif
	std::vector<ThreadPtr> threads;
	for (int i = 0; i < 10; ++i) {
#if __cplusplus >= 201103L
		ThreadPtr thr(new safe::Thread(std::bind(&qry_worker, pool)));
#else
		ThreadPtr thr(new boost::Thread(boost::bind(&qry_worker, pool)));
#endif
		thr->start();
		threads.push_back(thr);
	}

	safe::Thread th_build(
#if __cplusplus >= 201103L
		std::bind(&build_qry)
#else
		boost::bind(&build_qry)
#endif
	);
	th_build.start();

	anony::ev_build.wait();
	COUT << "创建查询请求完毕，准备回收BUILD线程...";

	th_build.join();
	COUT << "回收BUILD线程完毕...";


	COUT << "查询请求处理完毕，准备回收QUERY线程...";
	for (int i = 0; i < threads.size(); ++i) {
		threads[i]->join();
	}
	COUT << "回收QUERY线程完毕...";

	th_rsp.join();

	// 释放客户端池
	mongoc_client_pool_destroy(pool);
	mongoc_uri_destroy(uri);
	mongoc_cleanup();
}

}//namespace test