//
// Created by hacketo on 14/08/16.
//

#include "protocol/frame.h"
#include "protocol/opcode.h"

namespace protocol{
frame::Frame frame::from_string(std::string msg) {
		frame::Frame f;
	f.encoded = false;
	f.op_code = opcode::TEXT;
	f.fin = true;
	f.msg = msg;

	return f;
}

frame::Frame frame::from_uint8_t(uint8_t *data, uint16_t s) {
	frame::Frame f;
	f.encoded = true;
	f.bufferSize = std::min<uint16_t>(s, constant::max_buffer_size);
	std::copy(data, data + f.bufferSize, f.buffer);

	return f;
}

/**
 * Parse le buffer du holder dans une nouvelle Frame
 * @param holder
 * @return
 */
void frame::decode(frame::Frame *frame) {
	if (frame->encoded) {
		uint8_t *data = frame->buffer;

		uint8_t op_code = data[0] & Mask::OPCODE;
		int payload_len = data[1] & Mask::LEN_PAYLOAD;
		int first_mask_index = 2;

		frame->op_code = op_code;

		//log_data("frame", rawdata, data_len);

		if (payload_len == 0x7E) { // size 16 bit
			first_mask_index = 4;
			payload_len = (data[2] << 8) + data[3];
		} else if (payload_len == 0x7F) { // size 64 bit
			first_mask_index = 10;
			payload_len = (data[7] << 16) + (data[8] << 8) + data[9];
		}

		bool opcodeis_valid = opcode::is_valid(op_code);

		if (opcodeis_valid) {
			frame->valid = true;
			//DEBUG_PRINT("LENGTH : ", payload_len);
			//DEBUG_PRINT("INDEX_MASK : ", first_mask_index);


			uint8_t mask[4];
			std::copy(data + first_mask_index, data + first_mask_index + 4, mask);

			if (opcode::is_control(op_code)) {

				if (op_code == opcode::CLOSE) {
					//DEBUG_PRINT("CLOSE_FRAME");
				} else if (op_code == opcode::PONG) {

				}
			} else {

				if (op_code == opcode::TEXT) {
					//DEBUG_PRINT("TEXT_FRAME");
				} else if (op_code == opcode::BINARY) {
					//DEBUG_PRINT("BINARY_FRAME");
				} else if (op_code == opcode::CONTINUATION) {
					//DEBUG_PRINT("CONTINUATION_FRAME");
				}
			}

			if (payload_len > 0) {
				uint8_t decodedPayload[payload_len];
				for (int i = first_mask_index + 4, j = 0; j < payload_len; i++, j++) {
					decodedPayload[j] = data[i] ^ mask[j % 4];
				}
				frame->msg = std::string(decodedPayload, decodedPayload + payload_len);
				//DEBUG_PRINT("Parsed payload : ", frame->msg);
			}
		}
	}
}


/**
 * Encode un message
 * Fonctionne uniquement avec un frame de type
 * @return
 */
void frame::encode(frame::Frame *frame) {
	if (!frame->encoded) {

		uint8_t op_code = 0x0;

		if (frame->fin) {
			op_code |= Mask::FIN;
		}

		op_code |= (frame->op_code & Mask::OPCODE);

		bool from_string = false;

		size_t dataBufferSize = frame->bufferSize;
		if (frame->msg.size() > 0) {
			dataBufferSize = frame->msg.size();
			from_string = true;
		}


		uint8_t buffer[dataBufferSize];
		if (from_string) {
			string::convert(frame->msg, buffer, dataBufferSize);
		} else {
			std::copy(frame->buffer, frame->buffer + dataBufferSize, buffer);
		}


		// Text frame
		frame->buffer[0] = op_code;

		uint16_t bufferSize = 2;

		if (dataBufferSize < 0x7E) {
			frame->buffer[1] = dataBufferSize;
		} else if (dataBufferSize < 65536) {
			frame->buffer[1] = 0x7E;
			frame->buffer[2] = (dataBufferSize >> 8) & 0xFF;
			frame->buffer[3] = dataBufferSize & 0xFF;
			bufferSize = 4;
		} else {
			frame->buffer[1] = 0x7F;
			frame->buffer[2] = (dataBufferSize >> 56) & 0xFF;
			frame->buffer[3] = (dataBufferSize >> 48) & 0xFF;
			frame->buffer[4] = (dataBufferSize >> 40) & 0xFF;
			frame->buffer[5] = (dataBufferSize >> 32) & 0xFF;
			frame->buffer[6] = (dataBufferSize >> 24) & 0xFF;
			frame->buffer[7] = (dataBufferSize >> 16) & 0xFF;
			frame->buffer[8] = (dataBufferSize >> 8) & 0xFF;
			frame->buffer[9] = dataBufferSize & 0xFF;

			bufferSize = 10;
		}

		for (int i = 0; i < dataBufferSize; i++, bufferSize++) {
			frame->buffer[bufferSize] = buffer[i];
		}

		frame->bufferSize = bufferSize;
	}
}
}