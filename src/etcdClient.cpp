#include "etcdClient.h"
#include <curl/curl.h>
#include <iostream>
#include <sstream>
#include <cstring>

// Callback function to write HTTP response data
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *userp)
{
    size_t totalSize = size * nmemb;
    userp->append((char *)contents, totalSize);
    return totalSize;
}

EtcdClient::EtcdClient(const std::string &endpoint) : baseUrl(endpoint)
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

EtcdClient::~EtcdClient()
{
    curl_global_cleanup();
}

EtcdResponse EtcdClient::performRequest(const std::string &method,
                                        const std::string &endpoint,
                                        const std::string &data)
{
    EtcdResponse response;
    CURL *curl = curl_easy_init();

    if (!curl)
    {
        response.success = false;
        response.error = "Failed to initialize CURL";
        return response;
    }

    std::string url = baseUrl + endpoint;
    std::string responseString;

    // Set basic options
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

    // Set HTTP method and data
    if (method == "POST")
    {
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        if (!data.empty())
        {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        }
    }
    else if (method == "PUT")
    {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
        if (!data.empty())
        {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        }
    }
    else if (method == "DELETE")
    {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    }

    // Set headers
    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // Perform the request
    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK)
    {
        response.success = false;
        response.error = curl_easy_strerror(res);
    }
    else
    {
        long httpCode;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
        response.httpCode = static_cast<int>(httpCode);
        response.success = (httpCode >= 200 && httpCode < 300);
        response.data = responseString;

        if (!response.success)
        {
            response.error = "HTTP error: " + std::to_string(httpCode);
        }
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return response;
}

EtcdResponse EtcdClient::put(const std::string &key, const std::string &value, int ttl)
{
    std::string keyEncoded = base64Encode(key);
    std::string valueEncoded = base64Encode(value);

    std::ostringstream json;
    json << "{\"key\":\"" << keyEncoded << "\",\"value\":\"" << valueEncoded << "\"";
    if (ttl > 0)
    {
        json << ",\"lease\":" << ttl;
    }
    json << "}";

    return performRequest("POST", "/v3/kv/put", json.str());
}

EtcdResponse EtcdClient::get(const std::string &key)
{
    std::string keyEncoded = base64Encode(key);
    std::string json = "{\"key\":\"" + keyEncoded + "\"}";

    return performRequest("POST", "/v3/kv/range", json);
}

EtcdResponse EtcdClient::del(const std::string &key)
{
    std::string keyEncoded = base64Encode(key);
    std::string json = "{\"key\":\"" + keyEncoded + "\"}";

    return performRequest("POST", "/v3/kv/deleterange", json);
}

EtcdResponse EtcdClient::createLease(int ttl)
{
    std::string json = "{\"TTL\":" + std::to_string(ttl) + "}";
    return performRequest("POST", "/v3/lease/grant", json);
}

EtcdResponse EtcdClient::renewLease(const std::string &leaseId)
{
    std::string json = "{\"ID\":" + leaseId + "}";
    return performRequest("POST", "/v3/lease/keepalive", json);
}

EtcdResponse EtcdClient::revokeLease(const std::string &leaseId)
{
    std::string json = "{\"ID\":" + leaseId + "}";
    return performRequest("POST", "/v3/lease/revoke", json);
}

EtcdResponse EtcdClient::health()
{
    return performRequest("GET", "/health");
}

std::string EtcdClient::base64Encode(const std::string &value)
{
    // Simple base64 encoding implementation
    static const std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string result;
    int i = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    const char *bytes_to_encode = value.c_str();
    int in_len = value.length();

    while (i < in_len)
    {
        char_array_3[0] = bytes_to_encode[i++];
        char_array_3[1] = (i < in_len) ? bytes_to_encode[i++] : 0;
        char_array_3[2] = (i < in_len) ? bytes_to_encode[i++] : 0;

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (int j = 0; j < 4; j++)
        {
            result += chars[char_array_4[j]];
        }
    }

    // Add padding
    int mod = value.length() % 3;
    if (mod == 1)
    {
        result[result.length() - 2] = '=';
        result[result.length() - 1] = '=';
    }
    else if (mod == 2)
    {
        result[result.length() - 1] = '=';
    }

    return result;
}

std::string EtcdClient::urlEncode(const std::string &value)
{
    CURL *curl = curl_easy_init();
    if (curl)
    {
        char *encoded = curl_easy_escape(curl, value.c_str(), value.length());
        std::string result(encoded);
        curl_free(encoded);
        curl_easy_cleanup(curl);
        return result;
    }
    return value;
}
