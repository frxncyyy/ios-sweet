int nord2_audio_install(void)
{
 struct journal j;struct n2ap_view pkg;DTEntry chosen;const void *data=NULL;unsigned size=0;
 int e=0,recovery=0,phase=1,m=mode();ctx=vfs_context_kernel();
 if(m<0)return EINVAL;
 e=journal_read(&j);
 if(!e){
 phase=2;
 int rolled=!same(A,&j.original)&&!absent(B)&&!same(j.disabled?D:V,&j.virtual)&&!absent(j.disabled?V:D);
 int committed=!marker(C,&j), rollback_requested=!marker(R,&j);
 int interrupted=!same(B,&j.original) &&
 ((!absent(A)&&!same(S,&j.custom)) ||
 (j.disabled&&!same(D,&j.virtual)&&!absent(V)));
 if(rolled){e=m==1?EALREADY:0;goto out;}
 if(m==2||rollback_requested||!committed||interrupted){
 if(m==2&&!rollback_requested){e=rollback_intent(&j);if(e)goto out;}
 e=restore(&j);recovery=1;goto out;
 }
 /* Committed installs must match package if supplied on a repeated mode=1 boot. */
 e=custom(&j,1);if(e)goto out;e=same(B,&j.original);if(e)goto out;
 e=same(V,&j.virtual);if(e)goto out;e=absent(D);if(e)goto out;
 if(m==1){if(SecureDTLookupEntry(NULL,"/chosen",&chosen)!=kSuccess||SecureDTGetProperty(chosen,"nord2-audio-package",&data,&size)!=kSuccess){e=ENOENT;goto out;}
 e=n2ap_parse(data,size,check_hash,&pkg);if(e){e=EINVAL;goto out;}
 if(pkg.executable.length!=j.executable_size||pkg.plist.length!=j.plist_size||
 check_hash(pkg.executable.bytes,pkg.executable.length,j.executable_hash)||check_hash(pkg.plist.bytes,pkg.plist.length,j.plist_hash))e=EINVAL;}
 goto out;
 }
 if(e!=ENOENT)goto out;
 if(m!=1){e=m==2?baseline_original():0;goto out;}
 if(SecureDTLookupEntry(NULL,"/chosen",&chosen)!=kSuccess||SecureDTGetProperty(chosen,"nord2-audio-package",&data,&size)!=kSuccess){e=ENOENT;goto out;}
 e=n2ap_parse(data,size,check_hash,&pkg);if(e){e=EINVAL;goto out;}
 phase=3;memset(&j,0,sizeof(j));memcpy(j.magic,"N2AJNL1",8);j.version=1;
 if((e=directory(P))||(e=directory(VP))||(e=directory(TUN))||
 (e=absent(B))||(e=absent(S))||(e=absent(C))||(e=absent(R))||
 (e=identity(A,&j.original)))goto out;
 if(j.original.type!=VDIR){e=EINVAL;goto out;}
 if(!absent(V)){j.disabled=1;if((e=directory(DP))||(e=identity(D,&j.virtual)))goto out;}
 else {if((e=absent(D))||(e=identity(V,&j.virtual)))goto out;}
 if(j.virtual.type!=VDIR){e=EINVAL;goto out;}
 j.executable_size=pkg.executable.length;j.plist_size=pkg.plist.length;
 hash(pkg.executable.bytes,pkg.executable.length,j.executable_hash);hash(pkg.plist.bytes,pkg.plist.length,j.plist_hash);
 phase=4;if((e=mkdir_fixed(S))||(e=mkdir_fixed(SD))||
 (e=create_file(SE,pkg.executable.bytes,pkg.executable.length,0755))||
 (e=create_file(SP,pkg.plist.bytes,pkg.plist.length,0644))||
 (e=identity(S,&j.custom))||(e=custom(&j,0))||(e=sync_path(SD))||
 (e=sync_path(S))||(e=sync_path(P)))goto out;
 phase=5;seal(&j);if((e=create_file(J,&j,sizeof(j),0600))||(e=sync_path(P)))goto out;
 /* Journal is durable; every subsequent failure attempts original-layout recovery. */
 phase=6;if((e=move(A,B,&j.original))||(e=move(S,A,&j.custom))||
 (j.disabled&&(e=move(D,V,&j.virtual)))||(e=custom(&j,1))||
 (e=same(V,&j.virtual))||(e=create_file(C,j.digest,32,0600))||
 (e=sync_path(P))){
 /* Preserve durable rollback intent even when commit write succeeded but its
  * readback/parent sync failed. Still attempt restore if recording intent fails. */
 (void)rollback_intent(&j);recovery=restore(&j);goto out;}
 out:
 printf("NORD2 AUDIO INSTALL: mode=%d phase=%d error=%d recovery=%d\n",m,phase,e,recovery);
 return e?e:((recovery>1)?recovery:0);
}

