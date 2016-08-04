
#include "tinyhtml.h"
#include <fstream>
#include <iterator>
#include <iostream>
#include <regex>
#include <json/json.h>
using namespace std;
using namespace TinyHTML;

void test_root_tag(string fstr, const std::vector<TagInfo>& tagInfos)

{
    ifstream file;
    file.open(fstr);
    std::istreambuf_iterator<char> iter_begin = std::istreambuf_iterator<char>(file);
    std::string fileData(iter_begin,
        std::istreambuf_iterator<char>());
    //cout<<fileData<<std::endl;
    auto html = std::shared_ptr<HTML>(new HTML(fileData));
 
    /*for(auto taginfo: tagInfos)
    {
        if(taginfo.findType == eSingle){
            tinyTag = html->find(taginfo.name, taginfo.attributs);
            cout<<tinyTag->getContentText()<<endl;
        }else{
            auto tags = tinyTag->findAll("li",taginfo.attributs);
            for(auto tag: tags)
            {
                cout<<tag->getContentText()<<endl;
            }            
        }
    }
    return;*/
    auto tinyTag = html->find("div", {{"id","phrsListTab"}, {"class", "trans-wrapper clearfix"}});
    if(tinyTag)
    {
        cout<<"================="<<endl;
        cout<<tinyTag->getContentText()<<endl;
        cout<<"================="<<endl;
        auto tags = tinyTag->findAll("li");
        for(auto tag: tags){
            cout<<"tag "<<tag->getName()<<" content "<<tag->getContentText()<<endl;
        }
    }
    else{
        tinyTag = html->find("div", {{"class", "error-typo"}});
        if(tinyTag){
            auto h4Tag = tinyTag->find("h4");
            
            if(h4Tag)
            {
                cout<<"tag "<<h4Tag->getName()<<" content "<<h4Tag->getContentText()<<endl;
                auto tags = tinyTag->findAll("a");
                for(auto tag: tags){
                    cout<<"tag "<<tag->getName()<<" content "<<tag->getContentText()<<endl;
                }
            }
        }
    }
}

void test_json(){
    ifstream file;
    file.open("config.txt");
    std::istreambuf_iterator<char> iter_begin = std::istreambuf_iterator<char>(file);
    std::string fileData(iter_begin,
        std::istreambuf_iterator<char>());
        
    Json::Reader reader;    
    Json::Value root;
    std::vector<TagInfo> taginfos;
    if (reader.parse(fileData, root))       
    {
        for(auto item: root)
        {
            TagInfo taginfo;
            taginfo.name = item["tag"].asString();
            taginfo.findType = (EfindType)item["findType"].asInt();
            Attribute attribute;
           for(auto i=0; i< item["attrs"].size();i++)
           {
                if(i%2==0){
                    attribute.name = item["attrs"][i].asString();
                }else{
                    attribute.value = item["attrs"][i].asString();
                    taginfo.attributs.push_back(attribute);
                }
                
           }
          
           taginfos.push_back(taginfo);
        }
    }
    test_root_tag("main.html", taginfos);    
}
int main()
{
    //test_root_tag("main.html");/* */
    //html->find("div", {{"id","examplesToggle"}});
    test_json();
}
