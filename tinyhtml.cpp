#include "tinyhtml.h"
#include <iostream>
#include <assert.h>
#include <regex>
#include <list>
using namespace std;
using namespace TinyHTML;
TagBuilder TagBuilder::tagBuilder;
const TinyResult NullResult;

template<class T>
static T& Trim(T&& text)
{
    if(!text.empty())  
    {  
        text.erase(0, text.find_first_not_of(" \n\r\t"));  
        text.erase(text.find_last_not_of(" \n\r\t") + 1);  
    }  
    return text;  
}

std::shared_ptr<TinyTag> TagBuilder::dump()
{
    assert(!_data.empty());

    //root
    auto tagName = extractTagName(_data);
    std::shared_ptr<TinyTag> tinyTag(new TinyTag(std::get<0>(tagName)));
    auto pos = std::get<1>(tagName);
    extractTag(_data.substr(pos), pos, tinyTag);
    return tinyTag;
}


std::shared_ptr<TinyTag> TagBuilder::dump( const std::string &data)
{
    //root
    auto tagName = extractTagName(data);
    std::shared_ptr<TinyTag> tinyTag(new TinyTag(std::get<0>(tagName)));
    auto pos = std::get<1>(tagName);
    extractTag(data.substr(std::get<1>(tagName)), pos, tinyTag);
    return tinyTag;
}

void TagBuilder::pushTagsInfo(const TinyResult &tr)
{
    
    auto tn = std::get<0>(tr);
    auto pos = std::get<1>(tr)-tn.size()-1;
    
    _tmpTagsName.push_back(std::make_tuple(tn, pos));
}
void TagBuilder::popTagsInfo()
{
    _tmpTagsName.pop_back();
}
            
void TagBuilder::setData(const std::string &data)
{
    _data = data;
}

std::map<std::string, std::string>
TagBuilder::extractAttrs(const std::string &attrsText){

    
    std::map<std::string, std::string> attrs;
    const size_t size = attrsText.size();
    size_t pos = 0;
    string attrName;
    string attrValue;
    for(size_t i = 0; i < size; i++){
    
        if(attrsText[i] == ' ' && attrName.empty()){
            pos=i+1;
            continue;
        }
        
        if(attrsText[i] == '='){
            attrName = attrsText.substr(pos, i-pos);
            pos = i+1;
        }
        
        if(attrsText[i]==' ' || i == size-1){
            if(!attrName.empty())
            {
                attrValue = attrsText.substr(pos, i-pos+1);
                attrs.insert(std::make_pair(attrName, attrValue));
                attrName="";
                attrValue="";
            }
            pos = i+1;
        }
    }
    return attrs;
}

int TagBuilder::extractTag(const std::string &data,
                            int pos, 
                            std::shared_ptr<TinyTag> &tinyTag)
{
    
    pushTagsInfo(std::make_tuple(tinyTag->getName(),
                 pos-1-tinyTag->getName().size()));
                 
    auto attr_content = extractTagAttrsText(data);
     
    auto attrs = extractAttrs(std::get<0>(attr_content));
    tinyTag->setAttrs(attrs);
    // content string
    auto contentData = data.substr(std::get<1>(attr_content));
    auto ret = extractContentText(contentData, 
                                  pos + std::get<1>(attr_content),
                                  tinyTag);
    popTagsInfo();
    return std::get<1>(ret)+ std::get<1>(attr_content);
}
TinyResult TagBuilder::extractTagAttrsText(const std::string &data)
{
    const size_t size = data.size();
    for(size_t i=0; i < size; i++)
    {
        if(data[i] == '/' && data[i+1]=='>')
        {
            return std::make_tuple(data.substr(0,i), i+2);
        }
        if(data[i]=='>')
        {
            return std::make_tuple(data.substr(0,i), i+2);
        }
    }
    return NullResult;
}

void ShowContainer(const std::vector<TinyResult> &c)
{
    for(auto it: c)
    {
        cout<<"===========: "<<std::get<0>(it)<<endl;
    }
}
TinyResult TagBuilder::brother(const std::string &data)
{
    const auto size =  data.size();
    for(size_t i = 0; i < size; i++)
    {
        if(data[i]=='<'){
            if(data[i+1] != '/'){
                auto ret = extractTagName(data.substr(i));
                return std::make_tuple(std::get<0>(ret), i+std::get<1>(ret));
            }else{
                break;
            }
        }
    }
    return NullResult;
}

TinyResult TagBuilder::endTag(const std::string &data)
{
    const size_t size = data.size();
    size_t i = 0;
    for(; i < size; i++){
    
        // end tag type 1
        if(data[i] == '/' && data[i+1] == '>'){
            _tmpTagsName.pop_back();
            return std::make_tuple(EN_BACK, i);
        }
        
        // end tag type 2
        if(data[i] == '<' && data[i+1] == '/') 
        {
            //cout<<"found end tag: "<<data.substr(i)<<endl;
            std::string endTag;
            for(size_t j = i+2; j < size; j++){
                if(data[j] == '>'){
                    if(std::get<0>(_tmpTagsName.back()) == endTag)
                    {
                        return std::make_tuple(EN_CUR_TAG, i);
                    }
                    else
                    {
                         
                        while(!_tmpTagsName.empty())
                        {

                            _tmpTagsName.pop_back();

                            if(std::get<0>(_tmpTagsName.back()) == endTag)
                            {
                                break;
                            }
                        }
                        return std::make_tuple(EN_BACK, i);
                    }
                    endTag = "";
                    break;
                }
                if(data[j] != ' '){
                    endTag += data[j];
                }
            }
        }
    }
    return std::make_tuple(EN_NULL, i);
}


TinyResult TagBuilder::newTag(const std::string &data,TinyTagPtr &tinyTag){
    const size_t size = data.size();
    size_t i = 0;
    for(; i < size; i++){
        if(data[i] == '<' 
               && data[i+1] != '!'
               &&data[i+1] != '/')
            {
                auto ret = extractTagName(data.substr(i));
                i += std::get<1>(ret);
                auto tagName = std::get<0>(ret);
                cout<<"newTag: "<<tagName<< " pos "<<i<<endl;
                //ShowContainer(_tmpTagsName);
                std::shared_ptr<TinyTag> tg(new TinyTag(tagName));
                tinyTag->addChild(tg);
                //cout<<"Tag "<<tg->getName() <<" data :"<<data.substr(i)<<endl;
                i += extractTag(data.substr(i), i, tg);
                // check the brother tags
                ret = brother(data.substr(i));
                while(ret != NullResult)
                {
                    cout<<"new brother"<<endl;
                    std::shared_ptr<TinyTag> ttg(new TinyTag(std::get<0>(ret)));
                    tinyTag->addChild(ttg);
                    i += std::get<1>(ret);
                    i += extractTag(data.substr(i), i, ttg);
                    ret = brother(data.substr(i));
                }
                cout<<"no brother"<<" index "<<i<<endl;
                cout<<"data: " << data.substr(i)<<endl;
            }
        }
    return std::make_tuple(EN_CUR_TAG, i);
}

// 
std::string TagBuilder::makeContent(const std::string &data)
{
    std::vector<string> tagNamestack;
    auto size = data.size();
    size_t i = 0;
    for(; i < size; i++){
        // it's a tag
        if(data[i] == '<' 
           && data[i+1] != '!'
           &&data[i+1] != '/'){
            auto tagname = makeTagName(data.substr(i+1));
            if(std::get<0>(tagname).empty())
            {
                cout<<"error!"<<endl;
                continue;
            }
            cout<<"new tag "<<std::get<0>(tagname)<<endl;
            tagNamestack.push_back(std::get<0>(tagname));
            i += std::get<1>(tagname)-1;
        }

        // it's tag's end
        if(data[i] == '<' 
           && data[i+1] == '/'){
            auto tagname = makeTagName(data.substr(i+2));
            if(std::get<0>(tagname).empty())
            {
                continue;
            }
            auto name = std::get<0>(tagname);
            cout<<"new end tag "<< name<<endl;
            i += std::get<1>(tagname)+name.size()+1;
            while(std::get<0>(tagname) != tagNamestack.back()){
                cout<<"pop "<< tagNamestack.back()<<endl;;
                tagNamestack.pop_back();
            }
            tagNamestack.pop_back();
           
            if(tagNamestack.empty())
            {
                break;
            }
        }
    }
    return data.substr(0, i);
}
TinyResult TagBuilder::makeTagName(const std::string &data)
{
    std::string tagName;
    auto size = data.size();
    size_t i = 0;
    for(; i < size; i++){
        if(data[i] == '>' || data[i] ==' '){
            break;
        }
        tagName += data[i];
    }
    return std::make_tuple(tagName, i);
}

std::string TagBuilder::makePattern( const std::string &tagName,
                                     const std::vector<Attribute> &attrs) const
{
    std::string patternStr;
    if(attrs.empty())
    {
        patternStr = "<"+tagName;
    }
    else{
        patternStr = "<"+tagName+" +";
    }
    for(auto item:attrs){
        patternStr += item.name+" *";
        patternStr += "= *";
        patternStr += "\""+item.value+"\" *";
    }
    patternStr +=".*>";
    return patternStr;

}
std::string TagBuilder::trimTagLabel(std::string &tagText)
{
    auto pos = tagText.find_first_of("\r\n");
    while(pos != tagText.npos)
    {
        tagText = tagText.erase(pos,1);
        pos = tagText.find_first_of("\r\n", pos);
    }
    pos = tagText.find_first_of(">");
    if(pos != tagText.npos){
        auto pose = tagText.find_last_of("</");
        if(pose != tagText.npos)
        {
            return Trim(tagText.substr(pos+1, pose-pos-2));
        }
    }
    /*   
    std::string patternStr = ">.*<";                     
    const std::regex pattern(patternStr);
    sregex_token_iterator sit(tagText.begin(),tagText.end(),pattern);
    sregex_token_iterator send ;
    while(sit != send)
    {
        auto text = sit->str().substr(1,sit->str().size()-2);
        auto texts = Trim(text);
        cout<<texts<<endl;
        return Trim(text);
    }*/
    return tagText;
}

TinyResult TagBuilder::extractContentText(const std::string &data , 
                                            int pos,
                                            std::shared_ptr<TinyTag> &tinyTag)
{
    cout<<"extractContentText data: "<<data<<" pos: "<<pos<<endl;
    // check has end Tag
    const auto size = data.size(); 
    size_t i = 0;
    for(; i < size; i++)
    {
        if(data[i] == '<' && data[i+1] == '/'){
            auto ret = endTag(data.substr(i));
            if(std::get<0>(ret)!=EN_NULL)
            {
                return std::make_tuple(std::get<0>(ret), i+pos+std::get<1>(ret));
            }else{
                i = std::get<1>(ret);
            }
        }
        
        if(data[i] == '<' 
           && data[i+1] != '!'
           &&data[i+1] != '/'){
                auto ret = newTag(data.substr(i), tinyTag);
                i += std::get<1>(ret);
           }
    }
    return std::make_tuple(EN_CUR_TAG, i+pos);
}

TinyResult TagBuilder::extractDocument(const std::string &data)
{
    const size_t size = data.size();
    std::string result;
    size_t i = 0;
    for(; i < size; i++)
    {
        if(data[i] == '>'){
            break;
        }
        result += data[i];
    }
    
    return std::make_tuple(result, i);
}
const std::vector<std::shared_ptr<TinyTag>>& TagBuilder::getTags() const{
    return _tags;
}
TinyResult TagBuilder::extractTagName( const std::string &data )
{
    std::string tagName;
    const size_t size = data.size();
    bool isTag = false;
    for(size_t i=0; i<size;i++)
    {
        if(data[i] == '<'){
            if(i+1 < size && data[i+1] != '!')
            {// it's a tag begin
                isTag = true;
                continue;
            }else{
                if(i+2 < size 
                   && i+3 < size 
                   && data[i+2]=='-'
                   && data[i+3]=='-'){
                   // comments
                   
                   }else{
                    // document
                    
                    auto ret = extractDocument(data.substr(i+1));
                    if(ret != NullResult)
                    {
                        cout<<"comments is "<<std::get<0>(ret)<<endl;
                        i = std::get<1>(ret);
                    }
                }
                
            }
        }

        if( data[i] == ' '|| data[i] == '>'){
            if(isTag){
                isTag = false;
                
                return std::make_tuple(tagName, i);
                
            }
        }
        if(isTag){
            tagName += data[i];
            continue;
        }
    }
    return std::make_tuple(EN_NULL,-1);
}

///////////////////////////////


TinyTag::TinyTag(const std::string &name)
:_name(name){}

TinyTag::TinyTag(){}

TinyTagPtr TinyTag::find(const std::string &tagName,
                         const std::vector<Attribute> &attrs)
{
    assert(!_data.empty());
    auto tagBuilder = TagBuilder::Instance();
    auto patternStr = tagBuilder.makePattern(tagName, attrs);
    cout<<"patternStr = "<<patternStr<<endl;
    std::smatch result;
    const std::regex pattern(patternStr);
    bool match = std::regex_search(_data, result, pattern);
    if(match)
    {
        auto tmpdata = _data.substr(result.position());
        auto contentText = tagBuilder.makeContent(tmpdata);
        auto tinyTag = std::shared_ptr<TinyTag>(new TinyTag(tagName));
        tinyTag->setContentText(tagBuilder.trimTagLabel(contentText));
        return tinyTag;
    }

    return TinyTagPtr();
}
std::string TinyTag::getValue(){
    std::string result;
    for(auto c:_data)
    {
        if(c=='<'){
            break;
        }
        result += c;
    }
    
    return Trim(result);
}
void TinyTag::setContentText( const std::string &data )
{
    _data = data;
}

std::string TinyTag::getContentText()const{
    return _data;
}

TinyTagPtrs TinyTag::findAll(const std::string& tagName,
                            const std::vector<Attribute> &attrs)
{
    auto tagBuilder = TagBuilder::Instance();
    auto patternStr = tagBuilder.makePattern(tagName, attrs);
    const std::regex pattern(patternStr);
    
    std::smatch result;
    std::string::const_iterator it = _data.cbegin();
    std::string::const_iterator end = _data.cend();
    std::vector<string> tagsText;
    size_t pos = -1;
    while(std::regex_search(it, end, result, pattern))
    {
        auto dis = std::distance(_data.cbegin(), result[0].first);
        if(pos != (size_t)-1)
        {
            tagsText.push_back(_data.substr(pos, dis - pos));
            
        }
        pos = dis;
        it =result[0].second ;
    }
    if(pos == (size_t)-1)
    {
        return TinyTagPtrs();
    }
    tagsText.push_back(_data.substr(pos));
    TinyTagPtrs tinyTagPtrs;
    
    for(size_t i=0; i < tagsText.size(); i++)
    {
        // create Tag
        auto tag = TinyTagPtr(new TinyTag(tagName));
        auto contentText = tagBuilder.makeContent(tagsText[i]);
        tag->setContentText(tagBuilder.trimTagLabel(contentText));
        tinyTagPtrs.push_back(tag);
    }
    return tinyTagPtrs;
}
std::string TinyTag::getAttribute(const std::string& attrName) const
{
    auto it = _attrs.find(attrName);
    if(it != _attrs.end()) return it->second;
    return "";
}
std::string TinyTag::getName() const
{return _name;}
std::string TinyTag::getText() const
{return "";}

void TinyTag::setAttrs(const std::map<std::string, std::string> &attrs){
    _attrs = attrs;
}

void TinyTag::printAttrs() const
{
    auto it=_attrs.begin();  
    for(;it !=_attrs.end();++it) {
      cout<<_name<<" print attrs: "<<it->first<<" = "<<it->second<<endl;
    }
}

const std::vector<TinyTagPtr>& TinyTag::childrens()const
{
    return _childrens;

}

void TinyTag::addChild( const std::shared_ptr<TinyTag> &tg){
    _childrens.push_back(tg);

}
/////////////////////////////////////////////////
HTML::HTML(){}

HTML::HTML(const string& content):_data(content)
{
    TagBuilder::Instance().setData(content);
}

TinyTagPtr HTML::dump()
{
    _rootTag = TagBuilder::Instance().dump();
    return _rootTag;
}

TinyTagPtr HTML::find(const std::string &tagName,
                      const std::vector<Attribute> &attrs)
{

    std::string patternStr = "<"+tagName+" +";
    for(auto item:attrs){
        patternStr += item.name+" *";
        patternStr += "= *";
        patternStr += "\""+item.value+"\" *";
    }
    patternStr +=".*>";

    patternStr = TagBuilder::Instance().makePattern(tagName, attrs);
    cout<<patternStr<<endl;
    std::smatch result;
    const std::regex pattern(patternStr);
    bool match = std::regex_search(_data, result, pattern);
    if(match)
    {
        auto tagBuilder = TagBuilder::Instance();
        auto tmpdata = _data.substr(result.position());
        auto contentText = tagBuilder.makeContent(tmpdata);
        auto tinyTag = std::shared_ptr<TinyTag>(new TinyTag(tagName));
        tinyTag->setContentText(contentText);
        return tinyTag;
    }

    return nullptr;
}

TinyTagPtr HTML::findAll(const std::string& tagName,
                         const std::vector<Attribute> &attrs)
{
    TagBuilder::Instance().dump();//.buildTinyTag(tagName, _content);
    return nullptr;
}
void HTML::setRootTag(const std::shared_ptr<TinyTag> &rootTag)
{
    _rootTag = rootTag;
}
TinyTagPtr HTML::getRootTag() const
{
    return _rootTag;
}
void HTML::showTags()
{
    auto tags = TagBuilder::Instance().getTags();
     
    for(auto it: tags)
    {
        cout<<it->getName()<<endl;
        continue;
    }
}
