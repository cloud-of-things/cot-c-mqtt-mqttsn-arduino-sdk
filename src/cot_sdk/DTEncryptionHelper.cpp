/**
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved.
 * @licence MIT licence
 */

/*FIXME Get rid of this workaround!*/
#ifndef ARDUINO_ARCH_AVR

#include <cot_sdk/DTEncryptionHelper.h>

using namespace DTCoT;

////// XTEAEngine:: public methods:

XTEAEngine::XTEAEngine() {
	_initialized = false;
	_forEncryption = false;
}

String XTEAEngine::getAlgorithmName() {
	return "XTEA";
}

long XTEAEngine::getBlockSize() {
	return block_size;
}

void XTEAEngine::initEngine(boolean forEncryption, const char* tempKey,
		size_t keyLen) {
//    DT_DEBUG_PRINT(
//            "XTEAEngine::initEngine() forEncryption<%d> tempKey<%s> keyLen<%d>",
//            forEncryption, tempKey, keyLen);

	_forEncryption = forEncryption;
	_initialized = true;

	// Simple assign given char/byte array
	setKey(tempKey, keyLen);
}

long XTEAEngine::processBlock(char* in, long inOffset, char* out,
		long outOffset) {
//    DT_DEBUG_PRINT("XTEAEngine::processBlock() inOffset<%d> outOffset<%d>",
//            inOffset, outOffset);

	if (!_initialized) {
		DT_DEBUG_PRINTLN_ERROR("XTEAEngine::processBlock() NOT initialized.");
		return 0;
	}

	return (_forEncryption) ?
			encryptBlock(in, inOffset, out, outOffset) :
			decryptBlock(in, inOffset, out, outOffset);
}

///// XTEAEngine:: private methods:

void XTEAEngine::setKey(const char* key, size_t keyLen) {
//    DT_DEBUG_PRINT("XTEAEngine::setKey() key<%s> keyLen<%d>", key, keyLen);

	if (keyLen != 16) {
		DT_DEBUG_PRINTLN_ERROR(
				"XTEAEngine::setKey() Error! Key size must be 16 Bytes, but is <%d>",
				keyLen);
		return;
	}

	// Use  unsigned long instead of long:
	unsigned long i, j;
	for (i = j = 0; i < 4; i++, j += 4) {
		_KEY[i] = bytesToInt(key, j);
//        DT_DEBUG_PRINT_INFO("XTEAEngine::setKey() _KEY[%d]<0x%x>", i, _KEY[i]);
	}

	// Help IDs:
	uint32_t keyId0, keyId1;

	for (i = j = 0; i < rounds; i++) {
		keyId0 = j & 3;
		_sum0[i] = (j + _KEY[keyId0]);
		// DT_DEBUG_PRINT("::setKey(), keyId0: %d, _sum0[%d]: 0x%x", keyId0, i, _sum0[i]);

		j += delta;

		keyId1 = (j >> 11) & 3;
		_sum1[i] = (j + _KEY[keyId1]);
		// DT_DEBUG_PRINT("::setKey(), keyId1: %d, _sum1[%d]: 0x%x", keyId1, i, _sum1[i]);
	}
}

long XTEAEngine::encryptBlock(char* in, long inOffset, char* out,
		long outOffset) {
//    DT_DEBUG_PRINT("XTEAEngine::encryptBlock() inOffset<%d> outOffset<%d>",
//            inOffset, outOffset);

// Pack bytes into integers
	long v0 = bytesToInt(in, inOffset);
	long v1 = bytesToInt(in, inOffset + 4);

	// Help values:
	int32_t helpV0, helpV1;
	for (int i = 0; i < rounds; i++) {
		helpV1 = (v1 >> 5) & 0x7FFFFFF;
		v0 += ((v1 << 4 ^ helpV1) + v1) ^ _sum0[i];
		helpV0 = (v0 >> 5) & 0x7FFFFFF;
		v1 += ((v0 << 4 ^ helpV0) + v0) ^ _sum1[i];
	}

	unpackInt(v0, out, outOffset);
	unpackInt(v1, out, outOffset + 4);

	return block_size;
}

long XTEAEngine::decryptBlock(char* in, long inOffset, char* out,
		long outOffset) {
//    DT_DEBUG_PRINT("XTEAEngine::decryptBlock() inOffset<%d> outOffset<%d>",
//            inOffset, outOffset);

// Pack bytes into integers
	long v0 = bytesToInt(in, inOffset);
//    DT_DEBUG_PRINT_INFO("XTEAEngine::decryptBlock() bytesToInt(v0) returns v0<0x%x>", v0);
	long v1 = bytesToInt(in, inOffset + 4);
//    DT_DEBUG_PRINT_INFO("XTEAEngine::decryptBlock() bytesToInt(v1) returns v1<0x%x>", v1);

// Help values:
	int32_t helpV0, helpV1;
	for (int i = rounds - 1; i >= 0; i--) {
		helpV0 = (v0 >> 5) & 0x7FFFFFF;
		v1 -= ((v0 << 4 ^ helpV0) + v0) ^ _sum1[i];

		helpV1 = (v1 >> 5) & 0x7FFFFFF;
		v0 -= ((v1 << 4 ^ helpV1) + v1) ^ _sum0[i];
	}

	unpackInt(v0, out, outOffset);
//    DT_DEBUG_PRINT_INFO("XTEAEngine::decryptBlock() unpackInt(v0) returns out<%s>", out);
	unpackInt(v1, out, outOffset + 4);
//    DT_DEBUG_PRINT_INFO("XTEAEngine::decryptBlock() unpackInt(v1) returns out<%s>", out);

	return block_size;
}

// Transform byte array to 4 bytes long (signed) integer.
long XTEAEngine::bytesToInt(const char* in, long inOffset) {
	// DT_DEBUG_PRINT("XTEAEngine::bytesToInt(), in: %s, inOffset: %d", in, inOffset);
	long o1 = inOffset++;
	long o2 = inOffset++;
	long o3 = inOffset++;
	return ((in[o1]) << 24) | ((in[o2] & 255) << 16) | ((in[o3] & 255) << 8)
			| ((in[inOffset] & 255));
}

// Transform 4 bytes long (signed) integer to byte array.
void XTEAEngine::unpackInt(long v, char* out, long outOffset) {
	// DT_DEBUG_PRINT("XTEAEngine::unpackInt(), v: 0x%x, outOffset: %d", v, outOffset);
	out[outOffset++] = (char) ((v >> 24) & 0xFF);
	out[outOffset++] = (char) ((v >> 16) & 0xFFFF);
	out[outOffset++] = (char) ((v >> 8) & 0xFFFFFF);
	out[outOffset] = (char) v;
}

////// EncryptionHelper:: public methods:

EncryptionHelper::EncryptionHelper() {
	// Auto-generated constructor stub
}

boolean EncryptionHelper::encryptPassword(const char* key, const char* data,
		size_t dataLen, char* encryptedData) {
//    DT_DEBUG_PRINT(
//            "EncryptionHelper::encryptPassword() key<%s> data<%s> dataLen<%d>...",
//            key, data, dataLen);

	if (!assertValidBlockSize(dataLen)) {
		return false;
	}

	XTEAEngine engine = XTEAEngine();
	engine.initEngine(true, key, 16);   // Assumpted Key-length of 16 bytes !

	// Make copy of received encrypted data.
	// Array length mod BLOCK_SIZE (8) MUST be always 0 to avoid errors ! =>  8/16/24/32 ...
	long cryptLen = (dataLen / BLOCK_SIZE) * BLOCK_SIZE;
	if (dataLen % BLOCK_SIZE != 0) {
		cryptLen += BLOCK_SIZE;
	}

	char cryptStr[cryptLen];
	memset(cryptStr, 0, cryptLen);
	memcpy(cryptStr, data, dataLen); // Copy full credentials length, not as String.

	for (long i = 0; i < (long) dataLen; i += BLOCK_SIZE) {
		engine.processBlock(cryptStr, i, cryptStr, i);
	}
//    DT_DEBUG_PRINT_INFO("*** ::encryptPassword(), GOT cryptStr: %s", cryptStr);

	memcpy(encryptedData, cryptStr, dataLen);
	// Set closing '\0' at string end.
	encryptedData[dataLen] = '\0';

//    DT_DEBUG_PRINT(
//            "EncryptionHelper::encryptPassword() key<%s> data<%s> dataLen<%d>... done. encryptedData<%s>",
//            key, data, dataLen, encryptedData);
	return true;
}

boolean EncryptionHelper::decryptPassword(const char* key,
		const char* encryptedData, size_t dataLen, char* decryptedData) {
//    DT_DEBUG_PRINT(
//            "EncryptionHelper::decryptPassword() key<%s> encryptedData<%s> dataLen<%d>...",
//            key, encryptedData, dataLen);

	if (!assertValidBlockSize(dataLen)) {
		DT_DEBUG_PRINTLN_ERROR(
				"EncryptionHelper::decryptPassword() key<%s> encryptedData<%s> dataLen<%d>... FAILED! No valid data length!",
				key, encryptedData, dataLen);
		return false;
	}

	XTEAEngine engine = XTEAEngine();
	engine.initEngine(false, key, 16);   // Assumpted Key-length of 16 !

	// Make copy of received encrypted data.
	// Array length mod BLOCK_SIZE (8) MUST be always 0 to avoid errors ! =>  8/16/24/32 ...
	long cryptLen = (dataLen / BLOCK_SIZE) * BLOCK_SIZE;
	if (dataLen % BLOCK_SIZE != 0) {
		cryptLen += BLOCK_SIZE;
	}

	char cryptStr[cryptLen + 1];    // Additional byte for 0 termination.
	memset(cryptStr, 0, sizeof(cryptStr));
	memcpy(cryptStr, encryptedData, dataLen); // Copy full credentials length, not as String.

	for (long i = 0; i < (long) dataLen; i += BLOCK_SIZE) {
		engine.processBlock(cryptStr, i, cryptStr, i);
	}

	// If padding is not OK, return.
	if (!assertValidPadding(cryptStr, dataLen)) {
		return false;
	}

	const long paddingLength = getPaddingLength(cryptStr, dataLen);

	memcpy(decryptedData, cryptStr, dataLen - paddingLength);
	// Set closing '\0' at string end.
	decryptedData[dataLen - paddingLength] = '\0';

//    DT_DEBUG_PRINT(
//            "EncryptionHelper::decryptPassword() key<%s> encryptedData<%s> dataLen<%d>... done. decryptedData<%s>",
//            key, encryptedData, dataLen, decryptedData);
	return true;
}

////// EncryptionHelper:: private methods:

boolean EncryptionHelper::assertValidBlockSize(size_t dataLen) {
	if (dataLen < BLOCK_SIZE) {
		DT_DEBUG_PRINTLN_ERROR(
				"EncryptionHelper::assertValidBlockSize() Encrypted data must consist of at least one block of size<%d>",
				BLOCK_SIZE);
		return false;
	}

	if (dataLen % BLOCK_SIZE != 0) {
		DT_DEBUG_PRINTLN_ERROR(
				"EncryptionHelper::assertValidBlockSize() Encrypted data is not padded, byte length must be a multiple of <%d>. Actual length <%d>",
				BLOCK_SIZE, dataLen);
		return false;
	}

	return true;
}

boolean EncryptionHelper::assertValidPadding(const char* decryptedData,
		size_t dataLen) {
//    DT_DEBUG_PRINT(
//            "EncryptionHelper::assertValidPadding() decryptedData<%s> dataLen<%d>...",
//            decryptedData, dataLen);

	const long paddingLength = getPaddingLength(decryptedData, dataLen);

	if (paddingLength < 1 || paddingLength > 8) {
		DT_DEBUG_PRINTLN_ERROR(
				"EncryptionHelper::assertValidPadding() paddingLength must be in range [1..8]. Actual value is<%d>",
				paddingLength);
		return false;
	}

	for (long i = 0; i < paddingLength; i++) {
		// Padding content must be equal padding length!
		if (decryptedData[dataLen - i - 1] == paddingLength) {
			// Everything ok, the padding byte has the expected value.
			continue;
		}

		// Display Error message:
		DT_DEBUG_PRINTLN_ERROR(
				"EncryptionHelper::assertValidPadding() Expected exactly <%d> padding bytes!",
				paddingLength);
		return false;
	}

//    DT_DEBUG_PRINT(
//            "EncryptionHelper::assertValidPadding() decryptedData<%s> dataLen<%d>... done.",
//            decryptedData, dataLen);
	return true;
}

long EncryptionHelper::getPaddingLength(const char* decryptedData,
		size_t dataLen) {

	return (long) decryptedData[dataLen - 1];
}

#endif /*ARDUINO_ARCH_AVR*/

