//
// Created by hacketo on 09/08/16.
//

#ifndef SERVER_PACKET_H
#define SERVER_PACKET_H

#include <string>
#include <vector>
#include "server/config.h"
#include "rapidjson/document.h"
#include "rapidjson/allocators.h"
#include "frame.h"
#include "util/genericvalue.h"
#include "server/errors.h"

namespace protocol {
namespace packet{

	enum ParserType {
		NO, JSON
	};
	enum PacketType{
		DATA, ACTION, MODULE
	};


	class PacketData {
	public:
		typedef std::unique_ptr<PacketData> u_ptr;
		bool consumed;
		char packetType;
		GenericValue::u_ptr data;
		PacketData(GenericValue* value);
		PacketData();
	};

	class ActionPacketData : public PacketData {
	public:
		std::string action;
		ActionPacketData(const std::string action,GenericValue* value);
		ActionPacketData(GenericValue* value);
		ActionPacketData();
	};
	class ModulePacketData : public ActionPacketData{
	public:
		uint64_t module;
		ModulePacketData(uint64_t  moduleId, const std::string action, GenericValue* value);
		ModulePacketData(GenericValue* value);
		ModulePacketData();
	};

	typedef std::vector<PacketData::u_ptr>::iterator packet_iterator;


	/**
     * Un packet est une structure du donnée
     * Elle est composée d'une action et de données pour cette action
     * Un packet peut être composé de plusieurs packets
     *
     * Elle utilise PacketStructure pour stocker ces données
     */
	class Packet : public frame::FrameInterface {

	public:
		typedef std::unique_ptr<Packet> u_ptr;

		Packet();
		Packet(PacketData* data);
		Packet(GenericValue* value);
		Packet(const std::string action, GenericValue* value = new NullValue());

#ifdef USE_MODULES
		Packet(uint64_t  moduleId, const std::string action, GenericValue* value);
#endif
		~Packet();

		/**
		 * Ajoute un Packet structure à la liste des packet
		 * @param action
		 * @param data
		 */
		void addPacket(PacketData* data);


		/**
		 * Récupère les données pour le packet correspondant à l'index
		 * @param index
		 * @return
		 */
		PacketData* get(unsigned long index);


		packet_iterator begin() { return packets.begin(); }

		packet_iterator end() { return packets.end(); }

		/**
		 * Implémentation pour pouvoir envoyer ce packet aux clients
		 * Retourne une structure contenant la représentation json du packet
		 * @return
		 */
		virtual frame::Frame* getFrame() override;

	private:
		/**
		 * Liste des packet que contient ce packet
		 */
		std::vector<PacketData::u_ptr> packets;
	};





	/**
	 * Récupère la représentation json des données
	 * @param prettyprint
	 * @return
	 */
	std::string toJson(Packet* p, bool prettyprint = false);

	/**
	 * Récupère un packet parsé depuis du json
	 * @param json
	 * @return
	 */
	void parse(Packet* packet, std::string& valueToParse, errors::error& error);
	void parseFromJSON(Packet* packet, rapidjson::Value* object, errors::error& error);
	void parseFromJSON(Packet* packet, std::string& json, errors::error& error);

}
}

#endif //SERVER_PACKET_H
