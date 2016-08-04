#ifndef TINYHTML_H
#define TINYHTML_H
#include <unordered_map>
#include <map>
#include <string>
#include <memory>
#include <tuple>
#include <vector>
 // // type defines

namespace TinyHTML
{    
    struct Attribute;
    class TinyTag;
    typedef std::tuple<std::string, int> TinyResult;
    typedef std::shared_ptr<TinyTag> TinyTagPtr;
    typedef std::vector<TinyTagPtr> TinyTagPtrs;
    #define EN_CUR_TAG "EN_CURTAG"
    #define EN_NULL "EN_NULL"
    #define EN_BACK "EN_BACK"
    
    class TinyTag
    {
        public:
            TinyTag();
            TinyTag(const std::string &name);
            std::string getValue();
            TinyTagPtr find( const std::string &tagName,
                             const std::vector<Attribute> &attrs=std::vector<Attribute>());
            TinyTagPtrs findAll( const std::string &tagName,
                                const std::vector<Attribute> &attrs=std::vector<Attribute>());
            
            std::string getAttribute(const std::string& attrName) const;
            std::string getName() const;
            std::string getText() const;
            void addChild(const TinyTagPtr &);
            void setContentText(const std::string &data);
            std::string getContentText() const;
            void setAttrs(const std::map<std::string, std::string> &attrs);
            std::map<std::string,std::string> getAttrs() const;
            void printAttrs() const;
            const std::vector<TinyTagPtr>& childrens()const;
        protected:
            std::string _name;
            // begin index ,chars count
            std::string _data;
            std::tuple<int, int> _content;
            std::map<std::string,std::string> _attrs;
            std::vector<std::shared_ptr<TinyTag>> _childrens;
    };

    class TagBuilder{
            public:
            std::string makeContent(const std::string &data);
            TinyResult makeTagName(const std::string &data);
            std::string makePattern(const std::string &tag,
                                    const std::vector<Attribute> &attrs) const;
            std::string trimTagLabel(std::string &tagText);
            
        private:
            void pushTagsInfo(const TinyResult &tr);
            void popTagsInfo();
        public:
            static TagBuilder& Instance(){return tagBuilder;};
            std::shared_ptr<TinyTag> buildTinyTag(const std::string &tag,
                                                  const std::string &content) ;
            TinyResult extractTagAttrsText(const std::string &content);
            int extractTag(const std::string &content,
                           int position,
                           std::shared_ptr<TinyTag> &tinyTag);
            std::map<std::string, std::string> extractAttrs(const std::string &attrsText);
            std::shared_ptr<TinyTag> extractChildren(const std::string &data);
            TinyResult extractContentText(const std::string &data,
                                          int position,
                                          std::shared_ptr<TinyTag> &);
            TinyResult extractTagName(const std::string &data);
            TinyResult extractEndTagName(const std::string &data) const;
            TinyResult brother(const std::string &data);
            TinyResult extractDocument(const std::string &data);
            bool hasEndTag(const std::string &data) const;
            void setData(const std::string &data);
            const std::string& getData() const {return _data;};
            std::shared_ptr<TinyTag> dump(const std::string &data);
            std::shared_ptr<TinyTag> dump();
            const std::vector<std::shared_ptr<TinyTag>>& getTags() const;
            TinyResult endTag(const std::string &data);
            TinyResult newTag(const std::string &data,TinyTagPtr &tinyTag);
        private:
            static TagBuilder tagBuilder;
            std::vector<std::shared_ptr<TinyTag>> _tags;
            std::string _data;

        private:
            std::vector<TinyResult> _tmpTagsName;
    };


    struct Attribute{
        std::string name;
        std::string value;
    };
    enum EfindType{
        eSingle,
        eAll
    };
    struct TagInfo
    {
        EfindType findType;
        std::string name;
        std::vector<Attribute> attributs;
    };

    class HTML{
    public:
        HTML();
        HTML(const std::string &name);
        HTML(const std::string &name, const std::string& content);
        TinyTagPtr find(const std::string &tagName,
                      const std::vector<Attribute> &attrs);
        TinyTagPtr findAll( const std::string&tagName,
                          const std::vector<Attribute> &attrs);
        void setRootTag(const std::shared_ptr<TinyTag> &rootTag);
        std::shared_ptr<TinyTag> getRootTag() const;
        void showTags();
        TinyTagPtr dump();
    private:
        std::string _data;
        std::shared_ptr<TinyTag> _rootTag;
    };
    
   
}
#endif
