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

#include "app/net/httpObject.h"

#include "platform/platform.h"
#include "core/stream/fileStream.h"
#include "console/simBase.h"
#include "console/consoleInternal.h"
#include "console/engineAPI.h"
#include "core/strings/stringUnit.h"

IMPLEMENT_CONOBJECT(HTTPObject);

ConsoleDocClass( HTTPObject,
   "@brief Allows communications between the game and a server using HTTP.\n\n"
   
   "HTTPObject is derrived from TCPObject and makes use of the same callbacks for dealing with "
   "connections and received data.  However, the way in which you use HTTPObject to connect "
   "with a server is different than TCPObject.  Rather than opening a connection, sending data, "
   "waiting to receive data, and then closing the connection, you issue a get() or post() and "
   "handle the response.  The connection is automatically created and destroyed for you.\n\n"
   
   "@tsexample\n"
      "// In this example we'll retrieve the weather in Las Vegas using\n"
      "// Google's API.  The response is in XML which could be processed\n"
      "// and used by the game using SimXMLDocument, but we'll just output\n"
      "// the results to the console in this example.\n\n"

      "// Define callbacks for our specific HTTPObject using our instance's\n"
      "// name (WeatherFeed) as the namespace.\n\n"

      "// Handle an issue with resolving the server's name\n"
      "function WeatherFeed::onDNSFailed(%this)\n"
      "{\n"
      "   // Store this state\n"
      "   %this.lastState = \"DNSFailed\";\n\n"

      "   // Handle DNS failure\n"
      "}\n\n"

      "function WeatherFeed::onConnectFailed(%this)\n"
      "{\n"
      "   // Store this state\n"
      "   %this.lastState = \"ConnectFailed\";\n\n"
      "   // Handle connection failure\n"
      "}\n\n"

      "function WeatherFeed::onDNSResolved(%this)\n"
      "{\n"
      "   // Store this state\n"
      "   %this.lastState = \"DNSResolved\";\n\n"
      "}\n\n"

      "function WeatherFeed::onConnected(%this)\n"
      "{\n"
      "   // Store this state\n"
      "   %this.lastState = \"Connected\";\n\n"

      "   // Clear our buffer\n"
      "   %this.buffer = \"\";\n"
      "}\n\n"

      "function WeatherFeed::onDisconnect(%this)\n"
      "{\n"
      "   // Store this state\n"
      "   %this.lastState = \"Disconnected\";\n\n"

      "   // Output the buffer to the console\n"
      "   echo(\"Google Weather Results:\");\n"
      "   echo(%this.buffer);\n"
      "}\n\n"

      "// Handle a line from the server\n"
      "function WeatherFeed::onLine(%this, %line)\n"
      "{\n"
      "   // Store this line in out buffer\n"
      "   %this.buffer = %this.buffer @ %line;\n"
      "}\n\n"

      "// Create the HTTPObject\n"
      "%feed = new HTTPObject(WeatherFeed);\n\n"

      "// Define a dynamic field to store the last connection state\n"
      "%feed.lastState = \"None\";\n\n"

      "// Send the GET command\n"
      "%feed.get(\"www.google.com:80\", \"/ig/api\", \"weather=Las-Vegas,US\");\n"
   "@endtsexample\n\n" 
   
   "@see TCPObject\n"

   "@ingroup Networking\n"
);


IMPLEMENT_CALLBACK(HTTPObject, onLine, void, (const char* line), (line),
   "@brief Called whenever a line of data is sent to this TCPObject.\n\n"
   "This callback is called when the received data contains a newline @\\n character, or "
   "the connection has been disconnected and the TCPObject's buffer is flushed.\n"
   "@param line Data sent from the server.\n"
   );

IMPLEMENT_CALLBACK(HTTPObject, onPacket, bool, (const char* data), (data),
   "@brief Called when we get a packet with no newlines or nulls (probably websocket).\n\n"
   "@param data Data sent from the server.\n"
   "@return true if script handled the packet.\n"
   );

//IMPLEMENT_CALLBACK(HTTPObject, onEndReceive, void, (), (),
//   "@brief Called when we are done reading all lines.\n\n"
//   );
//
//IMPLEMENT_CALLBACK(HTTPObject, onDNSResolved, void, (),(),
//   "Called whenever the DNS has been resolved.\n"
//   );
//
//IMPLEMENT_CALLBACK(HTTPObject, onDNSFailed, void, (),(),
//   "Called whenever the DNS has failed to resolve.\n"
//   );

IMPLEMENT_CALLBACK(HTTPObject, onConnected, void, (),(),
   "Called whenever a connection is established with a server.\n"
   );

IMPLEMENT_CALLBACK(HTTPObject, onConnectFailed, void, (),(),
   "Called whenever a connection has failed to be established with a server.\n"
   );

IMPLEMENT_CALLBACK(HTTPObject, onDisconnect, void, (),(),
   "Called whenever the TCPObject disconnects from whatever it is currently connected to.\n"
   );

//--------------------------------------

HTTPObject::HTTPObject()
{
   mCURL = curl_easy_init();
   curl_easy_setopt(mCURL, CURLOPT_VERBOSE, false);
   curl_easy_setopt(mCURL, CURLOPT_FOLLOWLOCATION, true);
   curl_easy_setopt(mCURL, CURLOPT_TRANSFERTEXT, true);
   curl_easy_setopt(mCURL, CURLOPT_USERAGENT, TORQUE_APP_NAME " " TORQUE_APP_VERSION_STRING);
   curl_easy_setopt(mCURL, CURLOPT_ENCODING, "UTF-8");
   curl_easy_setopt(mCURL, CURLOPT_WRITEDATA, this);
   curl_easy_setopt(mCURL, CURLOPT_WRITEFUNCTION, &HTTPObject::writeCallback);
   curl_easy_setopt(mCURL, CURLOPT_HEADERDATA, this);
   curl_easy_setopt(mCURL, CURLOPT_HEADERFUNCTION, &HTTPObject::headerCallback);

   mBuffer = NULL;
   mBufferSize = 0;
   mBufferUsed = 0;
   mSendHeaders = NULL;
}

HTTPObject::~HTTPObject()
{
   curl_easy_cleanup(mCURL);
}

//--------------------------------------

class CURLFinishEvent : public SimEvent {
public:
   virtual void process(SimObject *object) {
      static_cast<HTTPObject *>(object)->process();
   }
};

void HTTPObject::start()
{
   mThread = new std::thread([this]() {
      //Do the curl
      mResponseCode = curl_easy_perform(mCURL);
      Sim::postCurrentEvent(this, new CURLFinishEvent());
   });
}

size_t HTTPObject::writeCallback(char *buffer, size_t size, size_t nitems, HTTPObject *object) {
   return object->processData(buffer, size, nitems);
}

bool HTTPObject::ensureBuffer(U32 length) {
   if (mBufferSize < length) {
      //CURL_MAX_WRITE_SIZE is the maximum packet size we'll be given. So round
      // off to that and we should not have to allocate too often.
      length = ((length / CURL_MAX_WRITE_SIZE) + 1) * CURL_MAX_WRITE_SIZE;
      void *alloced = dRealloc(mBuffer, length * sizeof(char));

      //Out of memory
      if (!alloced) {
         return false;
      }

      mBuffer = (U8 *)alloced;

      mBufferSize = length;
   }
   return true;
}

size_t HTTPObject::processData(char *buffer, size_t size, size_t nitems) {
//   char *packet = Con::getReturnBuffer(size * nitems + 1);
//   dMemcpy(packet, buffer, size * nitems);
//   packet[size * nitems] = '\0';
//   onPacket_callback(packet);

   size_t writeSize = size * nitems + 1;

   if (!ensureBuffer(mBufferUsed + writeSize)) {
      //Error
      return 0;
   }

   dMemcpy(mBuffer + mBufferUsed, buffer, size * nitems);
   mBufferUsed += size * nitems;
   mBuffer[mBufferUsed] = 0;

   return size * nitems;
}

void HTTPObject::process() {
   if (mResponseCode != CURLE_OK) {
      onConnectFailed_callback();
      return;
   }

   onConnected_callback();

   //Pull all the lines out of mBuffer
   char *str = (char *)mBuffer;
   char *nextLine = str;
   while (str && nextLine) {
      nextLine = dStrchr(str, '\n');

      //Get how long the current line for allocating
      U32 lineSize = 0;
      if (nextLine == NULL) {
         lineSize = dStrlen(str);
      } else {
         lineSize = nextLine - str;
      }

      //Copy into a return buffer for the script
      char *line = Con::getReturnBuffer(lineSize + 1);
      dMemcpy(line, str, lineSize);
      line[lineSize] = 0;

      //Strip the \r from \r\n
      if (lineSize > 0 && line[lineSize - 1] == '\r') {
         line[lineSize - 1] = 0;
      }

      onLine_callback(line);

      if (nextLine) {
         //Strip the \n
         str = nextLine + 1;
      }
   }

   //Clean up
   dFree(mBuffer);
   mBuffer = NULL;
   mBufferUsed = 0;
   mBufferSize = 0;

   onDisconnect_callback();
}

size_t HTTPObject::headerCallback(char *buffer, size_t size, size_t nitems, HTTPObject *object) {
   return object->processHeader(buffer, size, nitems);
}

size_t HTTPObject::processHeader(char *buffer, size_t size, size_t nitems) {
   char *colon = dStrchr(buffer, ':');
   if (colon != NULL) {
      String key(buffer, colon - buffer);
      String value(colon + 2);

      if (value[value.length() - 1] == '\n')
         value.erase(value.length() - 1, 1);
      if (value[value.length() - 1] == '\r')
         value.erase(value.length() - 1, 1);

      mRecieveHeaders.insert(key, value);
   }

   return size * nitems;
}

//--------------------------------------
void HTTPObject::get(const char *host, const char *path, const char *query) {
   dsize_t urlLength = dStrlen(host) + dStrlen(path) + 2;
   if (query) {
      urlLength += dStrlen(query);
   }
   char *url = new char[urlLength];
   dSprintf(url, urlLength, "%s%s%s%s", host, path, (query && *query ? "?" : ""), (query && *query ? query : ""));
   curl_easy_setopt(mCURL, CURLOPT_URL, url);
   if (mSendHeaders) {
      curl_easy_setopt(mCURL, CURLOPT_HTTPHEADER, mSendHeaders);
   }

   start();
}

void HTTPObject::post(const char *host, const char *path, const char *query, const char *post) {
   dsize_t urlLength = dStrlen(host) + dStrlen(path) + 2;
   if (query) {
      urlLength += dStrlen(query);
   }
   char *url = new char[urlLength];
   dSprintf(url, urlLength, "%s%s%s%s", host, path, (query && *query ? "?" : ""), (query && *query ? query : ""));
   curl_easy_setopt(mCURL, CURLOPT_URL, url);

   curl_easy_setopt(mCURL, CURLOPT_POST, true);
   curl_easy_setopt(mCURL, CURLOPT_POSTFIELDS, post);
   if (mSendHeaders) {
      curl_easy_setopt(mCURL, CURLOPT_HTTPHEADER, mSendHeaders);
   }

   start();
}

const char *HTTPObject::getHeader(const char *name) {
   if (!mRecieveHeaders.contains(name)) {
      return NULL;
   }
   return mRecieveHeaders[name].c_str();
}

void HTTPObject::getHeaderList(Vector<String> &headers) {
   for (const auto &pair : mRecieveHeaders) {
      headers.push_back(pair.key);
   }
}

void HTTPObject::setHeader(const char *name, const char *value) {
   dsize_t headerSize = dStrlen(name) + 3;
   if (value != NULL) {
      headerSize += dStrlen(value);
   }
   char *header = Con::getReturnBuffer(headerSize);
   if (value == NULL) {
      //Unset
      dSprintf(header, headerSize, "%s:", name);
   } else if (*value == 0) {
      //Empty value
      dSprintf(header, headerSize, "%s;", name);
   } else {
      //Given value
      dSprintf(header, headerSize, "%s: %s", name, value);
   }

   //Formatting: Replace spaces with hyphens
   dsize_t nameLen = dStrlen(name);
   for (U32 i = 0; i < nameLen; i ++) {
      if (header[i] == ' ')
         header[i] = '-';
   }

   mSendHeaders = curl_slist_append(mSendHeaders, header);
}

//--------------------------------------
DefineEngineMethod(HTTPObject, get, void, (const char *address, const char* requirstURI, const char *query), ("/", ""),
   "@brief Send a GET command to a server to send or retrieve data.\n\n"

   "@param address HTTP web address to send this get call to. Be sure to include the port at the end (IE: \"www.garagegames.com:80\").\n"
   "@param requirstURI Specific location on the server to access (IE: \"index.php\".)\n"
   "@param query Optional. Actual data to transmit to the server. Can be anything required providing it sticks with limitations of the HTTP protocol. "
   "If you were building the URL manually, this is the text that follows the question mark.  For example: http://www.google.com/ig/api?<b>weather=Las-Vegas,US</b>\n"
   
   "@tsexample\n"
      "// Create an HTTP object for communications\n"
      "%httpObj = new HTTPObject();\n\n"
      "// Specify a URL to transmit to\n"
      "%url = \"www.garagegames.com:80\";\n\n"
      "// Specify a URI to communicate with\n"
      "%URI = \"/index.php\";\n\n"
      "// Specify a query to send.\n"
      "%query = \"\";\n\n"
      "// Send the GET command to the server\n"
      "%httpObj.get(%url,%URI,%query);\n"
   "@endtsexample\n\n"
   )
{
   if (!query || !query[0])
      object->get(address, requirstURI, NULL);
   else
      object->get(address, requirstURI, query);
}

DefineEngineMethod(HTTPObject, post, void, (const char *address, const char *requirstURI, const char *query, const char *post), ("/", "", ""),
   "@brief Send POST command to a server to send or retrieve data.\n\n"

   "@param address HTTP web address to send this get call to. Be sure to include the port at the end (IE: \"www.garagegames.com:80\").\n"
   "@param requirstURI Specific location on the server to access (IE: \"index.php\".)\n"
   "@param query Actual data to transmit to the server. Can be anything required providing it sticks with limitations of the HTTP protocol. \n"
   "@param post Submission data to be processed.\n"

   "@tsexample\n"
      "// Create an HTTP object for communications\n"
      "%httpObj = new HTTPObject();\n\n"
      "// Specify a URL to transmit to\n"
      "%url = \"www.garagegames.com:80\";\n\n"
      "// Specify a URI to communicate with\n"
      "%URI = \"/index.php\";\n\n"
      "// Specify a query to send.\n"
      "%query = \"\";\n\n"
      "// Specify the submission data.\n"
      "%post = \"\";\n\n"
      "// Send the POST command to the server\n"
      "%httpObj.POST(%url,%URI,%query,%post);\n"
   "@endtsexample\n\n"
   )
{
   object->post(address, requirstURI, query, post);
}

DefineEngineMethod(HTTPObject, getHeader, const char *, (const char *name),,
   "@brief Get the contents of an HTTP header after the request has finished loading.\n\n"
   "@param name Header name\n"

   "@tsexample\n"
      "function someHTTPObject::onDisconnect(%this) {\n"
      "   echo(%this.getHeader(\"Cookie\"));\n"
      "}\n"
   "@endtsexample\n\n"
   )
{
   return object->getHeader(name);
}


DefineEngineMethod(HTTPObject, getHeaderList, const char *, (),,
   "@brief Get a tab-separated list of all HTTP headers\n\n"

   "@tsexample\n"
      "function someHTTPObject::onDisconnect(%this) {\n"
      "   echo(%this.getHeaderList());\n"
      "}\n"
   "@endtsexample\n\n"
   )
{
   Vector<String> headers;
   object->getHeaderList(headers);

   //Create a tab-separated list
   dsize_t bufferLen = 1;
   for (const String &str : headers) {
      bufferLen += str.length() + 1;
   }
   char *buffer = Con::getReturnBuffer(bufferLen);
   buffer[0] = 0;

   U32 at = 0;
   for (const String &str : headers) {
      dStrcat(buffer, str.c_str(),2048);
      at += str.length();
      buffer[at] = '\t';
      buffer[at + 1] = '\0';

      at ++;
   }
   if (at > 0) {
      buffer[at - 1] = 0;
   }
   return buffer;
}

DefineEngineMethod(HTTPObject, setHeader, void, (const char *name, const char *value), (""),
   "@brief Set a custom HTTP header to send with the request. Needs to be called before get() or post()\n\n"
   "@param name Header name\n"
   "@param value Header contents\n"

   "@tsexample\n"
      "// Create an HTTP object for communications\n"
      "%httpObj = new HTTPObject();\n\n"
      "// Set a custom HTTP header\n"
      "%httpObj.setHeader(\"Special-Data\", \"Contents\");\n"
      "%httpObj.get(...);\n"
   "@endtsexample\n\n"
   )
{
   object->setHeader(name, value);
}
