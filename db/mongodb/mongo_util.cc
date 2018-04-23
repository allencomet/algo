#include "mongo_util.h"

namespace dbmongo {

std::string MongoDoc::to_json() {
	if (nullptr == _doc) return std::string();
	std::string ret;
	char *result = bson_as_canonical_extended_json(_doc, NULL);
	if (nullptr != result) {
		ret = result;
		bson_free(result);
	}
	return ret;
}

std::vector<std::string> MongoHelper::find_all() {
	std::vector<std::string> results;

	bson_t *query = bson_new();
	mongoc_cursor_t *cursor = mongoc_collection_find_with_opts(_collection, query, NULL, NULL);

	char *result = NULL;
	const bson_t *qry_doc = NULL;
	while (mongoc_cursor_next(cursor, &qry_doc)) {//遍历查询结果集
		result = bson_as_canonical_extended_json(qry_doc, NULL);
		if (nullptr != result) {
			results.push_back(std::string(result));
			bson_free(result);
		}
	}

	mongoc_cursor_destroy(cursor);
	bson_destroy(query);

	return results;
}

std::vector<std::string> MongoHelper::find(MongoDoc &doc) {
	std::vector<std::string> results;

	mongoc_cursor_t *cursor = mongoc_collection_find_with_opts(_collection, doc.doc(), NULL, NULL);

	char *result = NULL;
	const bson_t *qry_doc = NULL;
	while (mongoc_cursor_next(cursor, &qry_doc)) {//遍历查询结果集
		result = bson_as_canonical_extended_json(qry_doc, NULL);
		if (nullptr != result) {
			results.push_back(std::string(result));
			bson_free(result);
		}
	}

	mongoc_cursor_destroy(cursor);
	return results;
}

bool MongoHelper::remove_all() {
	bson_t *doc = bson_new();
	bson_error_t error;
	bool ret = mongoc_collection_delete_many(_collection, doc, NULL, NULL, &error);
	if (!ret) COUT << "Delete failed: " << error.message;
	bson_destroy(doc);
	doc = NULL;
	return ret;
}

}