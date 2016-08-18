//
// Created by hacketo on 09/08/16.
//

#include "packet.h"
#include "../../debug.h"
#include "../../../lib/rapidjson/include/rapidjson/error/en.h"
#include "../../server/config.h"
#include <boost/algorithm/string/predicate.hpp>
namespace protocol {
namespace packet{

	PacketData::PacketData(GenericValue* value) : data(GenericValue::u_ptr(value)), packetType(DATA), consumed(false){};
	PacketData::PacketData() : packetType(DATA), consumed(false){};
	ActionPacketData::ActionPacketData(std::string action, GenericValue* value) : PacketData(value), action(action){ packetType = ACTION; };
	ActionPacketData::ActionPacketData(GenericValue* value) : PacketData(value){ packetType = ACTION; };
	ActionPacketData::ActionPacketData() : PacketData(){ packetType = ACTION; };
	ModulePacketData::ModulePacketData(uint64_t  moduleId, std::string action, GenericValue* value) : ActionPacketData(value){ packetType = MODULE; };
	ModulePacketData::ModulePacketData(GenericValue* value) : ActionPacketData(value){ packetType = MODULE; };
	ModulePacketData::ModulePacketData() : ActionPacketData(){ packetType = MODULE; };

	Packet::Packet() : packets() {}

	Packet::Packet(PacketData* data) : packets() {
		addPacket(data);
	}
	Packet::Packet(GenericValue* value) : packets() {
		addPacket(new PacketData(value));
	}
	Packet::Packet(std::string action, GenericValue* value) : packets() {
		addPacket(new ActionPacketData(action, value));
	}
	Packet::Packet(uint64_t  moduleId, std::string action, GenericValue* value) : packets() {
		addPacket(new ModulePacketData(moduleId, action, value));
	}

	Packet::~Packet() {
		// check a delete les pointeur d'abord
		packets.clear();
	}
	void Packet::addPacket(PacketData* data) {
		packets.push_back(PacketData::u_ptr(data));
	}

	PacketData* Packet::get(unsigned long index) {
		return packets.at(index).get();
	}

	frame::Frame Packet::getFrame() {
		if (config::DEFAULT_PARSER == ParserType::JSON){
			return frame::from_string(toJson(this));
		}
		return frame::from_string(this->get(0)->data->getString());
	}


	void parse(Packet *packet, std::string valueToParse, errors::error& error) {
		if (config::DEFAULT_PARSER == ParserType::JSON){
			parseFromJSON(packet, valueToParse, error);
			return;
		}

		PacketData *packetData = new PacketData(new StringValue(valueToParse));
		packet->addPacket(packetData);
	}

	void parseFromJSON(Packet *packet, std::string json, errors::error& error) {

		rapidjson::Document document;
		rapidjson::Document::AllocatorType &a = document.GetAllocator();

		char buffer[json.size()];
		memcpy(buffer, json.c_str(), json.size());
		document.Parse<rapidjson::kParseStopWhenDoneFlag>(buffer);

		if (document.HasParseError()) {
			std::stringstream ss;
			ss << "\nError(offset %u): %s\n" << (unsigned) document.GetErrorOffset() << GetParseError_En(document.GetParseError());
			error = errors::get_error("Packet", errors::code::JSON_DECODE, ss.str() );
			return;
		}

		if (document.IsArray()) {
			for (rapidjson::SizeType i = 0; i < document.Size(); i++) {
				parseFromJSON(packet, &document[i], error);
			}
		} else if (document.IsObject()) {
			parseFromJSON(packet, &document, error);
		}

	}

	void parseFromJSON(Packet* packet, rapidjson::Value* object, errors::error& error) {

		const char * actionKey = config::ACTION_KEY.c_str();
		const char * moduleKey = config::MODULE_KEY.c_str();
		const char * moduleActionKey = config::MODULE_ACTION_KEY.c_str();
		const char * dataKey = config::DATA_KEY.c_str();

		PacketData* data;
		if (object->HasMember(moduleKey)){
			ModulePacketData * d = new ModulePacketData();
			d->module = (*object)[moduleKey].GetInt();
			d->action = (*object)[moduleActionKey].GetString();
			data = d;
		}
		else if (object->HasMember(actionKey)){
			ActionPacketData * d = new ActionPacketData();
			d->action = (*object)[actionKey].GetString();
			data = d;
		}

		if (!data){
			data = new PacketData();
		}

		if (object->HasMember(dataKey)){
			data->data = GenericValue::u_ptr(GenericValue::parse(&(*object)[dataKey]));
		}
		else{
			data->data = GenericValue::u_ptr(GenericValue::parse(object));
		}


		packet->addPacket(data);
	}

	std::string toJson(Packet* packet, bool prettyprint) {
		const char * actionKey = config::ACTION_KEY.c_str();
		const char * moduleKey = config::MODULE_KEY.c_str();
		const char * moduleActionKey = config::MODULE_ACTION_KEY.c_str();
		const char * dataKey = config::DATA_KEY.c_str();

		rapidjson::StringBuffer sb;

		rapidjson::Writer<rapidjson::StringBuffer>* writer;

		if (prettyprint) {
			writer = new rapidjson::PrettyWriter<rapidjson::StringBuffer>(sb);
		} else {
			writer = new rapidjson::Writer<rapidjson::StringBuffer>(sb);
		}

		writer->StartArray();
		for (packet_iterator it = packet->begin(); it != packet->end(); ++it) {
			writer->StartObject();
			PacketData *packetData = it->get();

			if (packetData->packetType == PacketType::ACTION) {
				ActionPacketData *p = reinterpret_cast<ActionPacketData*>(packetData);
				writer->Key(actionKey);
				writer->String(p->action.c_str(), static_cast<rapidjson::SizeType>(p->action.length()));
			}
			else if (packetData->packetType == PacketType::MODULE){
				ModulePacketData *p = reinterpret_cast<ModulePacketData*>(packetData);
				writer->Key(moduleKey);
				writer->Int(p->module);
				writer->Key(moduleActionKey);
				writer->String(p->action.c_str(), static_cast<rapidjson::SizeType>(p->action.length()));

			}

			if (packetData->data) {
				writer->Key(dataKey);
				GenericValue::write(writer, packetData->data.get());
			}

			writer->EndObject();
		}
		writer->EndArray();

		std::string json = sb.GetString();

		delete (writer);

		return json;
	}

}
}