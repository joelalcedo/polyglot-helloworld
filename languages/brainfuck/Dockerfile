# syntax=docker/dockerfile:1
FROM alpine:3.20
WORKDIR /app
RUN <<'EOF'
set -e
apk add --no-cache build-base
cat > /tmp/bf.c <<'C'
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int isop(char c){
  return c=='>'||c=='<'||c=='+'||c=='-'||c=='.'||c==','||c=='['||c==']';
}

int main(int argc, char** argv){
  if(argc < 2){ fprintf(stderr,"usage: bf <file>\n"); return 2; }
  FILE* f = fopen(argv[1], "rb");
  if(!f){ perror(argv[1]); return 1; }
  fseek(f, 0, SEEK_END);
  long n = ftell(f);
  fseek(f, 0, SEEK_SET);
  char* src = (char*)malloc((size_t)n + 1);
  if(!src){ fclose(f); return 1; }
  if(fread(src, 1, (size_t)n, f) != (size_t)n){ fclose(f); free(src); return 1; }
  fclose(f);
  src[n] = 0;

  char* prog = (char*)malloc((size_t)n + 1);
  if(!prog){ free(src); return 1; }
  int m = 0;
  for(long i=0;i<n;i++) if(isop(src[i])) prog[m++] = src[i];
  prog[m] = 0;
  free(src);

  int* match = (int*)malloc(sizeof(int) * (size_t)m);
  int* stack = (int*)malloc(sizeof(int) * (size_t)m);
  if(!match || !stack){ free(prog); free(match); free(stack); return 1; }
  int sp = 0;
  for(int i=0;i<m;i++){
    if(prog[i] == '[') stack[sp++] = i;
    else if(prog[i] == ']'){
      if(sp == 0){ fprintf(stderr,"unmatched ]\n"); return 1; }
      int j = stack[--sp];
      match[i] = j;
      match[j] = i;
    }
  }
  if(sp != 0){ fprintf(stderr,"unmatched [\n"); return 1; }

  unsigned char tape[30000];
  memset(tape, 0, sizeof(tape));
  int p = 0;
  for(int ip=0; ip<m; ip++){
    switch(prog[ip]){
      case '>': p = (p + 1) % 30000; break;
      case '<': p = (p + 29999) % 30000; break;
      case '+': tape[p]++; break;
      case '-': tape[p]--; break;
      case '.': putchar(tape[p]); fflush(stdout); break;
      case ',': { int c = getchar(); tape[p] = (c == EOF) ? 0 : (unsigned char)c; } break;
      case '[': if(tape[p] == 0) ip = match[ip]; break;
      case ']': if(tape[p] != 0) ip = match[ip]; break;
    }
  }

  free(match);
  free(stack);
  free(prog);
  return 0;
}
C
cc -O2 -s -o /usr/local/bin/bf /tmp/bf.c
rm -f /tmp/bf.c
EOF
ENV PATH="/usr/local/bin:$PATH"
COPY hello.bf .
CMD ["sh", "-c", "bf hello.bf"]
