// Request.cpp: implementation of the Request class.
//
//////////////////////////////////////////////////////////////////////


#include "HttpRequest.hpp"
#include "bufwrap.hpp"

BEGIN_NAMESPACE_ARCHY

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Request::Request()
{
}

Request::~Request()
{
}

//*******************************************************************************************************
// ValidHostChar: 
//					Return TRUE if the specified character is valid
//						for a host name, i.e. A-Z or 0-9 or -.: 
//*******************************************************************************************************
BOOL Request::ValidHostChar(char ch)
{
    return( isalpha(ch) || isdigit(ch)
        || ch=='-' || ch=='.' || ch==':' );
}


//*******************************************************************************************************
// ParseURL: 
//					Used to break apart a URL such as 
//						http://www.localhost.com:80/TestPost.htm into protocol, port, host and request.
//*******************************************************************************************************
void Request::ParseURL(LPCSTR url,LPSTR protocol,int lprotocol,
        LPSTR host,int lhost,LPSTR request,int lrequest,int *port)
{
    char *work,*ptr,*ptr2;

    *protocol = *host = *request = 0;
    *port=80;

    work = strdup(url);
    strupr(work);

    ptr = strchr(work,':');							// find protocol if any
    if(ptr!=NULL)
    {
        *(ptr++) = 0;
        lstrcpyn(protocol,work,lprotocol);
    }
    else
    {
        lstrcpyn(protocol,"HTTP",lprotocol);
        ptr = work;
    }

    if( (*ptr=='/') && (*(ptr+1)=='/') )			// skip past opening /'s 
        ptr+=2;

    ptr2 = ptr;										// find host
    while( ValidHostChar(*ptr2) && *ptr2 )
        ptr2++;

    *ptr2=0;
    lstrcpyn(host,ptr,lhost);

    lstrcpyn(request,url + (ptr2-work),lrequest);	// find the request

    ptr = strchr(host,':');							// find the port number, if any
    if(ptr!=NULL)
    {
        *ptr=0;
        *port = atoi(ptr+1);
    }

    free(work);
}

//*******************************************************************************************************
// SendHTTP: 
//					Main entry point for this code.  
//					  url			- The URL to GET/POST to/from.
//					  headerSend		- Headers to be sent to the server.
//					  post			- Data to be posted to the server, NULL if GET.
//					  postLength	- Length of data to post.
//					  req			- Contains the message and headerSend sent by the server.
//
//					  returns -1 on failure, 0 on success.
//*******************************************************************************************************
int Request::SendHTTP(LPCSTR url,LPCSTR headerReceive,BYTE *post,
        DWORD postLength,HTTPRequest *req, int timeoutsec)
{
    char			buffer[512];
    char			protocol[20],host[256],request[1024];
    int				l,port,chars;
    bufwrap			headersBuffer,messageBuffer;
	char			headerSend[1024];
    BOOL			done;
    
	int				ret;


    ParseURL(url,protocol,sizeof(protocol),host,sizeof(host),		// Parse the URL
        request,sizeof(request),&port);
    if(strcmp(protocol,"HTTP"))
        return -1;


	//printf("\r\n\r\n <<SEND   HTTP   REQUEST  : >> \r\n\r\n");		 //Send request

    if( !*request )
        lstrcpyn(request,"/",sizeof(request));

    if( post == NULL )
	{
		strcpy(headerSend, "GET ");
	}
    else 
	{
		strcpy(headerSend, "POST ");
	}
	strcat(headerSend, request);
	strcat(headerSend, " HTTP/1.0\r\n");
	strcat(headerSend, "Accept: image/gif, image/x-xbitmap,"
        " image/jpeg, image/pjpeg, application/vnd.ms-excel,"
        " application/msword, application/vnd.ms-powerpoint,"
        " */*\r\n");
	strcat(headerSend, "Accept-Language: en-us\r\n");
	strcat(headerSend, "Accept-Encoding: gzip, deflate\r\n");
	strcat(headerSend, "User-Agent: Mozilla/4.0\r\n");

    if(postLength)
    {
        wsprintf(buffer,"Content-Length: %ld\r\n",postLength);
		strcat(headerSend, buffer);
    }
	//SendString(sock,"Cookie: mycookie=blablabla\r\n");
	//	printf("Cookie: mycookie=blablabla\r\n");
	strcat(headerSend, "Host: ");
	strcat(headerSend, host);
	strcat(headerSend, "\r\n");

    if( (headerReceive!=NULL) && *headerReceive )
	{
		strcat(headerSend, headerReceive);
	}
    
	// Send a blank line to signal end of HTTP headerReceive
	strcat(headerSend, "\r\n");
	int lenHeaderSend = strlen(headerSend);

	tcp_socket tcp;
	ret = tcp.connect(host, port, timeoutsec);
	if( ret < 0 ) {
		return -1;
	}
	
	ret = tcp.write(headerSend, lenHeaderSend, timeoutsec);
	if( ret != lenHeaderSend) {
		return -1;
	}

    if( (post!=NULL) && postLength )
	{
		ret = tcp.write((const char*)post, postLength, timeoutsec);
		if( ret != postLength) {
			return -1;
		}
		post[postLength] = '\0';
		
		strcat(headerSend, (const char*)post);
	}
    
	req->headerSend.assign(headerSend);

    // Read the result
	//printf("\r\n\r\n <<RECEIVE   HTTP   REQUEST  : >> \r\n\r\n\r\n");	
	// First read HTTP headerReceive

    chars = 0;
    done = FALSE;

    while(!done)
    {
        l = tcp.read(buffer,1, timeoutsec);
        if(l<0)
            done=TRUE;

        switch(*buffer)
        {
            case '\r':
                break;
            case '\n':
                if(chars==0)
                    done = TRUE;
                chars=0;
                break;
            default:
                chars++;
                break;
        }

		headersBuffer.append(buffer, 1);
    }

    req->headerReceive = headersBuffer;

	// Now read the HTTP body
    do
    {
        l = tcp.read(buffer,sizeof(buffer)-1,timeoutsec);
        if(l<0)
            break;
        *(buffer+l)=0;
		messageBuffer.append(buffer, l);

    } while(l>0);

    req->message = messageBuffer;

    return 0;
}


//*******************************************************************************************************
// SendRequest
//
//*******************************************************************************************************
int Request::SendRequest(bool IsPost, LPCSTR url, bufwrap &data, 
						 bufwrap &psHeaderReceive, bufwrap &psMessage, 
						 int timeoutsecs /*= 60*/)
{
	HTTPRequest			req;
    int					rtn;

    //Read in arguments
   
	if(IsPost)
    {													/* POST */
		rtn	= SendHTTP(	url,
					"Content-Type: application/x-www-form-urlencoded\r\n",
					(unsigned char*)data.getcbuf(),
					data.size(),
					&req, timeoutsecs);
        
    }
    else												/* GET */
        rtn = SendHTTP(url,NULL,NULL,0,&req, timeoutsecs);


	if(!rtn)											//Output message and/or headerSend 
	{
		data = req.headerSend;
		psHeaderReceive = req.headerReceive;
		psMessage = req.message;
    }
    else
	{
        //printf("\nFailed\n");
		// MessageBox(0, "Retrieve Failed", "", 0);
		return -1;
	}

	return 0;
}

int Request::GetPageStatusCode(const bufwrap&psHeaderReceive)
{
// the following code should be reviewed

	//get the correct header line
	char retCode[8];

	char *s = strstr(psHeaderReceive.getcbuf(), "http");
	if (s == NULL) return -1; //no headers

	s += 4;
	while (*s == ' ') {
		s++;
	}
	//find the 3 digit code...
	if (strlen(s) < 3) return -1; //some error,  the string is too short...
	while (!(isdigit(s[0]) && isdigit(s[1]) && isdigit(s[2])))
	{
		if (s[3] == '\0') return -1; //we have reached the end of the string, without finding the number...
		s++;
	}
	//make a copy of s, and return the code
	strncpy(retCode, s, 3);
	retCode[3] = '\0'; //remove all text after the 3 digit response code
	int result = atoi(retCode);

	return result;

}
END_NAMESPACE_ARCHY
