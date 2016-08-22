#ifndef WS_SERVER_WS_FRAME_H
#define WS_SERVER_WS_FRAME_H

#include <algorithm>
#include <string>

#include "constant.h"
#include "util/exception.h"
#include "util/string.h"

/**
 *       0                   1                   2                   3
      0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
     +-+-+-+-+-------+-+-------------+-------------------------------+
     |F|R|R|R| opcode|M| Payload len |    Extended payload length    |
     |I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
     |N|V|V|V|       |S|             |   (if payload len==126/127)   |
     | |1|2|3|       |K|             |                               |
     +-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
     |     Extended payload length continued, if payload len == 127  |
     + - - - - - - - - - - - - - - - +-------------------------------+
     |                               |Masking-key, if MASK set to 1  |
     +-------------------------------+-------------------------------+
     | Masking-key (continued)       |          Payload Data         |
     +-------------------------------- - - - - - - - - - - - - - - - +
     :                     Payload Data continued ...                :
     + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
     |                     Payload Data continued ...                |
     +---------------------------------------------------------------+
 */

namespace protocol {
namespace frame {
	enum Mask {
		OPCODE = 0x0F,
		RESERVED3 = 0x10,
		RESERVED2 = 0x20,
		RESERVED1 = 0x40,
		FIN = 0x80,
		LEN_PAYLOAD = 0x7F,
		LEN_MASK = 0x80
	};

	struct Frame {
		/** True si la frame est raw venant du client */
		bool encoded = false;

		/** False si la frame n'est pas valide */
		bool valid = false;
		bool fin = false;

		/** Op code de la frame */
		uint8_t op_code = 0;

		/** Payload décodé */
		std::string msg;

		uint16_t bufferSize = constant::max_buffer_size;
		uint8_t buffer[constant::max_buffer_size];

		/** @todo:Erreur de frame ? */
		int flags = 0;
	};

	Frame from_string(std::string msg);

	Frame from_uint8_t(uint8_t *data, uint16_t s);

	/**
	 * Parse le buffer du holder dans une nouvelle Frame
	 * @param holder
	 * @return
	 */
	void decode(Frame *frame);


	/**
	 * Encode un message
	 * Fonctionne uniquement avec un frame de type
	 * @return
	 */
	void encode(Frame *frame);


	class FrameInterface {
	public:
		virtual Frame getFrame() { throw new NotImplementedException(); };
	};
}

}


#endif //WS_SERVER_WS_FRAME_H
