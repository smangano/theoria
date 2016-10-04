#include "theoria/util/densemap.h"
#include <gtest/gtest.h>

using namespace theoria ;
using namespace theoria::util ;

class DensemapTest : public ::testing::Test 
{
public:
    
    using densemap_str = densemap<int, std::string> ;

protected:

    virtual void SetUp() {
        _dmDefault = new densemap_str() ;        
        _dmCopyDef = new densemap_str(*_dmDefault) ;        
        _dmFromIters = new densemap_str(_strings.begin(), _strings.end()) ;
        _dmCopyFromIters = new densemap_str(*_dmFromIters) ;        
    }

    virtual void TearDown() {
        delete _dmDefault ;
        delete _dmCopyDef ;
        delete _dmFromIters ;
        delete _dmCopyFromIters ;
    }


    
    std::vector<std::string> _strings = {"A", "B", "C"} ;
    densemap_str * _dmDefault ;
    densemap_str * _dmCopyDef ;
    densemap_str * _dmFromIters ;
    densemap_str * _dmCopyFromIters ;
   
    std::vector<densemap_str::value_type> _vecIl { {2, "two"}, {7, "seven"}, {17, "seventeen"} } ; 
    densemap_str _dmIl { {2, "two"}, {7, "seven"}, {17, "seventeen"} } ;
    
} ;

TEST_F(DensemapTest, TestCtors) {
    ASSERT_EQ(_dmDefault->size(), 0) ; 
    ASSERT_EQ(_dmDefault->begin(), _dmDefault->end()) ; 
    ASSERT_EQ(_dmCopyDef->size(), 0) ; 
    ASSERT_EQ(_dmCopyDef->begin(), _dmCopyDef->end()) ; 
    ASSERT_EQ(_dmFromIters->size(), _strings.size()) ; 
    ASSERT_EQ(_dmCopyFromIters->size(), _strings.size()) ; 

    ASSERT_EQ(_dmIl.size(), 3) ;
}

TEST_F(DensemapTest, TestAt) {
    ASSERT_THROW(_dmDefault->at(0), std::out_of_range) ;
    ASSERT_EQ(_dmFromIters->at(0), _strings.at(0)) ;
    ASSERT_EQ(_dmFromIters->at(1), _strings.at(1)) ;
    ASSERT_EQ(_dmFromIters->at(2), _strings.at(2)) ;
    ASSERT_THROW(_dmFromIters->at(3), std::out_of_range) ;

    ASSERT_THROW(_dmIl.at(0), std::out_of_range) ;
    ASSERT_THROW(_dmIl.at(1), std::out_of_range) ;
    ASSERT_EQ(_dmIl.at(2), std::string("two")) ;
    ASSERT_EQ(_dmIl.at(7), std::string("seven")) ;
    ASSERT_THROW(_dmIl.at(8), std::out_of_range) ;
    ASSERT_THROW(_dmIl.at(16), std::out_of_range) ;
    ASSERT_EQ(_dmIl.at(17), std::string("seventeen")) ;
    ASSERT_THROW(_dmIl.at(18), std::out_of_range) ;
}

TEST_F(DensemapTest, TestIter) {
    auto iter = _dmFromIters->begin() ;
    auto end = _dmFromIters->end() ;
    auto iter2 = _strings.begin() ;
    auto end2 = _strings.end() ;

    while(iter != end && iter2 != end2) {
        ASSERT_EQ(iter->second, *iter2) ;
        ++iter ; 
        ++iter2 ;
    }
    ASSERT_TRUE(iter == end) ;
    ASSERT_TRUE(iter2 == end2) ;

    iter = _dmIl.begin() ;
    end = _dmIl.end() ;
    auto iter3 = _vecIl.begin() ;
    auto end3 = _vecIl.end() ;
    while(iter != end && iter3 != end3) {
        ASSERT_EQ(iter->first, iter3->first) ;
        ASSERT_EQ(iter->second, iter3->second) ;
        ++iter ; 
        ++iter3 ;
    }
    ASSERT_TRUE(iter == end) ;
    ASSERT_TRUE(iter3 == end3) ;
}

TEST_F(DensemapTest, TestConstIter) {
    auto iter = _dmFromIters->cbegin() ;
    auto end = _dmFromIters->cend() ;
    auto iter2 = _strings.cbegin() ;
    auto end2 = _strings.cend() ;

    while(iter != end && iter2 != end2) {
        ASSERT_EQ(iter->second, *iter2) ;
        ++iter ; 
        ++iter2 ;
    }
    ASSERT_TRUE(iter == end) ;
    ASSERT_TRUE(iter2 == end2) ;
}
