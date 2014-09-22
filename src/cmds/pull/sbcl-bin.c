#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "pull.h"
#include "opt.h"

extern int sbcl_install(char* impl,char* version);

char* arch(void) {
  return s_cat(uname_m(),q("-"),uname(),NULL);
}

char* sbcl_bin(char* file);

char* sbcl_version_bin(char* impl,char* version)
{
  char* home= homedir();
  char* platforms_html=cat(home,"tmp",SLASH,"sbcl.html",NULL);
  ensure_directories_exist(platforms_html);

  /* TBD */
  if(version) {
    s(platforms_html);
    return s_cat(q(version),q("-"),arch(),NULL);
  }else {
    char* version;
    char* ret;
    printf("version not specified\nto specify version,downloading platform-table.html...");
    download_simple("http://www.sbcl.org/platform-table.html",platforms_html,0);
    printf("done\n");
    version=sbcl_bin(platforms_html);
    printf("version to install would be '%s'\n",version);
    ret = s_cat(version,q("-"),arch(),NULL);
    s(platforms_html);
    return ret;
  }
}

char* sbcl_bin_extention(char* impl,char* version) {
#ifdef _WIN32
  return "msi";
#else
  return "tar.bz2";
#endif
}

char* sbcl_uri_bin(char* impl,char* version)
{
  /*should I care about it's existance? */
  return cat("http://prdownloads.sourceforge.net/sbcl/sbcl-",version,
	     "-binary.",
             sbcl_bin_extention(impl,version)
             ,NULL);
}

#ifdef _WIN32
int sbcl_bin_expand(char* impl,char* version){
  int ret;
  char* home= homedir();
  char* archive=cat(impl,"-",version,".msi",NULL);
  char* log_path=cat(home,"impls",SLASH,"log",SLASH,impl,"-",version,SLASH,"install.log",NULL);
  char* dist_path;
  int pos=position_char("-",impl);
  if(pos!=-1) {
    impl=subseq(impl,0,pos);
  }else
    impl=q(impl);
  version=q(version);
  dist_path=cat(home,"src",SLASH,impl,"-",version,SLASH,NULL);
  printf("Extracting archive. %s to %s\n",archive,dist_path);
  archive=s_cat(q(home),q("archives"),q(SLASH),archive,NULL);
  delete_directory(dist_path,1);
  ensure_directories_exist(dist_path);
  ensure_directories_exist(log_path);

  char* cmd=cat("start /wait msiexec.exe /a \"",
                archive,
                "\" targetdir=\"",
                dist_path,
                "\" /qn /lv ",
                "\"",
                log_path,
                "\"",
                NULL);
  ret=system(cmd);
  s(impl);
  s(dist_path);
  s(log_path);
  s(archive);
  s(cmd),s(home),s(version);
  return !ret;
}
int sbcl_bin_install(char* impl,char* version) {
  char* home= homedir();
  char* str;
  char* version_num= subseq(version,0,position_char("-",version));
  str=cat("echo f|xcopy \"",
          home,"src\\sbcl-",version,"\\PFiles\\Steel Bank Common Lisp\\",version_num,"\\sbcl.exe\" \"",
          home,"impls\\sbcl-",version,"\\bin\\sbcl.exe\" >NUL",NULL);
  system(str),s(str);
  str=cat("echo f|xcopy \"",
          home,"src\\sbcl-",version,"\\PFiles\\Steel Bank Common Lisp\\",version_num,"\\sbcl.core\" \"",
          home,"impls\\sbcl-",version,"\\lib\\sbcl\\sbcl.core\" >NUL",NULL);
  system(str),s(str);
  str=cat("echo d|xcopy \"",
          home,"src\\sbcl-",version,"\\PFiles\\Steel Bank Common Lisp\\",version_num,"\\contrib\" \"",
          home,"impls\\sbcl-",version,"\\lib\\sbcl\\contrib\" >NUL",NULL);
  system(str),s(str);
  s(home);
}
#endif

install_cmds install_sbcl_bin_full[]={
  start,
  download,
#ifndef _WIN32
  expand,
  sbcl_install,
#else
  sbcl_bin_expand,
  sbcl_bin_install,
#endif
  NULL
};

struct install_impls impls_sbcl_bin={ "sbcl-bin", install_sbcl_bin_full,sbcl_version_bin,sbcl_uri_bin,sbcl_bin_extention};
