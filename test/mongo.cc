#include "../util/util.h"

#include <mongoc.h>
#include <bson.h>
#include <bcon.h>

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


}//namespace test