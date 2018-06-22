/*  HTTPS with follow-redirect
    Created by Sujay S. Phadke, 2016
    All rights reserved.

*/

#include <WiFiClientSecure.h>

class HTTPSRedirect : public WiFiClientSecure {
  private:
    const int httpsPort;
    const char* redirFingerprint;
    bool fpCheck = false;
    bool keepAlive = true;
    bool verboseInfo = false;
    bool tomada1=NULL;
    bool tomada2=NULL;
    bool tomada3=NULL;

  public:
    HTTPSRedirect(const int, const char*, bool);
    HTTPSRedirect(const int);
    ~HTTPSRedirect();

    bool printRedir(const char*, const char*, const char*);
    bool printRedir(String&, const char*, const char*);
    String createRequest(const char*, const char*);
    void fetchData(bool, bool);
    bool isTomada1On();
    bool isTomada2On();
    bool isTomada3On();

};

