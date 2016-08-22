#ifndef WS_SERVER_WS_FRAME_H
#define WS_SERVER_WS_FRAME_H

#include <algorithm>
#include <string>

#include "constant.h"
#include "util/exception.h"
#include "util/algo.h"

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

	struct FrameBuffer {
		uint32_t bufferSize = constant::max_buffer_size;
		uint8_t buffer[constant::max_buffer_size];
		uint32_t _id;
		uint8_t opcode;
	};

	class Frame {
	public:

		~Frame();

		/** True si la frame est raw venant du client */
		bool encoded = false;

		/** False si la frame n'est pas valide */
		bool valid = false;
		bool fin = false;

		/** Op code de la frame */
		uint8_t op_code = 0;

		/** Payload décodé */
		std::string msg;

		uint32_t bufferSize;
		uint8_t* buffer;

		/** @todo:Erreur de frame ? */
		int flags = 0;
	};

	Frame* from_string(std::string& msg);

	FrameBuffer from_uint8_t(uint8_t *data, uint16_t s, uint16_t id);

	uint8_t get_opcode(FrameBuffer* buffer);
	uint32_t get_framelen(FrameBuffer* buffer);

	/**
	 * Parse le buffer du holder dans une nouvelle Frame
	 * @param holder
	 * @return
	 */
	void decode(Frame *frame, uint8_t *buffer, uint32_t bufferSize);


	/**
	 * Encode un message
	 * Fonctionne uniquement avec un frame de type
	 * @return
	 */
	void encode(Frame *frame);

	uint32_t get_expected_frame_size(uint32_t s);

	class FrameInterface {
	public:
		virtual Frame* getFrame() { throw new NotImplementedException(); };
	};
}

}


#endif //WS_SERVER_WS_FRAME_H
