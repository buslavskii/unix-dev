#include <stdio.h>
#include <string.h>

#define HTTP_HEADER_LEN 256
#define HTTP_REQUEST_LEN 256
#define HTTP_METHOD_LEN 7
#define HTTP_URI_LEN 8000
#define HTTP_URI_PATH 100
#define HTTP_URI_PARAMS 1000
#define HTTP_VERSION 100
#define HTTP_USER_AGENT 100
#define HTTP_SERVER 100
#define HTTP_ACCEPT 100

#define REQ_END 100
#define ERR_NO_URI -100
#define ERR_ENDLESS_URI -101
#define ERR_ENDLESS_URI -101

struct http_req {
	char request[HTTP_REQUEST_LEN];
	char method[HTTP_METHOD_LEN];
	char uri[HTTP_URI_LEN];
	char uri_path[HTTP_URI_PATH];
	char uri_params[HTTP_URI_PARAMS];
	char version[HTTP_VERSION];
	char user_agent[HTTP_USER_AGENT];
	char server[HTTP_SERVER];
	char accept[HTTP_ACCEPT];
};

int parseGetRequest(char *buf, struct http_req *req);


int fill_req(char *buf, struct http_req *req) {
	if (strlen(buf) == 2)
	{
		// пустая строка (\r\n) означает конец запроса
		return REQ_END;
	}
	char *p, *a, *b, *question, *sharp;
	int answer;
	// Это строка GET-запроса
	p = strstr(buf, "GET");
	if (p == buf)
	{
		answer = parseGetRequest(p, req);
	}

	return answer;
}


int parseGetRequest(char *buf, struct http_req *req)
{
    char *p, *a, *b, *question, *sharp;
	// Это строка GET-запроса
	p = strstr(buf, "GET");
	if (p == buf)
	{
        // Строка запроса должна быть вида
		// GET /dir/ HTTP/1.0
		// GET /dir HTTP/1.1
		// GET /test123?r=123 HTTP/1.1
		// и т.п.
		strncpy(req->request, buf, strlen(buf));
		strncpy(req->method, "GET", strlen("GET"));
		a = strchr(buf, '/');
		question = strchr(a, '?');
		strncpy(req->uri_path, a, b-a);
		if ( a != NULL) { // есть запрашиваемый URI
			b = strchr(a, ' ');
			if ( b != NULL ) { // конец URI
				strncpy(req->uri, a, b-a);
			} else {
				return ERR_ENDLESS_URI;
				// тогда это что-то не то
			}
		} else {
			return ERR_NO_URI;
			// тогда это что-то не то
		}
    }
}

int log_req(struct http_req *req) {
	// fprintf(stderr, "%s %s\n%s\n", req->request, req->method, req->uri);
	return 0;
}

int make_resp(struct http_req *req) {
	printf("HTTP/1.1 200 OK\r\n");
	printf("Content-Type: text/html\r\n");
	printf("\r\n");
	printf("<html><body><title>Page title</title>");
	printf("<h1>Page Header</h1><h2>Title 2</h2><h3>Request type = %s</h3>", req->method);
	printf("<h3>Request text = %s</h3>", req->request);
	printf("<h3>Text of uri  = %s</h3>", req->uri);
	printf("<h3>Text of uri_path = %s</h3>", req->uri_path);
	printf("</body></html>\r\n");
	return 0;
}

int main (void) {
	char buf[HTTP_HEADER_LEN];
	struct http_req req;
	while(fgets(buf, sizeof(buf),stdin)) {
		int ret = fill_req(buf, &req);
		if (ret == 0)
			// строка запроса обработана, переходим к следующей
			continue;
		if (ret == REQ_END )
			// конец HTTP запроса, вываливаемся на обработку
			break;
		else
			// какая-то ошибка
			printf("Error: %d\n", ret);

	}
	log_req(&req);
	make_resp(&req);
}
