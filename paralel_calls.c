#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <omp.h>

// Prototipos de las funciones
size_t writeData(void *ptr, size_t size, size_t nmemb, FILE *stream);
int getApiData(const char *url, const char *output_filename);

int main(void)
{
  time_t start_time, end_time;
  double elapsed_time;
  // Registra el tiempo de inicio
  time(&start_time);

  char *urls[5];
  urls[0] = "https://api.covidtracking.com/v1/states/al/info.json";
  urls[1] = "https://api.covidtracking.com/v1/states/ak/info.json";
  urls[2] = "https://api.covidtracking.com/v1/states/ca/info.json";
  urls[3] = "https://api.covidtracking.com/v1/states/az/info.json";
  urls[4] = "https://api.covidtracking.com/v1/states/hi/info.json";

  char *output_files[5];
  output_files[0] = "Alabama.json";
  output_files[1] = "Alaska.json";
  output_files[2] = "California.json";
  output_files[3] = "Arizona.json";
  output_files[4] = "Hawaii.json";

#pragma omp parallel for
  for (int i = 0; i < 5; i++)
  {
    if (getApiData(urls[i], output_files[i]) == 0)
    {
#pragma omp critical
      printf("Descarga y guardado exitosos para %s.\n", output_files[i]);
    }
    else
    {
#pragma omp critical
      fprintf(stderr, "Error en la descarga y guardado de %s.\n", output_files[i]);
    }
  }

  time(&end_time);
  // Calcula el tiempo transcurrido en segundos
  elapsed_time = difftime(end_time, start_time);
  printf("Tiempo de ejecución: %.4f segundos\n", elapsed_time);
  return 0;
}

int getApiData(const char *url, const char *output_filename)
{
  CURL *curl = NULL;
  CURLcode res;

  FILE *output_file = fopen(output_filename, "wb");
  if (!output_file)
  {
    fprintf(stderr, "Error al abrir el archivo de salida: %s\n", output_filename);
    return 1;
  }

  curl_global_init(CURL_GLOBAL_DEFAULT);

  curl = curl_easy_init();

  if (curl)
  {
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, output_file);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeData);

    res = curl_easy_perform(curl);

    if (res != CURLE_OK)
    {
      fprintf(stderr, "curl_easy_perform() falló: %s\n", curl_easy_strerror(res));
      fclose(output_file);
      return 1;
    }
    else
    {
      printf("El resultado se ha guardado en '%s'\n", output_filename);
    }

    curl_easy_cleanup(curl);
  }

  curl_global_cleanup();
  fclose(output_file);

  return 0;
}

size_t writeData(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
  return fwrite(ptr, size, nmemb, stream);
}