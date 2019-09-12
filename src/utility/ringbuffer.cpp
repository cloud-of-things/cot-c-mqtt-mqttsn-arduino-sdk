/**
 * @file ringbuffer.cpp
 * @description Ring buffer abstraction functions.
 * @author mm1 Technology GmbH
 * @copyright (C) 2018 mm1 Technology GmbH
 * @licence MIT licence
 *
 * Created on: 14.03.2018
 */


#include "ringbuffer.h"
#include "utility/DTCoTDebugOutput.h"


/*rx FIFO ringbuffer implementation*/
#define NB_RINGBUFF_SZ (256)
volatile char nbRingBuff[NB_RINGBUFF_SZ + 1];
volatile int  rxPosStart = 0;
volatile int  rxPosEnd = 0;


/**
 * @brief Initialises the ring buffer indices.
 */
void NbRingBuffReset(void)
{
	rxPosStart = 0;
	rxPosEnd = 0;
	/*make sure to have terminating zeros...*/
	nbRingBuff[rxPosStart] = 0;
	nbRingBuff[NB_RINGBUFF_SZ] = 0;
}


/**
 * @brief Returns the number of valid characters available in the ring buffer.
 * @returns The number of valid characters available in the ring buffer.
 */
int NbRingBuffIsAvailable(void)
{
	int diff = rxPosEnd - rxPosStart;

	if(diff < 0)
	{
		return (NB_RINGBUFF_SZ + diff);
	}
	else
	{
		return diff;
	}
}



/**
 * @brief Checks the ring buffer is overflown.
 * @returns true for an overflow condition, false otherwise.
 */
bool NbRingBuffIsOverflow(void)
{
	return (rxPosStart == ((rxPosEnd + 1) % NB_RINGBUFF_SZ));
}



/**
 * @brief Writes a character into the ring buffer.
 * @param c The character that shall be written.
 * @returns true if the write operation was successful, false otherwise (i.e. on overflow).
 */
bool NbRingBuffWriteChar(char c)
{
	int nextRxPosEnd = (rxPosEnd + 1) % NB_RINGBUFF_SZ;

	if(rxPosStart == nextRxPosEnd)
	{
		/*overflow condition detected. Will not write this character anymore.*/
		return false;
	}
	else
	{
		nbRingBuff[rxPosEnd] = c;
		nbRingBuff[nextRxPosEnd] = 0;
		rxPosEnd = nextRxPosEnd;
		return true;
	}
}




/**
 * @brief Reads and removes the oldest character from the ring buffer.
 * @returns The oldest character pending in the ring buffer, -1 if the buffer was empty.
 */
char NbRingBuffReadChar(void)
{
	char retval = -1;

	if(NbRingBuffIsAvailable() > 0)
	{
		retval = nbRingBuff[rxPosStart];
		rxPosStart = (rxPosStart + 1) % NB_RINGBUFF_SZ;
	}

	return retval;
}



/**
 * @brief Reads and removes all pending characters from the ring buffer and adds
 * them at the end of the given String object.
 * @param Points to the String object that will be extended. The read characters
 * will be added at the end of the string and in order of reception.
 * @returns Number of characters added to the string.
 */
int NbRingBuffReadString(String &retStr)
{
	int numC = NbRingBuffPeekString(retStr);
	return NbRingBuffRemoveChars(numC);


#if 0 /*works, but slow*/
	int numC = 0;

	while(NbRingBuffIsAvailable() > 0)
	{
		numC++;
		retStr += NbRingBuffReadChar();
	}

	return numC;
#endif /**/
}



/**
 * @brief Removes a given number of characters (or less) from the ringbuffer.
 * @param numCharacters Maximum number of characters to be removed from the
 * ringbuffer.
 * @returns The Number of characters that actually have been removed from the
 * ringbuffer. May be less than requested, if there are less characters stored
 * in the buffer than should be deleted.
 */
int NbRingBuffRemoveChars(int numCharacters)
{
	int numAvailable = NbRingBuffIsAvailable();

	if(numCharacters >= numAvailable)
	{
		NbRingBuffReset();
		return numAvailable;
	}
	else
	{
		rxPosStart = (rxPosStart + numCharacters) % NB_RINGBUFF_SZ;
		return numCharacters;
	}
}



/**
 * @brief Reads all pending characters from the ring buffer and adds them at the
 * end of the given String object. The content of the ring buffer will not be
 * modified by this operation.
 * @param Points to the String object that will be extended. The read characters
 * will be added at the end of the string and in order of reception.
 * @returns Number of characters added to the string.
 */
int NbRingBuffPeekString(String &retStr)
{
	int diff = rxPosEnd - rxPosStart;

	retStr += String((char*)&(nbRingBuff[rxPosStart]));

	if(diff < 0)
	{
		/*read until wraparound first (only if applicable)*/
		retStr += String((char*)nbRingBuff);
	}

	if(retStr.length() != NbRingBuffIsAvailable())
	{
		DT_DEBUG_PRINTLN_INFO("================================================");
		DT_DEBUG_PRINTLN_INFO("ERROR: Calculated bad ringbuffer string length.");
		DT_DEBUG_PRINTLN_INFO("       Resetting ring buffer.");
		DT_DEBUG_PRINTLN_INFO("================================================");
		NbRingBuffReset();
		// while(1);
	}

	return retStr.length();


#if 0 /*works, but slow*/
	int i = rxPosStart;

	/*read until wraparound (only if applicable)*/
	while((i > rxPosEnd) && (i < NB_RINGBUFF_SZ))
	{
		retStr += nbRingBuff[i++];
	}
	i %= NB_RINGBUFF_SZ;

	/*keep on reading until end of received characters*/
	while(i < rxPosEnd)
	{
		retStr += nbRingBuff[i++];
	}

	return NbRingBuffIsAvailable();
#endif /*0*/
}


