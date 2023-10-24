#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <time.h>
// prototipos de las funciones
size_t writeData(void *ptr, size_t size, size_t nmemb, FILE *stream);
int getApiData(const char *url, const char *output_filename);
//utilizar wgetime https://www.openmp.org/spec-html/5.0/openmpsu160.html
//llamar a multiples apis
//llevar la funcion main al final
int main(void)//dentro del main solo 2 llamadas getMultipleApiData, saveApiData(poner una linea que registre si se guarda),
{
  time_t start_time, end_time;
  double elapsed_time;
  // Registra el tiempo de inicio
  time(&start_time);

  /*En caso de modificar hacerlo de la siguiente forma https://api.covidtracking.com/v1/states/{NOMENCLATURA_ESTADO}/info.json
  NO deben haber menos de 5 elementos y si hay mas de 5 elementos no se obtendran todos*/
  char *urls[5];//variables propias de una sola funcion van dentro de la funcion -> getMultipleApiData
  urls[0] = "https://api.covidtracking.com/v1/states/al/info.json";
  urls[1] = "https://api.covidtracking.com/v1/states/ak/info.json";
  urls[2] = "https://api.covidtracking.com/v1/states/ca/info.json";
  urls[3] = "https://api.covidtracking.com/v1/states/az/info.json";
  urls[4] = "https://api.covidtracking.com/v1/states/hi/info.json";
  // Deben corresponder a la nomenclatura utilizada en las urls SIN variar el orden
  char *output_files[5];
  output_files[0] = "Alabama.json";
  output_files[1] = "Alaska.json";
  output_files[2] = "California.json";
  output_files[3] = "Arizona.json";
  output_files[4] = "Hawaii.json";

  for (int i = 0; i < 5; i++)
  {
    if (getApiData(urls[i], output_files[i]) == 0)
    {/*crear funcion getMultipleApiData (devolver un arreglo por cada respuesta de la api para utilizar como parametro de 
    saveApiData dentro del main), llamar a getApiData adentro*/
      printf("Descarga y guardado exitosos.\n");
    }
    else
    {
      fprintf(stderr, "Error en la descarga y guardado.\n");
    }
  }

  time(&end_time);
  // Calcula el tiempo transcurrido en segundos
  elapsed_time = difftime(end_time, start_time);
  printf("Tiempo de ejecución: %.4f segundos\n", elapsed_time);//dentro de getMultipleData

  return 0;
}

//una cosa por funcion (guardar en una funcion a parte)
//obtener, guardar en variable, guardar en archivo, imprimir
int getApiData(const char *url, const char *output_filename)//registrar la demora y con cual api (log)
{
  CURL *curl = NULL;
  CURLcode res;

  FILE *output_file = fopen(output_filename, "wb");
  if (!output_file)
  {
    fprintf(stderr, "Error al abrir el archivo de salida: %s\n", output_filename);//imprimir en la funcion principal
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