/**
 * @description Header file for XTEA encryption algorithm  for interaction with Telekom Cloud of Things.
 *   Is a reproduction of similar implementation in Java.
 * @copyright (C) 2018 Deutsche Telekom AG- all rights reserved.
 * @licence MIT licence
 */

#ifndef DT_ENCRYPTIONHELPER_H_
#define DT_ENCRYPTIONHELPER_H_

#include <cot_sdk/DTCotSdkMqtt.h>
#include "utility/DTCoTDebugOutput.h"
#include "DTCotSdkTypes.h"


using namespace DTCoT;

/**
 * An XTEA engine.
 */
class XTEAEngine  // implements BlockCipher
{
private:

    static const long rounds = 32;
    static const long block_size = 8;
    static const long delta = 0x9E3779B9;

    /*
     * the expanded key array of 4 subkeys
     */
    long _KEY[4];
    long _sum0[32];
    long _sum1[32];

    boolean _initialized;
    boolean _forEncryption;

    /**
     * Create an instance of the TEA encryption algorithm
     * and set some defaults
     */
public:

    XTEAEngine();

    String getAlgorithmName();

    long getBlockSize();

    /**
     * initEngine
     *
     * @param forEncryption Whether or not we are for encryption.
     * @param tempKey The key required to set up the cipher.
     * @param keyLen Length of the preceding key.
     */
    void initEngine( boolean forEncryption, const char* tempKey, size_t keyLen);

    /**
     * Encrypt/decrypt a block of block_size bytes from first to second buffer,
     *  depending on previous set direction.
     */
    long processBlock( char* in, long inOffset, char* out, long outOffset);

    void reset() { }

private:

    /**
     * Re-key the cipher.
     * @param  key  The key to be used.
     * @param  keyLen Length of the preceding key.
     */
    void setKey( const char* key, size_t keyLen);

    /**
     * Encrypt a block of block_size bytes from first to second buffer.
     */
    long encryptBlock( char* in, long inOffset, char* out, long outOffset);

    /**
     * Decrypt a block of block_size bytes from first to second buffer.
     */
    long decryptBlock( char* in, long inOffset, char* out, long outOffset);

    /**
     * Transform byte array to 4 bytes long (signed) integer.
     */
    long bytesToInt(const char* in, long inOffset);

    /**
     * Transform 4 bytes long (signed) integer to byte array.
     */
    void unpackInt(long v, char* out, long outOffset);

};



/**
 * The class that prepares the input key and the data to be encrypted to be
 * processed by the XTEA (Extended Tiny Encryption Algorithm) provided by the
 * Bouncy Castle library. Does the required type conversions. Performs the
 * padding of the data then proceeds to the encryption/decryption.
 * We use ECB mode (i.e. no block chaining), because the text we need to
 * encrypt is so short, that it will not have duplicate blocks anyway.
 */
class EncryptionHelper {

private:

    static const int BLOCK_SIZE = 8;

public:

    EncryptionHelper();

    /**
     * The method that receives the data to decrypt in an byte array format.
     * Encrypts the data and returns the encrypted data/password.
     *
     * @param key The key, used to encrypt. Should be 16 bytes long (!)
     *    and identical to the key, that is used for the decryption.
     * @param data Data/password to be encrypted.
     * @param dataLen Length of the preceding data.
     * @param encryptedData Buffer for encrypted data/password. Should be initialized with 0.
     */
    boolean encryptPassword( const char* key, const char* data, size_t dataLen, char* encryptedData);

    /**
     * The method that receives the encrypted data in an byte array format. Decrypts
     * the data. Removes the additional bytes due to padding and returns the decrypted data.
     *
     * @param key The key, used to decrypt. Should be 16 bytes long (!)
     *    and identical to the key, that is used for the encryption.
     * @param encryptedData Data/password to be decrypted.
     * @param dataLen Length of the preceding encryptedData.
     * @param decryptedData Buffer for decrypted data/password. Should be initialized with 0.
     */
    boolean decryptPassword( const char* key, const char* encryptedData, size_t dataLen, char* decryptedData);

private:

    /**
     * Asserts that the given data has a valid block size.
     *
     * Size *must* be:
     *
     * - a multiple of 8
     * - at least 8
     *
     * @param dataLen The length of the encrypted data.
     */
    boolean assertValidBlockSize( size_t dataLen);


    /**
     * Asserts that the given byte sequence has a valid padding according to the PKCS7.
     *
     * - A padding *must* exist
     * - Padding bytes are in range [1..8]
     * - The value of the padding bytes equals the number of padded bytes
     * - All padding bytes must have the same value
     *
     * @param decryptedData The padded data.
     * @param dataLen The length of the preceding decryptedData array.
     */
    boolean assertValidPadding( const char* decryptedData, size_t dataLen);


    /**
     * With PKCS7-padding, the number of padding bytes equals the value of the padding bytes.
     *
     * @param decryptedData The padded data array.
     * @param dataLen The length of the preceding decryptedData array.
     * @return The padding length in range [1..8].
     */
    long getPaddingLength(const char* decryptedData, size_t dataLen);

};


#endif /* DT_ENCRYPTIONHELPER_H_ */
