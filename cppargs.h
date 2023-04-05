#pragma once
#include <iostream>
#include <vector>
#include <map>

using namespace std;

bool checkit(string control,string data,int curind){
     for(int i = 0; i<control.length();i++)
            if(control[i]!=data[curind+i])
               return false;
     return true;
}

bool isinside(string control,string data){
    for(int i = 0; i<data.length();i++)
            if(control[0]==data[i])
               if(checkit(control,data,i))
                  return true;
     return false;
}


int count(string control,string data){
    int c= 0;
    for(int i = 0; i<data.length();i++)
            if(control[0]==data[i])
               if(checkit(control,data,i))
                  c++;
    return c;
}

int checkitandfindlastindex(string control,string data,int curind){
     int i = 0;
     for(i; i<control.length();i++)
            if(control[i]!=data[curind+i])
               return -1;
     return curind+i;
}

vector<int> find(string control,string data){
     vector<int> rv;
     for(int i = 0; i<data.length();i++)
            if(control[0]==data[i]){
               int resultindex = checkitandfindlastindex(control,data,i);
               if(resultindex>-1){
                 rv.push_back(i);
                 rv.push_back(resultindex);
                 return rv;
               }
            }
     return rv;
}

vector<vector<int>> findall(string control,string data){
     vector<vector<int>> rv;
     for(int i = 0; i<data.length();i++)
            if(control[0]==data[i]){
               int resultindex = checkitandfindlastindex(control,data,i);
               if(resultindex>-1){
                 vector<int> tv;
                 tv.push_back(i);
                 tv.push_back(resultindex);
                 rv.push_back(tv);
               }
            }
     return rv;
}

string replace(string control,string newdata,string data){
     int len = newdata.length();
     vector<int> result = find(control,data);
     for(int i=0;i<len;i++){
         data[i+result[0]]=newdata[i];
      }
     return data;
}

string replaceall(string control,string newdata,string data){
     int len = newdata.length();
     vector<vector<int>> result = findall(control,data);
     for(auto& resultitem:result){
        for(int i=0;i<len;i++){
           data[i+resultitem[0]]=newdata[i];
        }
     }
     return data;
}


string getpart(string data,int start,int end){
    string rv;
    for(int i = start; i <end;i++){
            rv.push_back(data[i]);
    }
    return rv;

}

vector<string> split(string control,string data){
       vector<string> rv;
       int c = 0;
       vector<vector<int>> result = findall(control,data);
       for(auto& resultitem:result){
          rv.push_back(getpart(data,c,resultitem[0]));
          c=resultitem[1];
       }
       rv.push_back(getpart(data,c,data.length()));
       return rv;  

}

string join(string control,vector<string> data){
       string rv;
       for(int i=0;i<data.size();i++){
           rv+=data[i];
           if(i!=data.size()-1)
             rv+=control;
       }
       return rv;
}


map<string,string> parseArguments(string args){
    map<string,string> optmap;
    vector<string> rr = split("-",args);
    rr.erase(rr.begin());
    vector<vector<string>> opt;
    for(auto& arg :rr){
     vector<string> res= split("=",arg);
     if(res.size()==1){
      res.push_back("true");
     }
     opt.push_back(res);
    }
    for(auto& op :opt){
      optmap[op[0]]=op[1];
    }
    return optmap;
}

string prepareArgs(int argc,char** argv){
    vector<string> argslist;
    for(int i=1;i<argc;i++)
        argslist.push_back(argv[i]);
    return join(" ",argslist);
}

bool iskeyinmap(string key,map<string,string> datamap){
    for(auto& item:datamap)
       if(item.first==key)
         return 1;
    return false;
}

/*
    map<string,string> optdict = parseArguments(prepareArgs(argc,argv));
    for(auto& item :optdict)
        cout<<optdict[item.first]<<end;
*/