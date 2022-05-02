// gcc -Wall shm.c -o shm -lrt

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>

// Variáveis globais.
double *resultados;
unsigned n_process;

//----------------------------------------------------------------PI.
double pi(int id) {
   long ID = (long)id;
	double sum = 0.0,x;
	unsigned long long int i;
   unsigned long long int passos = 1000000000;        // 1 Bilhão.
	double h=1.0/passos;

	for (i = ID + 1; i <= passos; i += n_process) {   
		x = h * ((double)i - 0.5); 
		sum += 4.0 / (1.0 + x*x); 
	}
	resultados[ID] = h*sum;
   return resultados[ID];
}
//----------------------------------------------------------------PI.

int main(int argc, char *argv[], char *envp[])
{
   double fd, value;                                       
   const double SIZE = 4096;                          // Define um tamanho para a memória.
   double *ptr;                                       // Criação de um ponteiro.
   int new_process;
   double sum = 0;
   
   if(argc==2)
   {
      n_process = atoi(argv[1]);
   }
   else
   {
      printf("Informe a quantidade desejada de processos . ex: './shm 2'\n"); 
      exit(1);
   }

   resultados = malloc(n_process*sizeof(double));

   // Abre e cria uma área de memória compartilhada.
   fd = shm_open("/sharedmemory", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
   if (fd == -1)
   {
      perror("shm_open");
      exit(1);	
   }
   // Ajusta o tamanho da área compartilhada.
   
   if (ftruncate(fd, sizeof(value)) == -1)
   {
      perror("ftruncate");
      exit(1);
   }
   
   // Mapeia a área no espaço de endereçamento deste processo.
   
   ptr = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
   if (ptr == MAP_FAILED)
   {
      perror("mmap");
      exit(1);
   }

   printf("Ola, sou o processo pai %5d\n", getpid());
   printf("\n");

   for (int i = 0; i < n_process; i++)
   {
      ptr += sizeof(double);                          // Move o ponteiro.
      (*ptr) = pi(i);
      new_process = fork();                           // Cria um novo processo.
      
      if (new_process == 0)
      {
         printf ("Sou o %5d filho de %5d ", getpid(),getppid());
         break;
      }
   }

   sleep(1);

   if (new_process == 0)                              // Processo filho.
   {                                       
      value = (*ptr);
      printf ("e tenho o valor %f\n", value);
      sleep(1);
      exit(1);
   }
   else                                               // Processo pai.
   {
      sleep(1);  
      ptr -= n_process * sizeof(double);              // Reseta o ponteiro para o início da memória.
      
      for (int i = 0; i < n_process; i++) {
         printf("O processo %d terminou e ", wait(0));
         ptr += sizeof(double);                       // Move o ponteiro.
         value = (*ptr);
         printf("o pai leu o valor: %f\n", value); 
         sum += value;
      }
      
   }
   sleep(1);
   printf("\n");
   printf("Valor de PI é: %f\n", sum);
   return 0;
}
