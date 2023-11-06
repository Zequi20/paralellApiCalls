#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <string.h>
#include <omp.h>

#define NUM_URLS 5

size_t writeCallback(void *contents, size_t size, size_t nmemb, char **output);
char *getApiData(const char *url);
void saveApiData(char **responses);

char **getMultipleApiData()
{
    char *apiUrls[] = {
        "https://api.chucknorris.io/jokes/random",
        "https://rickandmortyapi.com/api/character/155",
        "https://v2.jokeapi.dev/joke/Any?lang=en",
        "https://fakestoreapi.com/products/1",
        "https://pokeapi.co/api/v2/location/1/"
    };
    char **apiRespones = (char **)malloc(NUM_URLS * sizeof(char *));
    #pragma omp parallel for
    for (int i = 0; i < NUM_URLS; i++)
    {
        apiRespones[i] = getApiData(apiUrls[i]);
    }
    return apiRespones;
}

char *getApiData(const char *url)
{
    CURL *curl;
    CURLcode res;
    char *response = NULL;
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        response = NULL;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    return response;
}

void saveApiData(char **responses)
{
    for (int i = 0; i < NUM_URLS; i++)
    {
        if (responses[i] != NULL)
        {
            char filename[100];
            snprintf(filename, sizeof(filename), "response%d.json", i + 1);
            FILE *file = fopen(filename, "w");
            if (file)
            {
                fprintf(file, "%s", responses[i]);
                fclose(file);
                printf("Respuesta de la solicitud numero %d guardada en %s\n", i + 1, filename);
            }
            else
            {
                fprintf(stderr, "No se pudo abrir el archivo %s para guardar la respuesta numero %d\n", filename, i + 1);
            }
        }
    }
}

size_t writeCallback(void *contents, size_t size, size_t nmemb, char **output)
{
    size_t totalSize = size * nmemb;
    *output = (char *)malloc(totalSize + 1);
    if (*output)
    {
        memcpy(*output, contents, totalSize);
        (*output)[totalSize] = '\0';
        return totalSize;
    }
    else
    {
        return 0;
    }
}

int main()
{
    double inicio;
    double fin;
    inicio = omp_get_wtime();
    char **responses = getMultipleApiData();
    saveApiData(responses);
    free(responses);
    fin = omp_get_wtime();
    printf("La ejecución ha tomado %f segundos\n", fin - inicio);
    return 0;
}
