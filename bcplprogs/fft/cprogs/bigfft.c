#include <stdio.h>
#include <math.h>

#define pi 3.14159265

#define K 16

#define N (1<<K)

double rv[N];
double iv[N];

void butterfly(double *rp, double *ip,
               double *rq, double *iq,
               double rwk, double iwk) {
  double ra = *rp;
  double ia = *ip;
  double rb = *rq * rwk - *iq * rwk;
  double ib = *rq * iwk + *iq * rwk;
  *rp = ra+rb;
  *ip = ia+ib;
  *rq = ra-rb;
  *iq = ia-ib;
}

void reorder(double *rv, double *iv, int n) {
  int i, j = 0;
  for(i = 0; i<=n-2; i++) {
    int k = n>>1;
    // j is i with its bits is reverse order
    if(i<j) {
      double rt = rv[j];
      double it = iv[j];
      rv[j] = rv[i];
      iv[j] = iv[i];
      rv[i] =  rt;
      iv[i] =  it;
    }
    // k  =  100..00       10..0000..00
    // j  =  0xx..xx       11..10xx..xx
    // j' =  1xx..xx       00..01xx..xx
    // k' =  100..00       00..0100..00
    while(k<=j) { j -= k; k >>= 1; } //) "increment" j
    j += k;                            //)
  }
}

void fft(double *rv, double *iv,
         int ln,                  // ln = log2 n
         double rw,  double iw) { // (rw,iw) = nth root of unity
  int n = 1<<ln;
  double *rvn = rv+n;
  double *ivn = iv+n;
  int n2  = n>>1;
  int s, i, j;

  // First do the perfect shuffle
  reorder(rv, iv, n);

  // Then do all the butterfly operations
  for(s = 1; s<=ln; s++) {
    int m  = 1<<s;
    int m2 = m>>1;
    double rwk=1;
    double iwk=0;
    double rwkfac=rw;
    double iwkfac=iw;

    for(i = s+1; i<=ln; i++) {
      rwkfac = rwkfac*rwkfac - iwkfac*iwkfac;
      iwkfac = rwkfac*iwkfac + iwkfac*rwkfac;
    }

    for(j = 0; j<m2; j++) {
      double *rp = rv+j;
      double *ip = iv+j;
      while(rp<rvn) {
        butterfly(rp, ip, rp+m2, ip+m2, rwk, iwk);
	rp += m;
        ip += m;
      }
      rwk = rwk*rwkfac - iwk*iwkfac;
      iwk = rwk*iwkfac + iwk*rwkfac;
    }
  }
}

void pr(double *rv, double *iv, int max) {
  int i;
  for(i = 0; i<=max; i++) {
    printf("(%8.3f,%8.3f) ", rv[i], iv[i]);
    if(i % 4 == 3) printf("\n");
  }
  printf("\n");
}

int main() {
  int i, s;
  double rw = cos(2 * pi / N);
  double iw = sin(2 * pi / N);

  // Print a table of w^2^i, i=0..K
  for(s=0; s<=K; s++) {
    int i = 1<<s;
    printf("%4d %5d: %12.9f %12.9f\n",
            s, i, cos(2 * pi * i / N), sin(2 * pi * i / N));
  }
  return 0;

  // Print a table of w^i, i=0..N
  for(i=0; i<=N; i++) {
    printf("%4d: %12.9f %12.9f\n", i, cos(2 * pi * i / N), sin(2 * pi * i / N));
  }
  return 0;

  // Set some test data

  for(i=0; i<N; i++) {
    //rv[i] = cos(2 * pi * i / N);
    //iv[i] = sin(2 * pi * i / N) * 0;
    rv[i] = i;
    iv[i] = 0;
  }

  //  rv[1]=1;

  pr(rv, iv, N-1);

  fft(rv, iv, K, rw, iw);

  pr(rv, iv, N-1);

  printf("\n");

  //for(i=8; i<N; i++) rw[i]=iw[i]=0;

  fft(rv, iv, K, rw, -iw);

  for(i=0; i<N; i++) {
    rv[i] /= N;
    iv[i] /= N;
  }

  pr(rv, iv, N-1);

  return 0;
}

