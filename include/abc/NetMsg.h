#pragma once

// abc::NetMsg - minimal user-message reader shared by the plugins.
// Promoted from AbcEssential src/modules/AutoFunc/MsgReader.h (Phase 2 audit):
// framing semantics per ABCEnchance src/core/library/NetworkMessageReader.* -
// little-endian, readCoord = int32 * (1/8). A truncated read flags error()
// and the caller must fall back to the original message handler. AbcUi's
// NetworkMessages data layer (plan 05 §4) extends this reader instead of
// forking a second one.

#ifndef ABC_NETMSG_H
#define ABC_NETMSG_H

#include <cstdint>
#include <cstring>
#include <string>

namespace abc {

class MsgReader {
public:
	MsgReader(const void* buffer, int size)
		: Buffer(static_cast<const uint8_t*>(buffer)), Size(size) {
		if (Size < 0 || (!Buffer && Size > 0))
			Error = true;
	}

	bool ok() const { return !Error; }
	int remaining() const { return !Error && Pos <= Size ? Size - Pos : 0; }

	int8_t readChar() {
		if (!canRead(1)) {
			Error = true;
			return 0;
		}
		return static_cast<int8_t>(Buffer[Pos++]);
	}

	uint8_t readByte() {
		if (!canRead(1)) {
			Error = true;
			return 0;
		}
		return Buffer[Pos++];
	}

	int16_t readShort() {
		if (!canRead(2)) {
			Error = true;
			return 0;
		}
		int16_t value = static_cast<int16_t>(Buffer[Pos] | (Buffer[Pos + 1] << 8));
		Pos += 2;
		return value;
	}

	int32_t readLong() {
		if (!canRead(4)) {
			Error = true;
			return 0;
		}
		int32_t value = Buffer[Pos] | (Buffer[Pos + 1] << 8) |
			(Buffer[Pos + 2] << 16) | (static_cast<int32_t>(Buffer[Pos + 3]) << 24);
		Pos += 4;
		return value;
	}

	float readFloat() {
		if (!canRead(4)) {
			Error = true;
			return 0;
		}
		uint32_t bytes = static_cast<uint32_t>(Buffer[Pos] | (Buffer[Pos + 1] << 8) |
			(Buffer[Pos + 2] << 16) | (Buffer[Pos + 3] << 24));
		Pos += 4;
		float out;
		static_assert(sizeof(out) == sizeof(bytes), "float size mismatch");
		memcpy(&out, &bytes, sizeof(out));
		return out;
	}

	std::string readString() {
		std::string result;
		while (Pos < Size) {
			char c = static_cast<char>(readByte());
			if (!ok())
				return result;
			if (c == '\0')
				return result;
			result += c;
		}
		Error = true; // Every string, including an empty one, needs a NUL.
		return result;
	}

	float readCoord() {
		int32_t value = readLong();
		if (!ok())
			return 0;
		return static_cast<float>(value) * (1.0f / 8.0f);
	}

private:
	bool canRead(int count) const {
		return !Error && Buffer && Pos <= Size && count <= Size - Pos;
	}
	const uint8_t* Buffer = nullptr;
	int Size = 0;
	int Pos = 0;
	bool Error = false;
};

}

#endif //ABC_NETMSG_H
