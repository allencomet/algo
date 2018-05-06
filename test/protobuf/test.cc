#include "../../util/util.h"

#include <string>

#include "addressbook.pb.h"

namespace {

void setting_person(tutorial::Person &person) {
	person.set_name("allen");
	person.set_id(1129);
	person.set_email("r9wemeet@outlook.com");

	{
		tutorial::Person::PhoneNumber *phone_number = person.add_phones();
		phone_number->set_number("18720969208");
		phone_number->set_type(tutorial::Person::MOBILE);
	}

	{
		tutorial::Person::PhoneNumber *phone_number = person.add_phones();
		phone_number->set_number("13378462406");
		phone_number->set_type(tutorial::Person::WORK);
	}
}

void setting_addressbook(tutorial::AddressBook &w_address_book) {
	tutorial::Person *per = w_address_book.add_people();
	setting_person(*per);
}

void list_addressbook(const tutorial::AddressBook &w_address_book) {
	COUT << "----------person size: " << w_address_book.people_size()
		<< "----------";
	for (size_t i = 0; i < w_address_book.people_size(); ++i) {
		const tutorial::Person &person = w_address_book.people(i);
		COUT << "person name: " << person.name();
		COUT << "person id: " << person.id();
		COUT << "person email: " << person.email();

		COUT << "\t[- ----------phones size: " << person.phones_size()
			<< "----------";

		for (size_t j = 0; j < person.phones_size(); ++j) {
			const tutorial::Person::PhoneNumber &phone = person.phones(j);
			COUT << "\t[-[" << j + 1 << "] phone number: " << phone.number();
			switch (phone.type()) {
			case tutorial::Person::MOBILE:
				COUT << "\t[-[" << j + 1 << "] phone type: MOBILE["
					<< phone.type() << "]";
				break;
			case tutorial::Person::HOME:
				COUT << "\t[-[" << j + 1 << "] phone type: HOME["
					<< phone.type() << "]";
				break;
			case tutorial::Person::WORK:
				COUT << "\t[-[" << j + 1 << "] phone type: WORK["
					<< phone.type() << "]";
				break;
			default:
				CERR << "\t[-[" << j + 1 << "] unknow";
				break;
			}//end-switch
		}//end-for
	}//end-for
}

}

namespace test {

DEF_test(test_protobuf) {
	// Verifyt hat the version of the library that we linked against is
	// compatible with the version of the headers we compiled against.
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	tutorial::AddressBook w_address_book;

	COUT << "Start >>>>>>>>>>>>>>>>>>>>> 设置消息";
	setting_addressbook(w_address_book);
	setting_addressbook(w_address_book);
	COUT << "Done >>>>>>>>>>>>>>>>>>>>>\n";

	COUT << "Start >>>>>>>>>>>>>>>>>>>>> 查看消息:";
	list_addressbook(w_address_book);
	COUT << "Done >>>>>>>>>>>>>>>>>>>>>\n";

	COUT << "Start >>>>>>>>>>>>>>>>>>>>> 序列化";
	std::string data;
	w_address_book.SerializeToString(&data);//序列化
	COUT << "Done >>>>>>>>>>>>>>>>>>>>>\n";


	COUT << "Start >>>>>>>>>>>>>>>>>>>>> 反序列化";
	tutorial::AddressBook r_address_book;
	r_address_book.ParseFromString(data);//反序列化
	COUT << "Done >>>>>>>>>>>>>>>>>>>>>\n";

	COUT << "Start >>>>>>>>>>>>>>>>>>>>> 反序列化后数据如下：";
	list_addressbook(w_address_book);
	COUT << "Done >>>>>>>>>>>>>>>>>>>>>\n";

	//Optional:  Delete all global objects allocated by libprotobuf.
	google::protobuf::ShutdownProtobufLibrary();
}

}//namespace test
