#include "info.h"

#include <string.h>

#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <curl/curl.h>

// libcurl stuff idk
struct MemoryStruct {
    char *memory;
    size_t size;
};

size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if(ptr == NULL)
        return 0;

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

// get the current public ip
int public_ip(char *dest) {
    CURL *curl_handle = curl_easy_init();
    CURLcode res;

    // fallback
    char ip_str[32] = "";
    int pipes[2];

    struct MemoryStruct chunk;
    chunk.memory = malloc(4096);
    chunk.size = 0;

    if(!curl_handle) {
        curl_easy_cleanup(curl_handle);
        free(chunk.memory);

        goto fallback;
    }

    curl_easy_setopt(curl_handle, CURLOPT_URL, "ident.me");
    curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    res = curl_easy_perform(curl_handle);

    if(res != CURLE_OK) {
        curl_easy_cleanup(curl_handle);
        free(chunk.memory);

        goto fallback;
    }

    strncpy(dest, chunk.memory, 256);

    curl_easy_cleanup(curl_handle);
    free(chunk.memory);

    return 0;

    fallback:
        if(pipe(pipes))
            return 1;
        
        if(!fork()) {
            close(pipes[0]);
            dup2(pipes[1], STDOUT_FILENO);

            execlp("curl", "curl", "-s", "ident.me", NULL); 
        }
        wait(0);
        close(pipes[1]);
        ip_str[read(pipes[0], ip_str, 32) - 1] = 0;
        close(pipes[0]);

    if(!ip_str[0]) {
        return 1;
    }

    strcpy(dest, ip_str);
    return 0;
}
