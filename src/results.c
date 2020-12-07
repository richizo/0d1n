#include "results.h"
#include "mem_ops.h"
#define TEMPLATE_RES "/opt/0d1n/templates/template.conf"
#define RESPONSE_PATH "/opt/0d1n/view/response_sources"

void write_result(
 char * chunk,
 char *pathtable,
 char *line,
 char * make, 
 char * make_agent, 
 char * make_cookie,
 long status,
 long length
)
{
	bool match_string=false; 
	char randname[32],log[3048],tabledata[4086],pathsource[1024],pathurl[1024];
	char *find_list = NULL, *ptr_line = NULL, *delim = "\n";

	// param.find_regex_list  list to find with regex , param.find_string_list list without regex
		if(  (param.find_string_list) || (param.find_regex_list)  )
		{
			find_list = readLine((param.find_string_list!=NULL)?param.find_string_list:param.find_regex_list);
			ptr_line = strtok(find_list, delim);

			while (ptr_line != NULL) 
			{
				chomp(ptr_line);
				char *tmp_make_cookie = NULL,*tmp_make_agent = NULL;
				char *tmp_make = NULL,*tmp_line = NULL,*tmp_ptr_line = NULL,*tmp_response = NULL,*responsetemplate = NULL;

				// find a string in response
				if (status != 404)
				{
					if (param.find_string_list != NULL )
						match_string = strstr(chunk,ptr_line)?true:false;

					if (param.find_regex_list != NULL )
						match_string = strstr_regex(chunk,ptr_line)?true:false;
				}
				if (chunk && (match_string == true) ) 
				{
					if (make_cookie!=NULL)
						fprintf(stdout,"%s [ %s %ld %s ] Payload: %s %s %s Grep: %s %s %s  Params: %s \nCookie: %s %s\n",YELLOW,CYAN,status,YELLOW,GREEN,line,YELLOW,CYAN,ptr_line,YELLOW,make,make_cookie,LAST);
					

					if (make_agent!=NULL)
						fprintf(stdout,"%s [ %s %ld %s ] Payload: %s %s %s Grep: %s %s %s  Params: %s \nCookie: %s %s\n",YELLOW,CYAN,status,YELLOW,GREEN,line,YELLOW,CYAN,ptr_line,YELLOW,make,make_agent,LAST);
					else 
						fprintf(stdout,"%s [ %s %ld %s ] Payload: %s %s %s Grep: %s %s %s  Params: %s %s\n",YELLOW,CYAN,status,YELLOW,GREEN,line,YELLOW,CYAN,ptr_line,YELLOW,make,LAST);
					

					if (param.save_response==true && status != 404)
					{
// create responses path
						memset(pathsource,0,sizeof(char)*1023);
						strlcat(pathsource,RESPONSE_PATH,45);
						strlcat(pathsource,"/",47);
						strlcat(pathsource,param.log, 78);
						mkdir(pathsource,S_IRWXU|S_IRWXG|S_IRWXO);
 						entropy_clock();
						char *str_tmp_rng = rand_str(randname, sizeof randname);
						snprintf(pathsource,986,"%s/%s/%s.html",RESPONSE_PATH,param.log,str_tmp_rng);
						snprintf(pathurl,986,"response_sources/%s/%s.html",param.log,str_tmp_rng);
					}

					// write log file
					if(status != 404)
					{
						snprintf(log,3047,"[ %ld ] Payload: %s  Grep: %s Params: %s cookie: %s  UserAgent: %s \n Path Response Source: %s\n",status,line,ptr_line,make,(make_cookie!=NULL)?make_cookie:" ",(make_agent!=NULL)?make_agent:" ",pathsource);
						WriteFile(param.log,log);
						memset(log,0,3047);	
					}


					if (param.save_response==true && status != 404)
					{
// write highlights response
						responsetemplate = NULL;
                				responsetemplate = readLine(TEMPLATE_RES);
						WriteFile(pathsource,responsetemplate);
						memset(responsetemplate,0,strlen(responsetemplate)-1);
						tmp_response = NULL;
						tmp_response = html_entities(chunk);
						WriteFile(pathsource,tmp_response);
				//		memset(tmp_response,0,strlen(tmp_response)-1);
						WriteFile(pathsource,"</pre></html>");
					}


// create datatables	
					tmp_make = html_entities(make);
					tmp_ptr_line = html_entities(ptr_line);
					tmp_line = html_entities(line);

					if (make_cookie!=NULL)
					{
						tmp_make_cookie = html_entities(make_cookie);
						snprintf(tabledata,4085,"[\"<a class=\\\"fancybox fancybox.iframe\\\" href=\\\"../%s\\\">%ld </a>\",\"%ld\",\"%s cookie: %s\",\"%s\",\"%s\"],\n",pathurl,status,length,tmp_make,tmp_make_cookie,tmp_ptr_line,tmp_line);
						memset(tmp_make_cookie,0,strlen(tmp_make_cookie)-1);
					}

					if (make_agent!=NULL)
					{
						tmp_make_agent = html_entities(make_agent);
						snprintf(tabledata,4085,"[\"<a class=\\\"fancybox fancybox.iframe\\\" href=\\\"../%s\\\">%ld </a>\",\"%ld\",\"%s UserAgent: %s\",\"%s\",\"%s\"],\n",pathurl,status,length,tmp_make,tmp_make_agent,tmp_ptr_line,tmp_line);
						memset(tmp_make_agent,0,strlen(tmp_make_agent)-1);
					} else 
						snprintf(tabledata,4085,"[\"<a class=\\\"fancybox fancybox.iframe\\\" href=\\\"../%s\\\">%ld </a>\",\"%ld\",\"%s\",\"%s\",\"%s\"],\n",pathurl,status,length,tmp_make,tmp_ptr_line,tmp_line);

					if (status != 404)					
						WriteFile(pathtable,tabledata);

					// liberate heap
					XFREE(tmp_line);
					XFREE(tmp_make);
					XFREE(tmp_ptr_line);
					XFREE(responsetemplate);
					XFREE(tmp_response);	
					XFREE(tmp_make_cookie);
					XFREE(tmp_make_agent);
	
	
				}
				
				ptr_line = strtok(NULL, delim);
			}
 
			XFREE(find_list);

// if don't have list to find data in response, usually for brute path result
		} else {
			
			char *tmp_line = NULL,*tmp_make = NULL,*tmp_response = NULL,*responsetemplate = NULL;
			char *tmp_make_cookie = NULL,*tmp_make_agent = NULL;

			if (make_cookie != NULL)
				fprintf(stdout,"%s [ %s %ld %s ] Payload: %s %s %s Params: %s %s\n Cookie: %s %s\n",YELLOW,CYAN,status,YELLOW,GREEN,line,YELLOW,CYAN,make,make_cookie,LAST);
			
			if (make_agent != NULL)
				fprintf(stdout,"%s [ %s %ld %s ] Payload: %s %s %s Params: %s %s\n UserAgent: %s %s\n",YELLOW,CYAN,status,YELLOW,GREEN,line,YELLOW,CYAN,make,make_agent,LAST);
			else 
				fprintf(stdout,"%s [ %s %ld %s ] Payload: %s %s %s Params: %s %s %s\n",YELLOW,CYAN,status,YELLOW,GREEN,line,YELLOW,CYAN,make,LAST);
	
			if (param.save_response==true && status != 404)
			{		
				memset(pathsource,0,sizeof(char)*1023);
				strlcat(pathsource,RESPONSE_PATH,45);
				strlcat(pathsource,"/",46);
				strlcat(pathsource,param.log, 60);
				mkdir(pathsource,S_IRWXU|S_IRWXG|S_IRWXO);
				entropy_clock();
				char *str_tmp_rng = rand_str(randname, sizeof randname);
				snprintf(pathsource,986,"%s/%s/%s.html",RESPONSE_PATH,param.log,str_tmp_rng);
				snprintf(pathurl,986,"response_sources/%s/%s.html",param.log,str_tmp_rng);
			}

//write logs
			if(status != 404)
			{
				snprintf(log,3047,"[%ld Payload: %s Params: %s Cookie: %s UserAgent: %s \n Path Response Source: %s\n",status,line,make,(make_cookie!=NULL)?make_cookie:" ",(make_agent!=NULL)?make_agent:" ",pathsource);
				WriteFile(param.log,log);
				memset(log,0,3047);
			}

			if (param.save_response==true && status != 404)
			{
// write response source with highlights
              	 		responsetemplate=readLine(TEMPLATE_RES);
				WriteFile(pathsource,responsetemplate);
				//memset(responsetemplate,0,strlen(responsetemplate)-1);
				tmp_response = html_entities(chunk);
				WriteFile(pathsource,tmp_response);
				//memset(tmp_response,0,strlen(tmp_response)-1);
	
				WriteFile(pathsource,"</pre></html>");
			}

// create datatables
			tmp_make = html_entities(make);
			tmp_line = html_entities(line);

			if (make_cookie != NULL)
			{
				
				tmp_make_cookie = html_entities(make_cookie);
				snprintf(tabledata,4085,"[\"<a class=\\\"fancybox fancybox.iframe\\\" href=\\\"../%s\\\">%ld </a>\",\"%ld\",\"%s  cookie: %s\",\"\",\"%s\"],\n",pathurl,status,length,tmp_make,tmp_make_cookie,tmp_line);
			//	memset(tmp_make_cookie,0,strlen(tmp_make_cookie)-1);
			}

			if (make_agent != NULL)
			{
				
				tmp_make_agent = html_entities(make_agent);
				snprintf(tabledata,4085,"[\"<a class=\\\"fancybox fancybox.iframe\\\" href=\\\"../%s\\\">%ld </a>\",\"%ld\",\"%s  UserAgent: %s\",\"\",\"%s\"],\n",pathurl,status,length,tmp_make,tmp_make_agent,tmp_line);

			} else 
				snprintf(tabledata,4047,"[\"<a class=\\\"fancybox fancybox.iframe\\\" href=\\\"../%s\\\">%ld </a>\",\"%ld\",\"%s\",\"\",\"%s\"],\n",pathurl,status,length,tmp_make,tmp_line);
			if (status != 404)
  				WriteFile(pathtable,tabledata);

			XFREE(tmp_line);
			XFREE(tmp_make);
			XFREE(tmp_response);
			XFREE(responsetemplate);
			XFREE(tmp_make_cookie);
			XFREE(tmp_make_agent);
	
		}
 
}

