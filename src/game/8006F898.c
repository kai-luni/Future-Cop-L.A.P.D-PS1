#include "game/types.h"
typedef struct {
 u8 pad[0x68]; s16 d0,d1; u8 a,b,c,d,e,f,g,h,i,j,k,l; u8 pad78; u8 m,n,o,pad7c,p,q,r; s16 x,y,z,u,v,w;
} Obj;
void func_8006F898(Obj *p) {
 s32 x,y,z; u8 t;
 if (p->d0 < 16) { p->x=0; p->y=0; p->z=0; }
 else {
 p->x=((p->d-p->a)<<12)/p->d0;
 p->y=((p->e-p->b)<<12)/p->d0;
 p->z=((p->f-p->c)<<12)/p->d0;
 }
 if (p->d1 < 16) { p->u=0; p->v=0; p->w=0; }
 else {
 p->u=((p->j-p->g)<<12)/p->d1;
 p->v=((p->k-p->h)<<12)/p->d1;
 p->w=((p->l-p->i)<<12)/p->d1;
 }
 p->n=p->b; p->o=p->c; p->m=p->a; p->p=p->g; p->q=p->h; p->r=p->i;
 if (p->x<0) { t=p->d; p->d=p->a; p->a=t; }
 if (p->y<0) { t=p->e; p->e=p->b; p->b=t; }
 if (p->z<0) { t=p->f; p->f=p->c; p->c=t; }
 if (p->u<0) { t=p->j; p->j=p->g; p->g=t; }
 if (p->v<0) { t=p->k; p->k=p->h; p->h=t; }
 if (p->w<0) { t=p->l; p->l=p->i; p->i=t; }
}
