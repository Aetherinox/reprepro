#ifndef REPREPRO_SHA512_H
#define REPREPRO_SHA512_H

/* Structure to save state of computation between the single steps.  */
struct SHA512_Context
{
  uint64_t H[8];

  uint64_t total[2];
  uint64_t buflen;
  char buffer[256];	/* NB: always correctly aligned for uint32_t.  */
};

#define SHA512_DIGEST_SIZE 64

void SHA512Init(/*@out@*/struct SHA512_Context *context);
void SHA512Update(struct SHA512_Context *context, const uint8_t *data, size_t len);
void SHA512Final(struct SHA512_Context *context, /*@out@*/uint8_t digest[SHA512_DIGEST_SIZE]);

#endif
