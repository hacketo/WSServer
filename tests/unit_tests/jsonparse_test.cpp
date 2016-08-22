//
// Created by hacketo on 11/08/16.
//


#include "gtest/gtest.h"
#include "../../src/protocol/packet.h"
#include "../../src/server/config.h"

using namespace protocol::packet;

TEST(jsonparse_test, testaccessingvalues){
	config::DEFAULT_PARSER = protocol::packet::ParserType::JSON;

	std::string json = "[{\"action\":\"test1\",\"data\":{\"d1\":\"v1\",\"d2\":0,\"d3\":[\"v1\",0,false,null,1.5]}},{\"action\":\"test2\",\"data\":{\"d2\":\"v2\"}}]";

	errors::error error;
	Packet::u_ptr p = Packet::u_ptr(new Packet());
	parse(p.get(), json, error);

	PacketData* p1 = p->get(0);
	PacketData* p2 = p->get(1);

	std::string newJson = toJson(p.get(),false);

	std::string value1 = (*(*p1->data)["d3"])[0]->getString();
	std::string value2 = (*p2->data)["d2"]->getString();

	EXPECT_STREQ(value1.c_str(), "v1");
	EXPECT_STREQ(value2.c_str(), "v2");

	assert(newJson.compare(json) == 0);

}