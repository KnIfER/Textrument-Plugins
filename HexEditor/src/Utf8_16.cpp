// Utf8_16.cxx
// Copyright (C) 2002 Scott Kirkwood
//
// Permission to use, copy, modify, distribute and sell this code
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all copies or
// any derived copies.  Scott Kirkwood makes no representations
// about the suitability of this software for any purpose.
// It is provided "as is" without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////
// 
// Modificated 2006 Jens Lorenz
// 
// - Clean up the sources
// - Removing UCS-Bug in Utf8_Iter
// - Add convert function in Utf8_16_Write
////////////////////////////////////////////////////////////////////////////////

#include "Utf8_16.h"

#include <stdio.h>
#include <windows.h>
#include "PluginInterface.h"

const Utf8_16::utf8 Utf8_16::k_Boms[][3] = {
	{0x00, 0x00, 0x00},  // Unknown
	{0xEF, 0xBB, 0xBF},  // UTF8
	{0xFE, 0xFF, 0x00},  // Big endian
	{0xFF, 0xFE, 0x00},  // Little endian
};


// ==================================================================

Utf8_16_Read::Utf8_16_Read() {
	m_eEncoding		= eUnknown;
	m_nBufSize		= 0;
	m_pNewBuf		= NULL;
	m_bFirstRead	= true;
}

Utf8_16_Read::~Utf8_16_Read()
{
	if ((m_eEncoding == eUtf16BigEndian) || (m_eEncoding == eUtf16LittleEndian))
    {
		delete [] m_pNewBuf;
		m_pNewBuf = NULL;
	}
}

void Utf8_16_Read::forceEncoding(Utf8_16::encodingType eType)
{
	m_eEncoding = eType;
}

int Utf8_16_Read::isUTF8_16()
{
	int rv=1;
	int ASCII7only=1;
	utf8 *sx	= (utf8 *)m_pBuf;
	utf8 *endx	= sx + m_nLen;

	while (sx<endx)
	{
		if (!*sx)
		{											// For detection, we'll say that NUL means not UTF8
			ASCII7only=0;
			rv=0;
			break;
		} 
		else if (*sx < 0x80)
		{			// 0nnnnnnn If the byte's first hex code begins with 0-7, it is an ASCII character.
			sx++;
		} 
		else if (*sx < (0x80 + 0x40)) 
		{											  // 10nnnnnn 8 through B cannot be first hex codes
			ASCII7only=0;
			rv=0;
			break;
		} 
		else if (*sx < (0x80 + 0x40 + 0x20))
		{					  // 110xxxvv 10nnnnnn  If it begins with C or D, it is an 11 bit character
			ASCII7only=0;
			if (sx>=endx-1) 
				break;
			if (!(*sx & 0x1F) || (sx[1]&(0x80+0x40)) != 0x80) {
				rv=0; break;
			}
			sx+=2;
		} 
		else if (*sx < (0x80 + 0x40 + 0x20 + 0x10))
		{								// 1110qqqq 10xxxxvv 10nnnnnn If it begins with E, it is 16 bit
			ASCII7only=0;
			if (sx>=endx-2) 
				break;
			if (!(*sx & 0xF) || (sx[1]&(0x80+0x40)) != 0x80 || (sx[2]&(0x80+0x40)) != 0x80) {
				rv=0; break;
			}
			sx+=3;
		} 
		else 
		{													  // more than 16 bits are not allowed here
			ASCII7only=0;
			rv=0;
			break;
		}
	}
	return(ASCII7only?0:rv);
}

size_t Utf8_16_Read::convert(char* buf, size_t len)
{
	// bugfix by Jens Lorenz
	static	size_t nSkip = 0;

    size_t  ret = 0;
    
	m_pBuf = (ubyte*)buf;
	m_nLen = len;

	if ((m_bFirstRead == true) && m_bIsBOM)
    {
		determineEncoding();
		nSkip = m_nSkip;
		m_bFirstRead = false;
	}

    switch (m_eEncoding)
    {
        case eUnknown:
        case eUtf8Plain: {
            // Do nothing, pass through
            m_nBufSize = 0;
            m_pNewBuf = m_pBuf;
            ret = len;
            break;
        }
        case eUtf8: {
            // Pass through after BOM
            m_nBufSize = 0;
            m_pNewBuf = m_pBuf + nSkip;
            ret = len - nSkip;
            break;
        }    
        case eUtf16BigEndian:
        case eUtf16LittleEndian: {
            size_t newSize = len + len / 2 + 1;
            
            if (m_nBufSize != newSize)
            {
				if (m_pNewBuf)
					delete [] m_pNewBuf;
                m_pNewBuf  = NULL;
                m_pNewBuf  = new ubyte[newSize];
                m_nBufSize = newSize;
            }
            
            ubyte* pCur = m_pNewBuf;
            
            m_Iter16.set(m_pBuf + nSkip, len - nSkip, m_eEncoding);

            for (; m_Iter16; ++m_Iter16)
            {
                *pCur++ = m_Iter16.get();
            }
            ret = pCur - m_pNewBuf;
            break;
        }
        default:
            break;
    }

	// necessary for second calls and more
	nSkip = 0;

	return ret;
}


void Utf8_16_Read::determineEncoding()
{
	m_eEncoding = eUnknown;
	m_nSkip = 0;

	if (m_nLen > 1)
    {
		if (m_pBuf[0] == k_Boms[eUtf16BigEndian][0] && m_pBuf[1] == k_Boms[eUtf16BigEndian][1]) {
			m_eEncoding = eUtf16BigEndian;
			m_nSkip = 2;
		} else if (m_pBuf[0] == k_Boms[eUtf16LittleEndian][0] && m_pBuf[1] == k_Boms[eUtf16LittleEndian][1]) {
			m_eEncoding = eUtf16LittleEndian;
			m_nSkip = 2;
		} else if (m_nLen > 2 && m_pBuf[0] == k_Boms[eUtf8][0] && m_pBuf[1] == k_Boms[eUtf8][1] && m_pBuf[2] == k_Boms[eUtf8][2]) {
			m_eEncoding = eUtf8;
			m_nSkip = 3;
		} else if (isUTF8_16()) {
			m_eEncoding = eUtf8Plain;
			m_nSkip = 0;
		}
	}
}


// ==================================================================

Utf8_16_Write::Utf8_16_Write()
{
	m_eEncoding = eUnknown;
	m_pFile = NULL;
	m_pBuf = NULL;
	m_pNewBuf = NULL;
	m_bFirstWrite = true;
	m_nBufSize = 0;
	m_bIsBOM = true;
}

Utf8_16_Write::~Utf8_16_Write()
{
	fclose();
}

FILE * Utf8_16_Write::fopen(const char *_name, const char *_type)
{
	m_pFile = ::fopen(_name, _type);

	m_bFirstWrite = true;

	return m_pFile;
}

size_t Utf8_16_Write::fwrite(const void* p, size_t _size)
{
    // no file open
	if (!m_pFile)
    {
		return 0;
	}

    size_t  ret = 0;
    
	if (m_bFirstWrite && m_bIsBOM)
    {
        switch (m_eEncoding)
        {
            case eUtf8: {
                ::fwrite(k_Boms[m_eEncoding], 3, 1, m_pFile);
                break;
            }    
            case eUtf16BigEndian:
            case eUtf16LittleEndian:
                ::fwrite(k_Boms[m_eEncoding], 2, 1, m_pFile);
                break;
            default:
                // nothing to do
                break;
        }
		m_bFirstWrite = false;
    }
    
    switch (m_eEncoding)
    {
        case eUnknown:
        case eUtf8Plain:
        case eUtf8: {
            // Normal write
            ret = ::fwrite(p, _size, 1, m_pFile);
            break;
        }
        case eUtf16BigEndian:
        case eUtf16LittleEndian: {
            if (_size > m_nBufSize)
            {
                m_nBufSize = _size;
				if (m_pBuf != NULL)
					delete [] m_pBuf;
                m_pBuf = NULL;
                m_pBuf = new utf16[_size + 1];
            }
            
            Utf8_Iter iter8;
            iter8.set(static_cast<const ubyte*>(p), _size, m_eEncoding);
            
            utf16* pCur = m_pBuf;
            
            for (; iter8; ++iter8) {
                if (iter8.canGet()) {
                    *pCur++ = iter8.get();
                }
            }
            ret = ::fwrite(m_pBuf, (const char*)pCur - (const char*)m_pBuf, 1, m_pFile);
            break;
        }    
        default:
            break;
    }
    
    return ret;
}


size_t Utf8_16_Write::convert(char* p, size_t _size)
{
	if (m_pNewBuf)
    {
		delete [] m_pNewBuf;
	}

    switch (m_eEncoding)
    {
        case eUnknown:
        case eUtf8Plain: {
            // Normal write
            m_nBufSize = _size;
            m_pNewBuf = (ubyte*)new ubyte[m_nBufSize];
            memcpy(m_pNewBuf, p, _size);
            break;
        }
        case eUtf8: {
			if (m_bIsBOM)
			{
				m_nBufSize = _size + 3;
				m_pNewBuf = (ubyte*)new ubyte[m_nBufSize];
				memcpy(m_pNewBuf, k_Boms[m_eEncoding], 3);
				memcpy(&m_pNewBuf[3], p, _size);
			}
			else
			{
				// Normal write
				m_nBufSize = _size;
				m_pNewBuf = (ubyte*)new ubyte[m_nBufSize];
				memcpy(m_pNewBuf, p, _size);
			}
            break;
        }
        case eUtf16BigEndian:
        case eUtf16LittleEndian: {
            m_pNewBuf = (ubyte*)new ubyte[sizeof(utf16) * (_size + 1)];
            
            if (m_bIsBOM && (m_eEncoding == eUtf16BigEndian || m_eEncoding == eUtf16LittleEndian)) {
                // Write the BOM
                memcpy(m_pNewBuf, k_Boms[m_eEncoding], 2);
            }
            
            Utf8_Iter iter8;
            iter8.set(reinterpret_cast<const ubyte*>(p), _size, m_eEncoding);
            
            utf16* pCur = (utf16*)&m_pNewBuf[m_bIsBOM?2:0];
            
            for (; iter8; ++iter8) {
                if (iter8.canGet()) {
                    *pCur++ = iter8.get();
                }
            }
            m_nBufSize = (const char*)pCur - (const char*)m_pNewBuf;
        }
        default:
            break;
    }
    
	return m_nBufSize;
}


void Utf8_16_Write::setEncoding(Utf8_16::encodingType eType)
{
	m_eEncoding = eType;
}


void Utf8_16_Write::fclose()
{
	if (m_pNewBuf)
		delete [] m_pNewBuf;

	if (m_pFile)
		::fclose(m_pFile);
}


//=================================================================
Utf8_Iter::Utf8_Iter()
{
	reset();
}

void Utf8_Iter::reset()
{
	m_pBuf = NULL;
	m_pRead = NULL;
	m_pEnd = NULL;
	m_eState = eStart;
	m_nCur = 0;
	m_eEncoding = eUnknown;
}

void Utf8_Iter::set(const ubyte* pBuf, size_t nLen, encodingType eEncoding)
{
	m_pBuf      = pBuf;
	m_pRead     = pBuf;
	m_pEnd      = pBuf + nLen;
	m_eEncoding = eEncoding;
	operator++();
	// Note: m_eState, m_nCur not set
}

// Go to the next byte.
void Utf8_Iter::operator++()
{
	switch (m_eState)
    {
        case eStart:
            if (*m_pRead < 0x80) {
                m_nCur = *m_pRead;
                toStart();
            } else if (*m_pRead < 0xE0) {
                m_nCur = static_cast<utf16>((0x1F & *m_pRead) << 6);
                m_eState = e2Bytes_Byte2;
            } else {
                m_nCur = static_cast<utf16>((0xF & *m_pRead) << 12);
                m_eState = e3Bytes_Byte2;
            }
            break;
        case e2Bytes_Byte2:
        case e3Bytes_Byte3:
            m_nCur |= static_cast<utf8>(0x3F & *m_pRead);
            toStart();
            break;
        case e3Bytes_Byte2:
            m_nCur |= static_cast<utf16>((0x3F & *m_pRead) << 6);
            m_eState = e3Bytes_Byte3;
            break;
	}
	++m_pRead;
}

void Utf8_Iter::toStart()
{
	m_eState = eStart;
	if (m_eEncoding == eUtf16BigEndian)
    {
		swap();
	}
}

void Utf8_Iter::swap()
{
	utf8* p = reinterpret_cast<utf8*>(&m_nCur);
	utf8 swapbyte = *p;
	*p = *(p + 1);
	*(p + 1) = swapbyte;
}

//==================================================
Utf16_Iter::Utf16_Iter()
{
	reset();
}

void Utf16_Iter::reset()
{
	m_pBuf = NULL;
	m_pRead = NULL;
	m_pEnd = NULL;
	m_eState = eStart;
	m_nCur = 0;
	m_nCur16 = 0;
	m_eEncoding = eUnknown;
}

void Utf16_Iter::set(const ubyte* pBuf, size_t nLen, encodingType eEncoding)
{
	m_pBuf = pBuf;
	m_pRead = pBuf;
	m_pEnd = pBuf + nLen;
	m_eEncoding = eEncoding;
	m_eState = eStart;
	operator++();
	// Note: m_eState, m_nCur, m_nCur16 not reinitalized.
}

// Goes to the next byte.
// Not the next symbol which you might expect.
// This way we can continue from a partial buffer that doesn't align
void Utf16_Iter::operator++()
{
	switch (m_eState)
    {
        case eStart:
            if (m_eEncoding == eUtf16LittleEndian) {
                m_nCur16 = *m_pRead++;
                m_nCur16 |= static_cast<utf16>(*m_pRead << 8);
            } else {
                m_nCur16 = static_cast<utf16>(*m_pRead++ << 8);
                m_nCur16 |= *m_pRead;
            }
            ++m_pRead;
            
            if (m_nCur16 < 0x80) {
                m_nCur = static_cast<ubyte>(m_nCur16 & 0xFF);
                m_eState = eStart;
            } else if (m_nCur16 < 0x800) {
                m_nCur = static_cast<ubyte>(0xC0 | m_nCur16 >> 6);
                m_eState = e2Bytes2;
            } else {
                m_nCur = static_cast<ubyte>(0xE0 | m_nCur16 >> 12);
                m_eState = e3Bytes2;
            }
            break;
        case e2Bytes2:
        case e3Bytes3:
            m_nCur = static_cast<ubyte>(0x80 | m_nCur16 & 0x3F);
            m_eState = eStart;
            break;
        case e3Bytes2:
            m_nCur = static_cast<ubyte>(0x80 | ((m_nCur16 >> 6) & 0x3F));
            m_eState = e3Bytes3;
            break;
    }
}


