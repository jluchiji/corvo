#ifndef _HEADERS_H_
#define _HEADERS_H_

#define RES_HTTP      "HTTP/1.1 "

/* 200 */
#define RES_200       RES_HTTP "200 OK"
#define RES_201       RES_HTTP "201 Created"
#define RES_204       RES_HTTP "204 No Content"

/* 300 */

/* 400 */
#define RES_400       RES_HTTP "400 Bad Request"
#define RES_401       RES_HTTP "401 Unauthorized"
#define RES_403       RES_HTTP "403 Forbidden"
#define RES_404       RES_HTTP "404 Not Found"

/* 500 */
#define RES_500       RES_HTTP "500 Internal Server Error"

/* Headers */
#define RES_POW       "X-Powered-By: Corvo/1.0" // Name this project ?

#endif
