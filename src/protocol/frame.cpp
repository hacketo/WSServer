//
// Created by hacketo on 14/08/16.
//

#include <debug.h>
#include <assert.h>
#include "protocol/frame.h"
#include "protocol/opcode.h"

namespace protocol {

	frame::Frame::~Frame() {
		delete[] buffer;
	}

	frame::Frame* frame::from_string(std::string& msg) {
		frame::Frame* f = new frame::Frame;
		f->encoded = false;
		f->op_code = opcode::TEXT;
		f->fin = true;
		f->bufferSize = msg.length();
		f->buffer = new uint8_t[f->bufferSize];
		string::convert(msg, f->buffer, f->bufferSize);
		return f;
	}

	frame::FrameBuffer frame::from_uint8_t(uint8_t *data, uint16_t s, uint16_t id) {
		frame::FrameBuffer f;
		f._id = id;
		f.bufferSize = s;
		std::copy(data, data + f.bufferSize, f.buffer);
		return f;
	}

	uint8_t frame::get_opcode(FrameBuffer *buffer) {
		assert(buffer->bufferSize > 0);
		return buffer->buffer[0] & Mask::OPCODE;
	}

	uint32_t frame::get_framelen(FrameBuffer *buffer) {
		assert(buffer->bufferSize > 1);
		int payload_len = buffer->buffer[1] & Mask::LEN_PAYLOAD;
		int rlen = payload_len + 2;
		if (payload_len == 0x7E) { // size 16 bit
			rlen = (buffer->buffer[2] << 8) + buffer->buffer[3] + 4;
		} else if (payload_len == 0x7F) { // size 64 bit
			rlen = (buffer->buffer[7] << 16) + (buffer->buffer[8] << 8) + buffer->buffer[9] + 10;
		}
		return rlen + 4;
	}

	/**
	 * Parse le buffer du holder dans une nouvelle Frame
	 * @param holder
	 * @return
	 */
	void frame::decode(Frame *frame, uint8_t *buffer, uint32_t bufferSize) {

		frame->buffer = buffer;
		frame->bufferSize = bufferSize;


		uint8_t op_code = buffer[0] & Mask::OPCODE;
		int payload_len = buffer[1] & Mask::LEN_PAYLOAD;
		int first_mask_index = 2;

		frame->op_code = op_code;

		//log_data("frame", rawdata, data_len);

		if (payload_len == 0x7E) { // size 16 bit
			first_mask_index = 4;
			payload_len = (buffer[2] << 8) + buffer[3];
		} else if (payload_len == 0x7F) { // size 64 bit
			first_mask_index = 10;
			payload_len = (buffer[7] << 16) + (buffer[8] << 8) + buffer[9];
		}

		bool opcodeis_valid = opcode::is_valid(op_code);

		if (opcodeis_valid) {
			frame->valid = true;
			DEBUG_PRINT("LENGTH : ", payload_len);
			DEBUG_PRINT("INDEX_MASK : ", first_mask_index);


			uint8_t mask[4];
			std::copy(buffer + first_mask_index, buffer + first_mask_index + 4, mask);

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
					decodedPayload[j] = buffer[i] ^ mask[j % 4];
				}
				frame->msg = std::string(decodedPayload, decodedPayload + payload_len);
				//DEBUG_PRINT("Parsed payload : ", frame->msg);
			}
		}

	}

	uint32_t frame::get_expected_frame_size(uint32_t s) {
		if (s < 0x7E) {
			return s + 2;
		} else if (s < 65536) {
			return s + 4;
		} else {
			return s + 10;
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


			uint32_t dataBufferSize = frame->bufferSize;

			uint32_t expectedFrameSize = get_expected_frame_size(dataBufferSize);

			uint8_t *oldBuffer = frame->buffer;

			frame->buffer = new uint8_t[expectedFrameSize];

			// Text frame
			frame->buffer[0] = op_code;

			uint32_t bufferIndex = 2;

			if (dataBufferSize < 0x7E) {
				frame->buffer[1] = dataBufferSize;
			} else if (dataBufferSize < 65536) {
				frame->buffer[1] = 0x7E;
				frame->buffer[2] = (dataBufferSize >> 8) & 0xFF;
				frame->buffer[3] = dataBufferSize & 0xFF;
				bufferIndex = 4;
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

				bufferIndex = 10;
			}

			for (int i = 0; i < dataBufferSize; i++, bufferIndex++) {
				frame->buffer[bufferIndex] = oldBuffer[i];
			}
			DEBUG_PRINT("Buffer Filled");
			frame->bufferSize = bufferIndex;

			frame->encoded = true;

			delete[] oldBuffer;
		}
	}
}