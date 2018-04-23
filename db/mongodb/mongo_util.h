#ifndef __DB_MONGODB_UTIL_H
#define __DB_MONGODB_UTIL_H

#include "../../util/util.h"

#include <mongoc.h>
#include <bson.h>
#include <bcon.h>

#include <string>

namespace dbmongo {

class MongoInit {
public:
	MongoInit() {
		mongoc_init();
	}
	~MongoInit() {
		mongoc_cleanup();
	}
private:
	DISALLOW_COPY_AND_ASSIGN(MongoInit);
};

class MongoClient {
public:
	explicit MongoClient(const std::string &uri,bool manual=false) 
		:_uri(uri) {
		if (!manual) {
			connect();
		} else {
			_client = NULL;
		}
	}
	~MongoClient() {
		if (NULL != _client) {
			mongoc_client_destroy(_client);
			_client = NULL;
		}
	}

	void connect() {
		if(NULL == _client) _client = mongoc_client_new(_uri.c_str());
	}

	mongoc_client_t *rawclient() {
		return _client;
	}

private:
	mongoc_client_t *_client;
	std::string _uri;
	DISALLOW_COPY_AND_ASSIGN(MongoClient);
};

class MongoDoc {
public:
	explicit MongoDoc(bson_t *doc):_doc(doc) {
		//_doc = bson_new();
	}

	~MongoDoc() {
		bson_destroy(_doc);
		_doc = NULL;
	}

	std::string to_json();

	void append(const std::string &key, const std::string value) {
		BSON_APPEND_UTF8(_doc, key.c_str(), value.c_str());
	}

	bson_t *doc() {
		return _doc;
	}
private:
	bson_t *_doc;
	DISALLOW_COPY_AND_ASSIGN(MongoDoc);
};

class MongoHelper {
public:
	MongoHelper(MongoClient &client,const std::string &db,const std::string &coll) 
		:_client(client),_db(db),_coll(coll) {
		_collection = mongoc_client_get_collection(client.rawclient(), _db.c_str(), _coll.c_str());
	}
	~MongoHelper() {
		mongoc_collection_destroy(_collection);
	}

	std::string db_name() const {
		return _db;
	}

	std::string coll_name() const {
		return _coll;
	}

	std::vector<std::string> find_all();
	std::vector<std::string> find(MongoDoc &doc);

	bool remove_all();

	
private:
	MongoClient &_client;
	mongoc_collection_t *_collection;
	std::string _db;
	std::string _coll;
	DISALLOW_COPY_AND_ASSIGN(MongoHelper);
};

}

#endif