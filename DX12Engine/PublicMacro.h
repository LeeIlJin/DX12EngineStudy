#pragma once

#define DELETE(p) if(p != nullptr){ delete p; p = nullptr; }
#define DELETE_ARR(p) if(p != nullptr){ delete[] p; p = nullptr; }
#define DESTROY(p) if(p != nullptr){ p->Destroy(); delete p; p = nullptr; }
#define RELEASE(p) if(p != nullptr){ p->Release(); delete p; p = nullptr; }

#define STR(a) #a
#define WSTR(a) L#a

#define HASH_SEED 3874628
#define HASH_ORIG(a,b) (b ^ ((b << 5) + a + (b >> 2)))
#define HASH1(a) HASH_ORIG(a,HASH_SEED)
#define HASH2(a,b) HASH_ORIG(a,HASH_ORIG(b,HASH_SEED))
#define HASH3(a,b,c) HASH_ORIG(a,HASH_ORIG(b,HASH_ORIG(c,HASH_SEED)))
#define HASH4(a,b,c,d) HASH_ORIG(a,HASH_ORIG(b,HASH_ORIG(c,HASH_ORIG(d,HASH_SEED))))
#define HASH5(a,b,c,d,e) HASH_ORIG(a,HASH_ORIG(b,HASH_ORIG(c,HASH_ORIG(d,HASH_ORIG(e,HASH_SEED)))))
#define HASH6(a,b,c,d,e,f) HASH_ORIG(a,HASH_ORIG(b,HASH_ORIG(c,HASH_ORIG(d,HASH_ORIG(e,HASH_ORIG(f,HASH_SEED))))))
