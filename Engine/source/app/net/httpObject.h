//-----------------------------------------------------------------------------
// Copyright (c) 2012 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#ifndef _HTTPOBJECT_H_
#define _HTTPOBJECT_H_

#ifndef _TVECTOR_H_
#include "core/util/tVector.h"
#endif
#ifndef _TCPOBJECT_H_
#include "app/net/tcpObject.h"
#endif

#include <curl/curl.h>
#include <thread>
#include <atomic>

class HTTPObject : public SimObject
{
public:
   DECLARE_CALLBACK(void, onLine, (const char* line));
   DECLARE_CALLBACK(bool, onPacket, (const char* data));
   // DECLARE_CALLBACK(void, onEndReceive, ());
   // DECLARE_CALLBACK(void, onDNSResolved,());
   // DECLARE_CALLBACK(void, onDNSFailed, ());
   DECLARE_CALLBACK(void, onConnected, ());
   DECLARE_CALLBACK(void, onConnectFailed, ());
   DECLARE_CALLBACK(void, onDisconnect, ());

private:
   typedef SimObject Parent;
   friend class CURLFinishEvent;

protected:
   CURL *mCURL;
   std::thread *mThread;
   CURLcode mResponseCode;
   curl_slist *mHeaders;

   U8 *mBuffer;
   U32 mBufferSize;
   U32 mBufferUsed;

   bool ensureBuffer(U32 length);

   static size_t writeCallback(char *buffer, size_t size, size_t nitems, HTTPObject *object);
   size_t processData(char *buffer, size_t size, size_t nitems);

   void start();
   void process();
public:
   void get(const char *hostName, const char *urlName, const char *query);
   void post(const char *host, const char *path, const char *query, const char *post);
   void setHeader(const char *name, const char *value);
   HTTPObject();
   virtual ~HTTPObject();

   DECLARE_CONOBJECT(HTTPObject);
};


#endif  // _H_HTTPOBJECT_
