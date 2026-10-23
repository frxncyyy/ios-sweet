static int rollback_intent(const struct journal *j)
{
 int e=marker(R,j);if(!e)return 0;
 if(e!=ENOENT)return e;
 e=create_file(R,j->digest,32,0600);if(e)return e;
 return sync_path(P);
}
static int restore(const struct journal *j)
{
 int e;
 if(j->disabled){
 if(!same(D,&j->virtual)){if((e=absent(V)))return e;}
 else {if((e=same(V,&j->virtual))||(e=absent(D)))return e;
 if((e=move(V,D,&j->virtual)))return e;}
 }else if((e=same(V,&j->virtual))||(e=absent(D)))return e;
 if(!same(A,&j->original))return absent(B);
 if((e=same(B,&j->original)))return e;
 if(!same(A,&j->custom)){
 if((e=custom(j,1))||(e=absent(S)))return e;
 if((e=move(A,S,&j->custom)))return e;
 }else if((e=absent(A)))return e;
 if((e=move(B,A,&j->original)))return e;
 return same(A,&j->original);
}
