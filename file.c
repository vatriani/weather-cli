/*
 * file.c - for file, config and cache handling
 * Copyright © 2013 - Niels Neumann  <vatriani.nn@googlemail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __FILE_H__
#define __FILE_H__

#ifndef __OUTPUT_H__
 #include "output.c"
#endif

#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <string.h>

static void getUserPath(char **path) {
 struct passwd *userInformations;
 int len;

 userInformations=getpwuid(getuid());
 len=strlen(userInformations->pw_dir);
 *path=malloc(++len);
 memcpy(*path,userInformations->pw_dir,len+1);
}

static void createPathIfNotExists(char **path) {
 DIR *tmp;
 tmp=opendir(*path);
 if(tmp==NULL)
  mkdir(*path,0700);
 else closedir(tmp);
}

extern void getConfigPath(char **path,char *app_name) {
 getUserPath(path);
 if(strlen(*path)!=0) {
  strmcat(path,"/.config/");
  strmcat(path,app_name);
  strmcat(path,"/");
  createPathIfNotExists(path);
 }
}

extern void getCachePath(char **path,char *app_name) {
 getUserPath(path);
 if(strlen(*path)!=0) {
  strmcat(path,"/.cache/");
  strmcat(path,app_name);
  strmcat(path,"/");
  createPathIfNotExists(path);
 }
}

/*
void loadConfig(char **path) {
}

void saveConfig(char **path) {
}
*/
#endif