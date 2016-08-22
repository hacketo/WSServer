//
// Created by hacketo on 12/08/16.
//

#ifndef SERVER_OPCODE_H
#define SERVER_OPCODE_H

namespace protocol {
namespace opcode {
	enum code {
		CONTINUATION = 0x0,
		TEXT = 0x1,
		BINARY = 0x2,
		RESERVED3 = 0x3,
		RESERVED4 = 0x4,
		RESERVED5 = 0x5,
		RESERVED6 = 0x6,
		RESERVED7 = 0x7,
		CLOSE = 0x8,
		PING = 0x9,
		PONG = 0xA,
		RESERVEDB = 0xB,
		RESERVEDC = 0xC,
		RESERVEDD = 0xD,
		RESERVEDE = 0xE,
		RESERVEDF = 0xF
	};

	bool is_valid(uint8_t opcode);
	bool is_control(uint8_t c);
}
}

#endif //SERVER_OPCODE_H
