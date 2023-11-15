#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <stdatomic.h>
#include <signal.h>

#define ENDVAL -1
#define NOTHING -42

typedef long long int lli;
typedef unsigned long long int ulli;

_Atomic int flag = 1;
int sig = NOTHING;

int FLAG_N = 0;
int FLAG_O = 0;
lli FLAG_S = 100000LL;
lli FLAG_B = 0LL;
lli FLAG_C = -1LL;

void signalHandler(int signal){
	if(signal == SIGINT){
		fprintf(stderr, "\nInterrupted by user.\n");
		fprintf(stderr, "Exitting...\n");
		
		atomic_store(&flag, 0);
	}else if(signal == SIGTERM){
		fprintf(stderr, "\nTerminated.\n");
		
		atomic_store(&flag, 0);
	}
	sig = signal;
		
}

int calculate(lli limit, FILE *out){
    if(limit < 2LL)
		return 1;

	int usen;
    double per;
    int control;
    double sqr;
    
	lli refsize = sqrt(limit);

	lli psize = refsize - (refsize % 100000);
	psize = psize / 100000;
	psize++;

	lli **tmpref = (lli**)malloc(psize * sizeof(lli*));
    lli *values = (lli*)malloc(FLAG_S * sizeof(lli));

	if(values == NULL || tmpref == NULL){
		fprintf(stderr, "Bad alloc.\n");
		fprintf(stderr, "Exitting.\n");

		if(tmpref != NULL)
			free(tmpref);
		if(values != NULL)
			free(values);

		return 2;
	}

	for(int i = 0; i < psize; i++){
		tmpref[i] = NULL;
	}
	while(tmpref[0] == NULL){
		tmpref[0] = (lli*)malloc(100001 * sizeof(lli));
	}
	for(int i = 0; i < 100001; i++){
		tmpref[0][i] = ENDVAL;
	}
	for(int i = 0; i < FLAG_S; i++){
		values[i] = ENDVAL;
	}
	tmpref[0][0] = 2LL;

	usen = 1;
	int j, w;
	lli k = 0LL;
	lli size = 1LL;
	for(lli i = 3LL; i <= refsize; i += 2){
		if(atomic_load(&flag) == 0)
			break;

		if(usen == 100000){
			k++;
			while(tmpref[k] == NULL){
				tmpref[k] = (lli*)malloc(100001 * sizeof(lli));
			}
			for(int f = 0; f < 100001; f++){
				tmpref[k][f] = ENDVAL;
			}

			usen = 0;
		}

		w = 0LL;
		j = 1LL;
		control = 1;
		sqr = sqrt(i);
		while(tmpref[w] != NULL){
			if(atomic_load(&flag) == 0){
				control = 0;
				break;
			}

			while(tmpref[w][j] <= sqr && tmpref[w][j] != ENDVAL){
				if(i % tmpref[w][j] == 0LL){
					control = 0;
					break;
				}
	
				j++;
			}
			w++;
		}

		if(control == 1){
			tmpref[k][usen] = i;
			usen++;
			size++;
		}
		
		
	}

	lli *ref = NULL;
	while(ref == NULL){
		ref = (lli*)malloc((size + 1) * sizeof(lli));
	}
	ref[size] = ENDVAL;
	lli r = 0LL;
	for(lli i = 0; i <= k; i++){
		j = 0LL;
		while(tmpref[i][j] != ENDVAL){
			ref[r] = tmpref[i][j];
			j++;
			r++;
		}
	}
	for(lli i = 0; i <= k; i++){
		free(tmpref[i]);
	}
	free(tmpref);

	lli bgn;
	if(FLAG_B == 0LL){
		for(lli i = 0LL; i < size; i++){
			fprintf(out, "%lld\n", ref[i]);
		}
		bgn = refsize;
	}else if(FLAG_B < refsize && FLAG_B != 0LL){
		lli s = 0;
		while(ref[s] < FLAG_B && ref[s] != ENDVAL){
			s++;
		}
		for(lli i = s; i < size; i++){
			fprintf(out, "%lld\n", ref[i]);
		}
		bgn = ref[size - 1] + 1;
	}else{
		bgn = FLAG_B;
	}

	if(bgn % 2 == 0){
		bgn++;
	}
	usen = 0;
    for(lli i = bgn; i <= limit; i += 2){
    	if(atomic_load(&flag) == 0){
			break;
		}
		if(usen == FLAG_S){
			for(int f = 0; f < FLAG_S; f++){
				fprintf(out, "%lld\n", values[f]);
				values[f] = ENDVAL;
			}
			usen = 0;
		}
    	
		j = 1LL;
		control = 1;
		sqr = sqrt(i);
		while(ref[j] <= sqr && ref[j] != ENDVAL){
			if(i % ref[j] == 0LL){
				control = 0;
				break;
			}

			j++;
		}

		if(control == 1){
			values[usen] = i;
			usen++;
		}
		


		if(i % 250000LL == 1 && FLAG_N == 0){
			per = (double)i;
			per = per * 100.0;
			per = per / (double)limit;
	
			fprintf(stderr, "\r%.3lf%%", per);
		}

    }

	if(usen != 0 && sig != SIGTERM){
		for(int i = 0; i < usen; i++){
			fprintf(out, "%lld\n", values[i]);
		}
	}

    free(values);
	free(ref);

	return 0;
}

int check(long long int num){
	if(num < 2){
		return 0;
	}else if(num == 2){
		return 1;
	}else if(num % 2 == 0){
		printf("%lld %% 2 = 0\n", num);
		return 0;
	}

	int c;
	int rval = 1;

	lli j, k, w;
	lli sqr;

	int usen = 0;
	lli refsize = (lli)sqrt(num);
	refsize++;
	lli psize = refsize - (refsize % 100000);
	psize = psize / 100000;
	psize++;

	lli **ref = (lli**)malloc(psize * sizeof(lli*));
	if(ref == NULL)
		return -1;

	for(int i = 0; i < psize; i++){
		ref[i] = NULL;
	}
	while(ref[0] == NULL){
		ref[0] = (lli*)malloc(100001 * sizeof(lli));
	}

	for(lli i = 0LL; i < 100001; i++){
		ref[0][i] = ENDVAL;
	}
	ref[0][0] = 2LL;

	k = 0LL;
	for(lli i = 3LL; i < refsize; i += 2){
		if(atomic_load(&flag) == 0)
			break;


		if(usen == 100000){
			k++;
			while(ref[k] == NULL){
				ref[k] = (lli*)malloc(100001 * sizeof(lli));
			}
			for(int h = 0; h < 100001; h++){
				ref[k][h] = ENDVAL;
			}
			usen = 0;
		}
		w = 0LL;
		c = 1;
		sqr = (lli)sqrt(i);
		while(ref[w] != NULL){
			if(atomic_load(&flag) == 0)
				break;


			j = 0LL;
			while(ref[w][j] != ENDVAL && ref[w][j] <= sqr){
				if(i % ref[w][j] == 0){
					c = 0;
					break;
				}
				j++;
			}
			w++;
		}

		if(c == 1){
			if(num % i == 0){
				printf("%lld %% %lld = 0\n", num, i);
				rval = 0;
				break;
			}
			ref[k][usen] = i;

			usen++;
		}
	}

	for(long long int i = 0LL; i <= k; i++){
		free(ref[i]);
	}
	free(ref);

	return rval;
}



int main(int argc, char *argv[]){
	
	if(argc < 2){
		fprintf(stderr, "Provide a number as an argument\n");
		fprintf(stderr, "Exitting\n");
		
		exit(1);
	}

	struct sigaction sa;
	sa.sa_handler = signalHandler;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);

	struct sigaction sa2;
	sa2.sa_handler = signalHandler;
	sa2.sa_flags = 0;
	sigemptyset(&sa2.sa_mask);
	
	if(sigaction(SIGINT, &sa, NULL) == -1 || sigaction(SIGTERM, &sa2, NULL) == -1){
		fprintf(stderr, "An error occured while creating sigaction.\n");
		exit(2);
	}
	
	//signal(SIGINT, signalHandler);
	
	int opt;
	char *outf;
	char *inpf;
	char *endptr;
	while((opt = getopt(argc, argv, "hvno:b:s:c:")) != EOF){
		switch(opt){
			case 'h':
				printf("An useless prime number calculator.\n");
				printf("Usage: prime4 [OPTIONS] LIMIT");
				printf("\n\n");

				printf("-h\t\tDisplay this help and exit.\n");
				printf("-v\t\tDisplay version and exit\n");
				printf("-n\t\tDo not print progress.\n");
				printf("-o FILE\t\tSpecify output FILE, default: stdout\n");
				printf("-b BEGIN\tSet the BEGIN of program. Default: 0\n");
				printf("-s SIZE\t\tIndicate the SIZE of used memory block (byte).\n");
				printf("\t\tDefault: %llu bytes.\n",FLAG_S * sizeof(lli));
				printf("-c NUMBER\tCheck NUMBER if it is prime or not.\n");
				printf("\t\tCan not be used with any other flag.\n");

				exit(0);

			case 'v':
				printf("Version 4.3\n");
				exit(0);

			case 'o':
				if(optarg == NULL){
					fprintf(stderr, "-o requires an argument.\n");
					fprintf(stderr, "Exitting.\n");

					exit(1);
				}

				FLAG_O = 1;
				outf = optarg;
				break;

			case 'b':
				if(optarg == NULL){
					fprintf(stderr, "-b requires an argument.\n");
					fprintf(stderr, "Exitting.\n");

					exit(1);
				}
				FLAG_B = strtoll(optarg, &endptr, 10);
				if(*endptr != '\0'){
					fprintf(stderr, "Argument of -b have to be an integer.\n");
					fprintf(stderr, "Exitting.\n");

					exit(1);
				}
				if(FLAG_B < 0){
					fprintf(stderr, "First value can not be less than 0.\n");
					fprintf(stderr, "Exitting.\n");

					exit(1);
				}
				break;

			case 's':
				if(optarg == NULL){
					fprintf(stderr, "-s requires an argument.\n");
					fprintf(stderr, "Exitting.\n");

					exit(1);
				}
				FLAG_S = strtoll(optarg, &endptr, 10);
				if(*endptr != '\0'){
					fprintf(stderr, "Argument of -s have to be an integer.\n");
					fprintf(stderr, "Exitting.\n");

					exit(1);
				}
				if(FLAG_S < sizeof(long long int)){
					fprintf(stderr, "Size of memory block can not be less than %lu bytes.\n", sizeof(lli));
					fprintf(stderr, "Exitting.\n");

					exit(1);
				}
				FLAG_S = FLAG_S / (int)sizeof(long long int);
				break;

			case 'n':
				FLAG_N = 1;
				break;

			case 'c':
				if(optarg == NULL){
					fprintf(stderr, "-c requires an argument.\n");
					fprintf(stderr, "Exitting.\n");

					exit(1);
				}
				FLAG_C = strtoll(optarg, &endptr, 10);
				if(*endptr != '\0'){
					fprintf(stderr, "Argument of -c have to be an integer.\n");
					fprintf(stderr, "Exitting.\n");

					exit(1);
				}
				if(FLAG_C < 0){
					fprintf(stderr, "There is no prime number less than 0.\n");
					fprintf(stderr, "Exitting.\n");

					exit(1);
				}
				break;


		}
	}

	if(FLAG_C >= 0){
		int val = check(FLAG_C);
		if(val == -1){
			fprintf(stderr, "Bad alloc.\n");
			return -1;
		}else if(val == 1){
			if(sig == NOTHING){
				printf("%lld is prime.\n", FLAG_C);
			}
			return 0;
		}else{
			if(sig == NOTHING){
				printf("%lld is not prime.\n", FLAG_C);
			}
			return 0;
		}
	}

	if(argv[optind] == NULL){
		fprintf(stderr, "Provide a number as an argument\n");
		fprintf(stderr, "Exitting\n");

		exit(1);
	}
	long long int limit = strtoll(argv[optind], &endptr, 10);
	if(*endptr != '\0'){
		fprintf(stderr, "Provide a NUMBER as an argument\n");
		fprintf(stderr, "Exitting\n");

		exit(1);
	}
	
	int rval;
	if(FLAG_O == 0){
		rval = calculate(limit, stdout);
	}else{
		FILE *fp = fopen(outf, "w");
		if(fp == NULL){
			fprintf(stderr, "fopen() error.\n");
			fprintf(stderr, "Exitting.\n");

			exit(1);
		}
		rval = calculate(limit, fp);

		fclose(fp);
	}

    fprintf(stderr, "\n");

    return rval;
}
