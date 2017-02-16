// Request.h: interface for the Request class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REQUEST_H__9F2C9BB6_CBA7_40AF_80A4_09A1CE1CE220__INCLUDED_)
#define AFX_REQUEST_H__9F2C9BB6_CBA7_40AF_80A4_09A1CE1CE220__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "tcp.hpp"
#include "bufwrap.hpp"


#define MEM_BUFFER_SIZE 10

BEGIN_NAMESPACE_ARCHY
/* 
	HTTPRequest: Structure that returns the HTTP headers and message
					from the request
*/
typedef struct
{ 
    bufwrap headerSend;								// Pointer to HTTP header Send 
	bufwrap headerReceive;							// Pointer to HTTP headers Receive
    bufwrap message;									// Pointer to the HTTP message 
} HTTPRequest;


class Request  
{
public:
	Request();
	virtual ~Request();

private:
	BOOL		ValidHostChar(char ch);
	void		ParseURL(LPCSTR url,LPSTR protocol,int lprotocol, 
		LPSTR host,int lhost,LPSTR request,int lrequest,int *port);

	int			SendHTTP(LPCSTR url,LPCSTR headers,BYTE *post, 
		DWORD postLength,HTTPRequest *req, int timeoutsec);

public:
	int		SendRequest(bool IsPost, LPCSTR url, bufwrap &data, 
		bufwrap &psHeaderReceive, bufwrap &psMessage, int timeoutsecs = 60);

	static int GetPageStatusCode(const bufwrap&psHeaderReceive);
};

END_NAMESPACE_ARCHY

#endif // !defined(AFX_REQUEST_H__9F2C9BB6_CBA7_40AF_80A4_09A1CE1CE220__INCLUDED_)
