#### CEFET-MG - Engenharia de Computação
#### Sistemas Operacionais
---
# Processos com Memória Compartilhada
O objetivo deste trabalho é entendermos melhor o funcionamento dos sistemas operacionais e como funciona a criação de processos que dividem um mesmo espaço de memoria

## Metodologia

 ##### Memória Compartilhada

1. **Criando uma área para a memória compartilhada**

Para isso, utilizamos o comando shm_open, em que cria e mapeia de acordo com os parâmetros informados na chamada: 

```C
   fd = shm_open("/sharedmemory", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
   if (fd == -1)
   {
      perror("shm_open");
      exit(1);	
   }

```

2. **Ajustando o tamanho da área compartilhada:**

Ao chamarmos ftruncate, apenas informamos o descritor, retornado no passo anterior, e o tamanho do segmento desejado:

```C
if (ftruncate(fd, sizeof(value)) == -1)
   {
      perror("ftruncate");
      exit(1);
   }
```
obs: declaramos anteriormente *value* como double.

3. **Mapeando o seguimento de memória criado:**
Primeiramente, criamos um ponteiro “double *ptr;” e utilizamos o mmap da seguinte forma:
```C
   ptr = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
   if (ptr == MAP_FAILED)
   {
      perror("mmap");
      exit(1);
   }
```
Em que o último parâmetro indica o ponto de início do mapeamento do arquivo e SIZE (definido anteriormente) o tamanho em bytes:


##### Processos

Como vimos anteriormente, ao utilizarmos fork(), criamos um novo processo idêntico ao pai no momento da criação.<br>

Dessa forma, com o auxílio de uma variável global, iniciamos um loop, em que a cada iteração… 

1.  Move o ponteiro.

2. Chama pi(i) salvando sua parcela calculada na nova posição do ponteiro.

3. Cria um novo processo.
```c
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
```
Obs: n_process é informado pelo usuário.
```c
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
```

##### Valor de Pi

Finalmente, criamos um novo laço, em que somamos cada parcela calculada pelos processos filhos, finalizados, em uma única variável, enquanto movemos o ponteiro para a próxima posição adequada, com o auxílio de sizeof(double).

```C
for (int i = 0; i < n_process; i++) {
         printf("O processo %d terminou e ", wait(0));
         ptr += sizeof(double);                       // Move o ponteiro.
         value = (*ptr);
         printf("o pai leu o valor: %f\n", value); 
         sum += value;
      }
```

# Resultado

Para compilação devemos utilizar:<br>
>*gcc -Wall shm.c -o shm -lrt*

Para iniciarmos devemos utilizar:<br>
>*./shm **x*** <br>

em que **x** deve ser um valor inteiro, representando a quantidade de processos desejados.
