//系统权限管理

#define _XOPEN_SOURCE 500  // 启用POSIX扩展功能
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <shadow.h>
#include <crypt.h>
#include <time.h>

#include "system_permission.h"

//需要创建的组列表
const char *GroupDevicesList[]={
	"tpNetwork"
	"tpBluetooth",
	"tpUsb"
	"tpCamera"
	//...在此处新增
	"NULL"
};



#define PASSWD_FILE      "/etc/passwd"
#define SHADOW_FILE      "/etc/shadow"
#define GROUP_FILE       "/etc/group"
#define LINE_BUF_LEN     2048
#define MAX_MEMBERS      100
#define MAX_CREDS        64
#define PATH_MAX		2048

/* 通用读/写文件及锁 */
static int lock_and_read(const char *path, char ***lines_out, size_t *n_out) {
    int fd = open(path, O_RDWR|O_CLOEXEC);
    if (fd < 0) return -errno;
    if (flock(fd, LOCK_EX) < 0) { close(fd); return -errno; }
    FILE *fp = fdopen(fd, "r");
    if (!fp) { flock(fd, LOCK_UN); close(fd); return -errno; }
    char **lines = NULL;
    char buf[LINE_BUF_LEN]; size_t n=0;
    while (fgets(buf, sizeof(buf), fp)) {
        char *dup = strdup(buf);
        if (!dup) { fclose(fp); return -ENOMEM; }
        char **tmp = realloc(lines, (n+1)*sizeof(*tmp));
        if (!tmp) { free(dup); fclose(fp); return -ENOMEM; }
        lines = tmp; lines[n++] = dup;
    }
    *lines_out = lines; *n_out = n;
    rewind(fp);
    return fd;
}

static int write_and_unlock(int fd, char **lines, size_t n, const char *orig_path) {
    char tmp_path[PATH_MAX]; 
	snprintf(tmp_path, sizeof(tmp_path), "%s.tmp", orig_path);
    FILE *fp = fopen(tmp_path, "w");
    if (!fp) { flock(fd, LOCK_UN); close(fd); return -errno; }
    for (size_t i=0;i<n;i++) fputs(lines[i], fp);
    fflush(fp); fsync(fileno(fp)); fclose(fp);
    if (rename(tmp_path, orig_path) < 0) { flock(fd, LOCK_UN); close(fd); return -errno; }
    flock(fd, LOCK_UN); close(fd);
    return 0;
}

/* 分割字段 */
static int split(char *line, char **fields, int max) {
    int i=0; char *p=line;
    while (i<max-1 && (fields[i]=strsep(&p, ":"))) i++;
    fields[i]=p; return i+1;
}

/* 查找下一个可用 UID/GID */
static uid_t next_available_uid(void) {
    struct passwd *pw; uid_t max=1000;
    setpwent(); while((pw=getpwent())) if (pw->pw_uid>=max) max=pw->pw_uid+1;
    endpwent(); return max;
}
static gid_t next_available_gid(void) {
    struct group *gr; gid_t max=1000;
    setgrent(); while((gr=getgrent())) if (gr->gr_gid>=max) max=gr->gr_gid+1;
    endgrent(); return max;
}

/* ===== group 操作 ===== */

static int create_group(const char *groupname, gid_t gid) {
    if (!groupname) return EINVAL;
    if (!gid) gid = next_available_gid();
    char **lines; size_t n; int fd = lock_and_read(GROUP_FILE, &lines, &n);
    if (fd<0) return fd;
    for (size_t i=0;i<n;i++) {
        char *f[4]; char *cp=strdup(lines[i]); split(cp,f,4);
        if (strcmp(f[0],groupname)==0) { free(cp); return -EEXIST; }
        free(cp);
    }
    char buf[LINE_BUF_LEN]; snprintf(buf,sizeof(buf),"%s:x:%d:\n",groupname,(int)gid);
    lines = realloc(lines,(n+1)*sizeof(*lines)); lines[n]=strdup(buf); n++;
    int ret=write_and_unlock(fd, lines, n, GROUP_FILE);
    for (size_t i=0;i<n;i++) free(lines[i]); free(lines);
    return ret;
}

static int delete_group(const char *groupname) {
    if (!groupname) return EINVAL;
    /* 禁止删除含有主组用户 */
    struct passwd *pw;
    setpwent(); while((pw=getpwent())) if(pw->pw_gid==getgrnam(groupname)->gr_gid) { endpwent(); return -EPERM; }
    endpwent();
    char **lines; size_t n; int fd=lock_and_read(GROUP_FILE,&lines,&n);
    if (fd<0) return fd;
    size_t m=0;
    for (size_t i=0;i<n;i++) {
        char *f[4]; char *cp=strdup(lines[i]); split(cp,f,4);
        if (strcmp(f[0],groupname)!=0) lines[m++]=lines[i]; else free(lines[i]);
        free(cp);
    }
    int ret=write_and_unlock(fd,lines,m,GROUP_FILE);
    for (size_t i=m;i<n;i++) free(lines[i]); free(lines);
    return ret;
}

int add_user_to_group(const char *groupname,const char *username) {
    char **lines; size_t n; int fd=lock_and_read(GROUP_FILE,&lines,&n);
    if(fd<0) return fd;
    for(size_t i=0;i<n;i++){
        char *f[4]; char *cp=strdup(lines[i]); split(cp,f,4);
        if(strcmp(f[0],groupname)==0){
            /* 拆成员 */
            char *mems=f[3]; char *tok, *save; char *arr[MAX_MEMBERS]; int cnt=0;
            for(tok=strtok_r(mems,",\n",&save);tok;tok=strtok_r(NULL,",\n",&save)) arr[cnt++]=tok;
            /* 插入 */ bool found=false;
            for(int j=0;j<cnt;j++) if(strcmp(arr[j],username)==0) found=true;
            if(!found && cnt<MAX_MEMBERS) arr[cnt++]=(char*)username;
            /* 重构行 */
            char buf[LINE_BUF_LEN]; size_t p=0;
            p+=snprintf(buf+p,sizeof(buf)-p,"%s:%s:%s:",f[0],f[1],f[2]);
            for(int j=0;j<cnt;j++) p+=snprintf(buf+p,sizeof(buf)-p,"%s%s",arr[j],j+1<cnt?",":"");
            buf[p++]='\n'; buf[p]=0;
            free(lines[i]); lines[i]=strdup(buf);
            free(cp); break;
        }
        free(cp);
    }
    int ret=write_and_unlock(fd,lines,n,GROUP_FILE);
    for(size_t i=0;i<n;i++) free(lines[i]); free(lines);
    return ret;
}

int remove_user_from_group(const char *groupname,const char *username){
    char **lines; size_t n; int fd=lock_and_read(GROUP_FILE,&lines,&n);
    if(fd<0)return fd;
    for(size_t i=0;i<n;i++){
        char *f[4]; char *cp=strdup(lines[i]); split(cp,f,4);
        if(strcmp(f[0],groupname)==0){
            char *mems=f[3]; char *tok,*save; char *arr[MAX_MEMBERS]; int cnt=0;
            for(tok=strtok_r(mems,",\n",&save);tok;tok=strtok_r(NULL,",\n",&save)) if(strcmp(tok,username)!=0) arr[cnt++]=tok;
            char buf[LINE_BUF_LEN]; size_t p=0;
            p+=snprintf(buf+p,sizeof(buf)-p,"%s:%s:%s:",f[0],f[1],f[2]);
            for(int j=0;j<cnt;j++) p+=snprintf(buf+p,sizeof(buf)-p,"%s%s",arr[j],j+1<cnt?",":"");
            buf[p++]='\n'; buf[p]=0;
            free(lines[i]); lines[i]=strdup(buf);
            free(cp); break;
        }
        free(cp);
    }
    int ret=write_and_unlock(fd,lines,n,GROUP_FILE);
    for(size_t i=0;i<n;i++) free(lines[i]); free(lines);
    return ret;
}

/* ===== user 操作 ===== */
/**
 * 创建用户接口
 * @param username 登录用户名
 * @param password 明文密码（将使用 crypt 加密后写入 /etc/shadow）
 * @param shell 用户登录后默认 shell（例如 "/bin/bash"）
 * @return 0 成功，负值为错误码
 */
static int create_user(const char *username,const char *password,const char *shell){
    if(!username||!password) 
		return EINVAL;
    /* passwd 文件 */
    char **pl; 
	size_t pn; 
	int pfd=lock_and_read(PASSWD_FILE,&pl,&pn);
    if(pfd<0) 
		return pfd;
    uid_t uid=next_available_uid(); 
	gid_t gid=next_available_gid();
    char home[256]; 
	snprintf(home,sizeof(home),"/home/%s",username);
    char entry[LINE_BUF_LEN]; 
	snprintf(entry,sizeof(entry),"%s:x:%d:%d::%s:%s\n",username,(int)uid,(int)gid,home,shell);
    pl=realloc(pl,(pn+1)*sizeof(*pl)); 
	pl[pn]=strdup(entry); 
	pn++;
    int pr=write_and_unlock(pfd,pl,pn,PASSWD_FILE);
    for(size_t i=0;i<pn;i++) 
		free(pl[i]); 
	free(pl);
    if(pr) 
		return pr;
    /* shadow 文件 */
    char **sl; 
	size_t sn; 
	int sfd=lock_and_read(SHADOW_FILE,&sl,&sn);
    if(sfd<0) 
		return sfd;
    time_t now=time(NULL)/86400;
    snprintf(entry,sizeof(entry),"%s:%s:%ld:0:99999:7:::\n",username, password[0] ? password : "!",now);
    sl=realloc(sl,(sn+1)*sizeof(*sl)); 
	sl[sn]=strdup(entry); sn++;
    int sr=write_and_unlock(sfd,sl,sn,SHADOW_FILE);
    for(size_t i=0;i<sn;i++) 
		free(sl[i]); 
	free(sl);
    return sr;
}

static int delete_user(const char *username){
    if(!username) return EINVAL;
    /* passwd */
    char **pl; size_t pn; int pfd=lock_and_read(PASSWD_FILE,&pl,&pn);
    if(pfd<0) return pfd;
    size_t m=0;
    for(size_t i=0;i<pn;i++){
        char *f[7]; 
		char *cp=strdup(pl[i]); 
		split(cp,f,7);
        if(strcmp(f[0],username)!=0) 
			pl[m++]=pl[i]; 
		else 
			free(pl[i]);
        free(cp);
    }
    int pr=write_and_unlock(pfd,pl,m,PASSWD_FILE);
    free(pl);
    if(pr) 
		return pr;
    /* shadow */
    char **sl; 
	size_t sn; 
	int sfd=lock_and_read(SHADOW_FILE,&sl,&sn);
    if(sfd<0) 
		return sfd;
    m=0;
    for(size_t i=0;i<sn;i++){
        char *f[2]; char *cp=strdup(sl[i]); split(cp,f,2);
        if(strcmp(f[0],username)!=0) sl[m++]=sl[i]; else free(sl[i]);
        free(cp);
    }
    int sr=write_and_unlock(sfd,sl,m,SHADOW_FILE);
    free(sl);
    /* 同名私有组 */
    delete_group(username);
    return sr;
}

/* 修改用户：示例修改 shell */
static int modify_user_shell(const char *username,const char *newshell){
    if(!username||!newshell) return EINVAL;
    char **pl; size_t pn; int pfd=lock_and_read(PASSWD_FILE,&pl,&pn);
    if(pfd<0) return pfd;
    for(size_t i=0;i<pn;i++){
        char *f[7]; char *cp=strdup(pl[i]); split(cp,f,7);
        if(strcmp(f[0],username)==0) {
            snprintf(cp,sizeof(cp),"%s:%s:%s:%s:%s:%s:%s\n",
                     f[0],f[1],f[2],f[3],f[4],f[5],newshell);
            free(pl[i]); pl[i]=strdup(cp);
            free(cp);
            break;
        }
        free(cp);
    }
    int pr=write_and_unlock(pfd,pl,pn,PASSWD_FILE);
    free(pl);
    return pr;
}


/* ===== 新增接口 ===== */
/**
 * 获取组内所有用户
 * @param groupname 组名
 * @param users 输出数组，调用方需保证 users 至少大小为 max_users
 * @param max_users 最大容量
 * @return 成员数量，负值表示错误
 */
static int get_group_users(const char *groupname, char **users, int max_users) {
    if (!groupname || !users) return -EINVAL;
    char **lines; size_t n; int fd = lock_and_read(GROUP_FILE, &lines, &n);
    if (fd < 0) return fd;
    int count = 0;
    for (size_t i = 0; i < n && count < max_users; ++i) {
        char *fields[4]; char *cp = strdup(lines[i]);
        split(cp, fields, 4);
        if (strcmp(fields[0], groupname) == 0) {
            char *tok, *save;
            for (tok = strtok_r(fields[3], ",\n", &save);
                 tok && count < max_users;
                 tok = strtok_r(NULL, ",\n", &save)) {
                users[count++] = strdup(tok);
            }
            free(cp);
            break;
        }
        free(cp);
    }
    write_and_unlock(fd, lines, n, GROUP_FILE);
    for (size_t i = 0; i < n; ++i) free(lines[i]); free(lines);
    return count;
}

/**
 * 获取用户所属所有组
 * @param username 用户名
 * @param groups 输出数组，调用方需保证 groups 至少大小为 max_groups
 * @param max_groups 最大容量
 * @return 组数量，负值表示错误
 */
static int get_user_groups(const char *username, char **groups, int max_groups) {
    if (!username || !groups) return -EINVAL;
    char **lines; size_t n; int fd = lock_and_read(GROUP_FILE, &lines, &n);
    if (fd < 0) return fd;
    int count = 0;
    for (size_t i = 0; i < n && count < max_groups; ++i) {
        char *fields[4]; char *cp = strdup(lines[i]);
        split(cp, fields, 4);
        bool matched = false;
        if (strcmp(fields[0], username) == 0) matched = true;
        else {
            char *tok, *save;
            for (tok = strtok_r(fields[3], ",\n", &save);
                 tok; tok = strtok_r(NULL, ",\n", &save)) {
                if (strcmp(tok, username) == 0) { matched = true; break; }
            }
        }
        if (matched) {
            groups[count++] = strdup(fields[0]);
        }
        free(cp);
    }
    write_and_unlock(fd, lines, n, GROUP_FILE);
    for (size_t i = 0; i < n; ++i) free(lines[i]); free(lines);
    return count;
}



//初始化组，没有则创建
int system_group_devices_init(SystemPermission *system)
{
	for(int i=0;;i++)
	{
		if(!GroupDevicesList[i])
			break;
		if (create_group(GroupDevicesList[i], 0) != 0) {
        	fprintf(stderr,"group:%s creste error\n",GroupDevicesList[i]);
    	}
	}
	return 0;
}

SystemPermission *system_permission_manage_create()
{
	SystemPermission *system=(SystemPermission *)malloc(sizeof(SystemPermission));
	if(!system)
		return NULL;
//	system_group_devices_init(system);
	system->create_group=create_group;
	system->delete_group=delete_group;
//	system->create_user=create_user_by_groupname;
//	system->delete_user=delete_user_by_groupname;
	system->add_user_to_group=add_user_to_group;
	system->del_user_from_group=remove_user_from_group;
	return system;
}

int system_permission_manage_delete(SystemPermission *system)
{
	if(!system)
		return 0;
	free(system);
	return 0;
}


