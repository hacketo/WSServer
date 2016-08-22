//
// Created by hacketo on 12/08/16.
//

#include <cstdint>
#include "protocol/opcode.h"

namespace protocol {

	bool opcode::is_valid(uint8_t opcode) {
		return opcode <= 0xF && opcode >= 0;
	}

	bool opcode::is_control(uint8_t c){
		return c >= CLOSE;
	}

}