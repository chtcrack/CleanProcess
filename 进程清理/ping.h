/*
Module : PING.H
Purpose: Interface for an MFC wrapper class to encapsulate PING
Created: PJN / 10-06-1998

Copyright (c) 1998 - 2007 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


/////////////////////////// Macros / Defines ////////////////////////

#pragma once

#ifndef __PING_H__
#define __PING_H__

#ifndef CPING_NO_WINSOCK2
  #ifndef _WINSOCK2API_
    #pragma message("To avoid this message place you need to include Winsock2.h in your PCH (normally stdafx.h)")
    #include <winsock2.h>
    #include <ws2tcpip.h>
  #endif
#else
  #ifndef _WINSOCKAPI_
    #pragma message("You need to include Winsock.h, Winsock2.h or AfxSock.h in your PCH (normally stdafx.h) to compile CPing")
    #include <winsock.h>
  #endif
#endif

#ifndef IP_EXPORT_INCLUDED
#pragma message("To avoid this message please put IPExport.h in your PCH (normally stdafx.h)")
#include <IPExport.h>
#endif

#ifndef CPING_EXT_CLASS
#define CPING_EXT_CLASS
#endif

#ifndef CPING_EXT_API
#define CPING_EXT_API
#endif


/////////////////////////// Classes /////////////////////////////////

struct CPING_EXT_CLASS CPingReply
{
//Constructors / Destructors
  CPingReply();

//Member variables
	in_addr	 Address;              //The IP address of the replier
	unsigned long RTT;             //Round Trip time in Milliseconds
  unsigned long EchoReplyStatus; //here will be status of the last ping if successful
};

class CPING_EXT_CLASS CPing
{
public:
//Constructors / Destructors
  CPing();
  ~CPing();

//Methods
#ifndef CPING_NO_ICMP
	BOOL PingUsingICMP(LPCTSTR pszHostName, CPingReply& pr, UCHAR nTTL = 10, DWORD dwTimeout = 5000, WORD wDataSize = 32, UCHAR nTOS = 0, BOOL bDontFragment = FALSE) const;
#endif
#ifndef CPING_NO_WINSOCK2
	BOOL PingUsingWinsock(LPCTSTR pszHostName, CPingReply& pr, UCHAR nTTL = 10, DWORD dwTimeout = 5000, WORD wDataSize = 32, UCHAR nTOS = 0, BOOL bDontFragment = FALSE, LPCTSTR pszLocalBoundAddress = NULL, USHORT nSequenceNumber = 0, USHORT nIdentifier = GetCurrentProcessId()) const;
#endif

protected:
// Macros / Defines
#ifndef CPING_NO_WINSOCK2
  //IP header
  #pragma pack(push, 1) //The IP_HEADER and ICMP_HEADER should be alligned on 1 byte boundaries
  typedef struct tagIP_HEADER 
  {
	  unsigned char  h_len:4;        // length of the header
	  unsigned char  version:4;      // Version of IP
	  unsigned char  tos;            // Type of service
	  unsigned short total_len;      // total length of the packet
	  unsigned short ident;          // unique identifier
	  unsigned short frag_and_flags; // flags
	  unsigned char  ttl; 
	  unsigned char  proto;          // protocol (TCP, UDP etc)
	  unsigned short checksum;       // IP checksum
	  unsigned long  sourceIP;
	  unsigned long  destIP;
  } IP_HEADER;
  typedef IP_HEADER FAR* LPIP_HEADER;

  // ICMP header
  typedef struct tagICMP_HEADER 
  {
    BYTE   i_type;
    BYTE   i_code; // type sub code
    USHORT i_cksum;
    USHORT i_id;
    USHORT i_seq;
    ULONG  timestamp; // This is not the std header, but we reserve space for time data field
  } ICMP_HEADER;
  typedef ICMP_HEADER FAR* LPICMP_HEADER;
  #pragma pack(pop)
#endif

//Methods
#ifndef CPING_NO_WINSOCK2
  virtual BOOL Bind(SOCKET socket, LPCTSTR pszLocalBoundAddress) const;
  virtual void FillIcmpData(LPICMP_HEADER pIcmp, int nPacketSize, USHORT nSequenceNumber, USHORT nIdentifier) const;
  virtual BOOL DecodeResponse(LPIP_HEADER pIPHeader, int nBytes, sockaddr_in* from, USHORT nExpectedSequenceNumber, BOOL& bTryAgain) const;
  virtual USHORT GenerateIPChecksum(USHORT* pBuffer, int nSize) const;
#endif

#ifndef CPING_NO_ICMP
//Function Pointers
  typedef HANDLE (WINAPI ICMPCREATEFILE)(void);
  typedef ICMPCREATEFILE* LPICMPCREATEFILE;
  typedef BOOL (WINAPI ICMPCLOSEHANDLE)(HANDLE);
  typedef ICMPCLOSEHANDLE* LPICMPCLOSEHANDLE;
  typedef DWORD (WINAPI ICMPSENDECHO)(HANDLE, IPAddr, LPVOID, WORD, IP_OPTION_INFORMATION*, LPVOID, DWORD, DWORD);
  typedef ICMPSENDECHO* LPICMPSENDECHO;

//Member variables
  HMODULE           m_hIcmp;         //HINSTANCE of Iphlapi.DLL or as a fall back ICMP.DLL
  LPICMPCREATEFILE  m_pIcmpCreateFile;
  LPICMPSENDECHO    m_pIcmpSendEcho;
  LPICMPCLOSEHANDLE m_pIcmpCloseHandle;
#endif

  static BOOL IsSocketReadible(SOCKET socket, DWORD dwTimeout, BOOL& bReadible);
};

#endif //__PING_H__
